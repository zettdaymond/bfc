#include "compiller.h"

#include <cstdio>
#include <cstdlib>
#include <stack>
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <elf.h>
#include <stddef.h>

#include "utils.h"

std::string asmTemplate {""};
std::vector<std::string> asmTemplates;

#define LOAD_ADDRESS 0x8048000
#define DUMP_VAR( V ) std::cout << (#V) << " : " << V << std::endl;

struct Binary {
  Elf32_Ehdr e_hdr;
  Elf32_Phdr text_hdr;
  Elf32_Phdr bss_hdr;
//  char padding[12 + 64];
  char code[];
};

struct Footer {
    char shstrtab[23];
    Elf32_Shdr dummy;
    Elf32_Shdr text_shdrs;
    Elf32_Shdr bss_shdrs;
    Elf32_Shdr strtab;
};

Footer createFooter(unsigned sourceSize)
{
    Footer f {

        {"\0.shstrtab\0.text\0.bss\0"},

        { 0, SHT_NULL, 0, 0, 0, 0, SHN_UNDEF, 0, 0, 0 },
        /* .note.gnu.build-id */
//        { 11, SHT_NOTE, SHF_ALLOC, ADDR_TEXT + offsetof(elf, note_gnu_build_id),
//                    offsetof(elf, note_gnu_build_id), sizeof foo.note_gnu_build_id,
//                    SHN_UNDEF, 0, sizeof(Elf32_Word), 0 },
        /* .text */
        {
            30,
            SHT_PROGBITS,
            SHF_EXECINSTR | SHF_ALLOC,
            LOAD_ADDRESS + ( offsetof (struct Binary, code) ),  //sh_addr
            ( offsetof(struct Binary, code) ), //sh_offset
            sourceSize,
            SHN_UNDEF,
            0,
            0x10,
            0
        },
        /* .bss */
        {
            36,
            SHT_NOBITS,
            SHF_WRITE | SHF_ALLOC,
            LOAD_ADDRESS + sizeof(struct Binary) + sourceSize + 2,
            offsetof(struct Binary, code) + sourceSize + 2,//sh_offset
            0x1d4c0,       //size
            SHN_UNDEF,                  //link
            0,                          //info
            sizeof(Elf32_Addr),         //addralign
            0                           //entsize
        },
        /* .shstrtab */
        {
            1,
            SHT_STRTAB,
                    0,
                    0,
                    sizeof(struct Binary) + sourceSize ,
                    sizeof (f.shstrtab),
                    SHN_UNDEF,
                    0,
                    1,
                    0
        }
    };
    return f;
}

Binary createBinary()
{
    Binary bin = {
        /* ELF HEADER */
        /*.ehdr = */
        {
          /* general */

          /*.e_ident   = */ {
                ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,
                ELFCLASS32,
                ELFDATA2LSB,
                EV_CURRENT,
                0,
              },
          /*.e_type    =*/  ET_EXEC,
          /*.e_machine = */ EM_386,
          /*.e_version = */ EV_CURRENT,
          /*.e_entry   = */ LOAD_ADDRESS + ( offsetof (struct Binary, code) ),
          /*.e_phoff   = */ offsetof (struct Binary, text_hdr),
          /*.e_shoff   = */ 0,
          /*.e_flags   = */ 0,
          /*.e_ehsize  = */ sizeof (Elf32_Ehdr),

          /* program header */
          /*.e_phentsize = */sizeof (Elf32_Phdr),
          /*.e_phnum     = */2,

          /* section header */
          /*.e_shentsize = */sizeof (Elf32_Shdr),
          /*.e_shnum     = */4,
          /*.e_shstrndx  = */3
        },

        /* PROGRAM HEADER .text */
        /*.phdr =*/ {
          /*.p_type   =*/ PT_LOAD,
          /*.p_offset =*/ 0,
          /*.p_vaddr = */ LOAD_ADDRESS,
          /*.p_paddr = */ LOAD_ADDRESS,
          /*.p_filesz = */0,
          /*.p_memsz = */ 0,
          /*.p_flags = */ PF_R | PF_X,
          /*.p_align = */ 0x1000
        },

        /* PROGRAM HEADER .bss*/
        /*.phdr =*/ {
          /*.p_type   =*/ PT_LOAD,
          /*.p_offset =*/ 0,
          /*.p_vaddr = */ 0,
          /*.p_paddr = */ 0,
          /*.p_filesz = */0,
          /*.p_memsz = */ 0,
          /*.p_flags = */ PF_R | PF_W,
          /*.p_align = */ 0x1000
        },
      };
    return bin;
}


unsigned startOfTextSegmentAdr()
{
    return  LOAD_ADDRESS + (offsetof (struct Binary, code)) ;
}

std::array<char, 4> splitToBytes( unsigned n )
{
    std::array<char, 4> bytes;

    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
    return bytes;
}

void dump( std::vector<char>& v) {
    std::cout << "Dump : ";
    for(auto c : v) {
        std::cout << hex(c)  << " ";
    }
    std::cout << "\t|\tSIZE: " << v.size() << std::endl << std::endl;
}

void compile(std::string source, std::string outPath)
{
    /* if src file exists
     * generate asembly file for it
     * then generate elf/exe file for it.
     *
     * does not support linker/object files. just simple executable creator.
     */

    std::vector<char> bin_out;

    //insert opcodes that install .bss adr in esi registry
    push_back_array(bin_out, {'\xBE','\x00','\x00','\x00','\x00'} );

    //to track shifts
    std::stack<unsigned> stack;

    for(auto token : source) {
        if(token == '>') {
            //add esi, 4
            push_back_array(bin_out, {'\x83','\xC6','\x04'} );
            DUMP_VAR(bin_out.size());
            dump(bin_out);
            continue;
        }
        if(token == '<') {
            //sub esi, 4
            push_back_array(bin_out, {'\x83','\xEE','\x04'} );
            DUMP_VAR(bin_out.size());
            dump(bin_out);
            continue;
        }
        if (token == '+') {
            //add dword [esi], 1
            push_back_array(bin_out, {'\x83','\x06','\x01'} );
            DUMP_VAR(bin_out.size());
            dump(bin_out);
            continue;
        }
        if (token == '-') {
            //sub dword [esi], 1
            push_back_array(bin_out, {'\x83','\x2E','\x01'} );
            DUMP_VAR(bin_out.size());
            dump(bin_out);
            continue;
        }
        if (token == '.') {
            //mov eax, 3
            push_back_array(bin_out, {'\xB8','\x04','\x00','\x00','\x00'} );
            //mov ebx, 1
            push_back_array(bin_out, {'\xBB','\x01','\x00','\x00','\x00'} );
            //mov ecx, esi
            push_back_array(bin_out, {'\x89','\xF1'} );
            //mov edx, 1
            push_back_array(bin_out, {'\xBA','\x01','\x00','\x00','\x00'} );
            //int 0x80
            push_back_array(bin_out, {'\xCD','\x80'} );
            DUMP_VAR(bin_out.size());
            dump(bin_out);
            continue;
        }
        if (token == '[') {
            //w_num:
            stack.push(bin_out.size());
            //cmp dword [esi], 0
            push_back_array(bin_out, {'\x83','\x3E','\x00'} );
            //je near ew_NUM (fill by temporary adress 00000000)
            push_back_array(bin_out, {'\x0F','\x84','\x00','\x00','\x00', '\00'} );
            DUMP_VAR(bin_out.size());
            dump(bin_out);
            continue;
        }
        if (token == ']') {
            //jmp near w_num
            std::cout << "TOKEN ']' : " << std::endl;
            push_back_array(bin_out, {'\xE9','\x00','\x00','\x00','\x00'} );
            DUMP_VAR(bin_out.size());

            //FIND JMP ADRESSES

            //FIRST: Find shifts to begin and end of the loop needed by je jmp
            //bin_out.size() contains shift that coresponds end of the loop

            //SECOND:
            //in the top of the 'stack' we store shift that corresponds to start of this loop
            auto lStart = stack.top();
            stack.pop();
            DUMP_VAR(lStart);

            //THIRD: Find shift between NEXT AFTER jmp instruction and begin of the loop
            // this is a negative number means how many bytes we need to jump BACK
            // relative to this position
            int jmpShift = lStart - bin_out.size();
            DUMP_VAR(jmpShift);

            //split by bytes and fill by little endian encoding rules
            DUMP_VAR(bin_out.size());
            auto jmpShiftBytes = splitToBytes(jmpShift);
            bin_out[bin_out.size() - 1] = jmpShiftBytes[0];
            bin_out[bin_out.size() - 2] = jmpShiftBytes[1];
            bin_out[bin_out.size() - 3] = jmpShiftBytes[2];
            bin_out[bin_out.size() - 4] = jmpShiftBytes[3];

            std::cout << "start Adr Bytes: ";
            for(auto c: jmpShiftBytes) {
                std::cout << hex( c ) << " ";
            }
            std::cout <<std::endl;

            //FOURTH: Calculate the shift between NEXT AFTER the je instruction and the end of the loop
            // 9 corresponds number of bytes cmp instruction + je instruction itself.
            unsigned jeShift = bin_out.size() - ( lStart + 9 );

            //split by bytes and fill by little endian encoding rules
            auto jeShiftBytes = splitToBytes(jeShift);

            //move forward to skip 3 cmp instruction bytes and 2 je opcode bytes
            lStart += 5;

            bin_out[lStart]     = jeShiftBytes[3];
            bin_out[lStart + 1] = jeShiftBytes[2];
            bin_out[lStart + 2] = jeShiftBytes[1];
            bin_out[lStart + 3] = jeShiftBytes[0];

            std::cout << "end Adr Bytes: ";
            for(auto c: jeShiftBytes) {
                std::cout << hex( c ) << " ";
            }
            std::cout <<std::endl;

            dump(bin_out);
            continue;
        }
        printf("Could not recognize token : '%c' \n", token);
    }

    //add return opcodes
    //mov eax. 1 B801000000
    push_back_array(bin_out, {'\xB8','\x01','\x00','\x00','\x00'} );
    //mov ebx. 0 BB00000000
    push_back_array(bin_out, {'\xBB','\x00','\x00','\x00','\x00'} );
    //int 0x80
    push_back_array(bin_out, {'\xCD', '\x80'} );

    //calculate adress that starts immediately after our .tex segment
    //this is an andress of .bss segment

    unsigned bssAdr = LOAD_ADDRESS + sizeof(Binary) + bin_out.size();
    DUMP_VAR(bssAdr);

    //replace undifined .bss segment adress with calculated.
    //follow little endiad rules

    auto bssAdrBytes = splitToBytes(bssAdr);
    bin_out[1] = bssAdrBytes[3];
    bin_out[2] = bssAdrBytes[2];
    bin_out[3] = bssAdrBytes[1];
    bin_out[4] = bssAdrBytes[0];
    dump(bin_out);

    //Write output to elf
    std::ofstream out(outPath, std::ios::binary);

    Binary bin = createBinary();

    bin.text_hdr.p_memsz = sizeof (struct Binary) + bin_out.size();
    bin.text_hdr.p_filesz = sizeof (struct Binary) + bin_out.size();

    bin.bss_hdr.p_offset = (offsetof (struct Binary, code) ) + bin_out.size() +2;
    bin.bss_hdr.p_memsz = 0x1D4C0; //30'000 * 4
    bin.bss_hdr.p_filesz = 0;
    bin.bss_hdr.p_paddr = LOAD_ADDRESS + (offsetof (struct Binary, code) ) + bin_out.size() + 2;
    bin.bss_hdr.p_vaddr = LOAD_ADDRESS + (offsetof (struct Binary, code) ) + bin_out.size() + 2;

    bin.e_hdr.e_shoff = sizeof( struct Binary ) + bin_out.size() +
            offsetof (struct Footer, dummy);

    DUMP_VAR( sizeof(Footer::shstrtab) )
    out.write((char*)&bin, sizeof(bin));
    out.write((char*)&bin_out[0], bin_out.size()* sizeof(char));

    Footer footer= createFooter(bin_out.size());
//    char term[2] = {'0', 'E'};
    out.write((char*)&footer, sizeof(footer));
    out.close();

}

void init()
{
    std::stringstream buffer;

    std::ifstream templateFile("template_linux.asm");
    buffer << templateFile.rdbuf();
    templateFile.close();
    asmTemplate = buffer.str();
    buffer.str(""); //clear buffer;

    std::ifstream cmdFile("command_linux.asm");
    buffer << cmdFile.rdbuf();
    templateFile.close();

    asmTemplates = split(buffer.str(), '/');
}

std::string assembly(std::string source)
{
    init();

    std::string cmds = "";

    unsigned lastID = 0;
    std::stack<unsigned> stack;

    for(auto token : source) {
        if(token == '>') {
            cmds.append( asmTemplates[Operations::INC_PTR] );
            continue;
        }
        if(token == '<') {
            cmds.append( asmTemplates[Operations::DEC_PTR] );
            continue;
        }
        if (token == '+') {
            cmds.append( asmTemplates[Operations::INC_VALUE] );
            continue;
        }
        if (token == '-') {
            cmds.append( asmTemplates[Operations::DEC_VALUE] );
            continue;
        }
        if (token == '.') {
            cmds.append( asmTemplates[Operations::PUT_CHAR] );
            continue;
        }
        if (token == '[') {
            auto templ = asmTemplates[Operations::START_LOOP];

            stack.push(lastID);
            replaceAll( templ, "NUM", std::to_string(lastID) );
            lastID++;

            cmds.append( templ );
            continue;
        }
        if (token == ']') {
            auto templ = asmTemplates[Operations::END_LOOP];

            auto id = stack.top();
            stack.pop();
            replaceAll( templ, "NUM", std::to_string(id) );
            lastID++;

            cmds.append( templ );
            continue;
        }
        printf("Could not recognize token : '%c' \n", token);
    }


    if (stack.size() != 0) {
        printf(" Number of '[' does not equal to number of ']'");
        std::exit(255);
    }

    auto ret = asmTemplate;
    replaceAll(ret, "{SOURCE}", cmds);
    ret.append("\n");

    return ret;
}
