#include "../../compiller.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stack>
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <stddef.h>
#include <windows.h>

#include "../../utils.h"

#define BFC_WIN_SECTION_NUM 3
#define BFC_WIN_CODE_RWA 0x1000
#define BFC_WIN_SECTION_ALIGN 0x1000
#define BFC_WIN_FILE_ALIGN 0x200
#define BFC_WIN_VIRTUAL_LOAD_ADRESS 0x40000000

#define LOAD_ADDRESS 0x8048000
#define BSS_MEM_SIZE 0x1D4C0 /*30'000 * 4*/

namespace Compiller {

template<class T>
T roundToAlign( T v, T align) {
    return v + v % align;
}

struct ASMTemplates {
    std::string src;
    std::vector< std::string > cmds;
};


 struct Binary {
   IMAGE_DOS_HEADER dos_Header; //DOS Header
   IMAGE_NT_HEADERS32 pe_Header;  //PE HEader
   IMAGE_SECTION_HEADER scn_Headers[3]; //.text, .rdata, .bss section headers;
   char code[];
 };


 Binary createBinary(unsigned code_size)
 {
     unsigned sizeOfImage;
     unsigned RDATA_RWA  = roundToAlign(BFC_WIN_CODE_RWA + code_size, BFC_WIN_SECTION_ALIGN);

     Binary bin {
         /* DOS_HEADER */ {
             IMAGE_DOS_SIGNATURE, //e_magick
             0, //WORD e_cblp
             0, //WORD e_cp
             0, //WORD e_crlc
             0, //WORD e_cparhdr
             0, //WORD e_minalloc;
             0, //WORD e_maxalloc;
             0, //WORD e_ss;
             0, //WORD e_sp;
             0, //WORD e_csum
             0, //WORD e_ip
             0, //WORD e_cs
             0, //WORD e_lfarlc
             0, //WORD e_ovno
             {0,0,0,0},//WORD e_res[4];
             0,//WORD e_oemid;
             0,//WORD e_oeminfo;
             {0},//WORD e_res2[10];
             offsetof( Binary, pe_Header )//LONG e_lfanew;
         },
         /* PE Header */ {
             IMAGE_NT_SIGNATURE,    //DWORD Signature;

             /* IMAGE_FILE_HEADER */ {
                  IMAGE_FILE_MACHINE_I386,           //WORD Machine;
                  BFC_WIN_SECTION_NUM,      //WORD NumberOfSections;
                  cast_to_U32( std::time(0) ),  //DWORD TimeDateStamp;
                  0,                            //DWORD PointerToSymbolTable;
                  0,                            //DWORD NumberOfSymbols;
                  sizeof( Binary::pe_Header.OptionalHeader) , //WORD SizeOfOptionalHeader;
                  IMAGE_FILE_32BIT_MACHINE | IMAGE_FILE_EXECUTABLE_IMAGE, //WORD Characteristics;
             },

             /* OPTIONAL_HEADER32 */ {
                 IMAGE_NT_OPTIONAL_HDR32_MAGIC, /*WORD Magic;*/
                 0,/*BYTE MajorLinkerVersion;*/
                 0,/*BYTE MinorLinkerVersion;*/
                 code_size,/*DWORD SizeOfCode;*/
                 0,/*DWORD SizeOfInitializedData;*/
                 0,/*DWORD SizeOfUninitializedData;*/
                 BFC_WIN_CODE_RWA,/*DWORD AddressOfEntryPoint;*/
                 BFC_WIN_CODE_RWA,/*DWORD BaseOfCode;*/
                 0,/*DWORD BaseOfData;*/
                 BFC_WIN_VIRTUAL_LOAD_ADRESS,/*DWORD ImageBase;*/
                 BFC_WIN_SECTION_ALIGN,/*DWORD SectionAlignment;*/
                 BFC_WIN_FILE_ALIGN,/*DWORD FileAlignment;*/
                 0,/*WORD MajorOperatingSystemVersion;*/
                 0,/*WORD MinorOperatingSystemVersion;*/
                 0,/*WORD MajorImageVersion;*/
                 0,/*WORD MinorImageVersion;*/
                 4,/*WORD MajorSubsystemVersion;*/
                 0,/*WORD MinorSubsystemVersion;*/
                 0,/*DWORD Win32VersionValue;*/
                 sizeOfImage,/*DWORD SizeOfImage;*/
                 0x200,/*DWORD SizeOfHeaders;*/
                 0,/*DWORD CheckSum;*/
                 IMAGE_SUBSYSTEM_WINDOWS_CUI,/*WORD Subsystem;*/
                 0,/*WORD DllCharacteristics;*/
                 0,/*DWORD SizeOfStackReserve;*/
                 0,/*DWORD SizeOfStackCommit;*/
                 0,/*DWORD SizeOfHeapReserve;*/
                 0,/*DWORD SizeOfHeapCommit;*/
                 0,/*DWORD LoaderFlags;*/
                 IMAGE_NUMBEROF_DIRECTORY_ENTRIES,/*DWORD NumberOfRvaAndSizes;*/
                 {
                     /*IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];*/
                     /*IMAGE_DIRECTORY_ENTRY_EXPORT*/{
                         0,
                         0,
                     },
                     /*IMAGE_DIRECTORY_ENTRY_IMPORT*/{
                         RDATA_RWA,  /*DWORD VirtualAddress;*/
                         0,         /*DWORD Size;*/
                     }
                 }
             }
         },

         /* SECTION .text Header */ {
             {'.','t','e','x','t'}, /*BYTE Name[IMAGE_SIZEOF_SHORT_NAME];*/
             {
                 sizeof(Binary), /*DWORD PhysicalAddress;*/
                 code_size, /*DWORD VirtualSize;*/
             }, /* union Misc;*/
             BFC_WIN_CODE_RWA, /*DWORD VirtualAddress;*/
             roundToAlign(code_size, BFC_WIN_FILE_ALIGN),/*DWORD SizeOfRawData;*/
             roundToAlign(sizeof(Binary), BFC_WIN_FILE_ALIGN),/*DWORD PointerToRawData;*/
             0,/*DWORD PointerToRelocations;*/
             0,/*DWORD PointerToLinenumbers;*/
             0,/*WORD NumberOfRelocations;*/
             0,/*WORD NumberOfLinenumbers;*/
             0x00000020 | 0x60000000, /*READ, EXEC*/ /*DWORD Characteristics;*/
         },

         /* SECTION .rdata Header */ {
             {'.','r','d','a','t','a'}, /*BYTE Name[IMAGE_SIZEOF_SHORT_NAME];*/
             {
                 sizeof(Binary) + code_size, /*DWORD PhysicalAddress;*/
                 code_size, /*DWORD VirtualSize;*/
             }, /* union Misc;*/
             RDATA_RWA, /*DWORD VirtualAddress;*/
             roundToAlign(code_size, BFC_WIN_FILE_ALIGN),/*DWORD SizeOfRawData;*/
             roundToAlign(sizeof(Binary), BFC_WIN_FILE_ALIGN),/*DWORD PointerToRawData;*/
             0,/*DWORD PointerToRelocations;*/
             0,/*DWORD PointerToLinenumbers;*/
             0,/*WORD NumberOfRelocations;*/
             0,/*WORD NumberOfLinenumbers;*/
             0x00000020 | 0x60000000, /*READ, EXEC*/ /*DWORD Characteristics;*/
         }

     };
 }


std::string compile(const std::string& source)
{
    std::stringstream out;
    Binary bin = createBinary( 0 );
    out.write((char*)&bin, sizeof(bin));
    return out.str();
}
//     /* if src file exists
//      * generate asembly file for it
//      * then generate elf/exe file for it.
//      *
//      * does not support linker/object files. just simple executable creator.
//      */

//     std::vector<char> bin_out;

//     //insert opcodes that install .bss adr in esi registry
//     pushBackArray(bin_out, {'\xBE','\x00','\x00','\x00','\x00'} );

//     //to track shifts
//     std::stack<unsigned> stack;

//     for(auto token : source) {
//         if(token == '>') {
//             //add esi, 4
//             pushBackArray(bin_out, {'\x83','\xC6','\x04'} );
//             DUMP_VAR(bin_out.size());
//             DUMP_COLLECTION(bin_out);
//             continue;
//         }
//         if(token == '<') {
//             //sub esi, 4
//             pushBackArray(bin_out, {'\x83','\xEE','\x04'} );
//             DUMP_VAR(bin_out.size());
//             DUMP_COLLECTION(bin_out);
//             continue;
//         }
//         if (token == '+') {
//             //add dword [esi], 1
//             pushBackArray(bin_out, {'\x83','\x06','\x01'} );
//             DUMP_VAR(bin_out.size());
//             DUMP_COLLECTION(bin_out);
//             continue;
//         }
//         if (token == '-') {
//             //sub dword [esi], 1
//             pushBackArray(bin_out, {'\x83','\x2E','\x01'} );
//             DUMP_VAR(bin_out.size());
//             DUMP_COLLECTION(bin_out);
//             continue;
//         }
//         if (token == '.') {
//             //mov eax, 3
//             pushBackArray(bin_out, {'\xB8','\x04','\x00','\x00','\x00'} );
//             //mov ebx, 1
//             pushBackArray(bin_out, {'\xBB','\x01','\x00','\x00','\x00'} );
//             //mov ecx, esi
//             pushBackArray(bin_out, {'\x89','\xF1'} );
//             //mov edx, 1
//             pushBackArray(bin_out, {'\xBA','\x01','\x00','\x00','\x00'} );
//             //int 0x80
//             pushBackArray(bin_out, {'\xCD','\x80'} );
//             DUMP_VAR(bin_out.size());
//             DUMP_COLLECTION(bin_out);
//             continue;
//         }
//         if (token == '[') {
//             //w_num:
//             stack.push(bin_out.size());
//             //cmp dword [esi], 0
//             pushBackArray(bin_out, {'\x83','\x3E','\x00'} );
//             //je near ew_NUM (fill by temporary adress 00000000)
//             pushBackArray(bin_out, {'\x0F','\x84','\x00','\x00','\x00', '\00'} );
//             DUMP_VAR(bin_out.size());
//             DUMP_COLLECTION(bin_out);
//             continue;
//         }
//         if (token == ']') {
//             //jmp near w_num
//             pushBackArray(bin_out, {'\xE9','\x00','\x00','\x00','\x00'} );
//             DUMP_VAR(bin_out.size());

//             //FIND JMP ADRESSES

//             //FIRST: Find shifts to begin and end of the loop needed by je jmp
//             //bin_out.size() contains shift that coresponds end of the loop

//             //SECOND:
//             //in the top of the 'stack' we store shift that corresponds to start of this loop
//             auto lStart = stack.top();
//             stack.pop();
//             DUMP_VAR(lStart);

//             //THIRD: Find shift between NEXT AFTER jmp instruction and begin of the loop
//             // this is a negative number means how many bytes we need to jump BACK
//             // relative to this position
//             int jmpShift = lStart - bin_out.size();
//             DUMP_VAR(jmpShift);

//             //split by bytes and fill by little endian encoding rules
//             DUMP_VAR(bin_out.size());
//             auto jmpShiftBytes = splitToBytes(jmpShift);
//             bin_out[bin_out.size() - 1] = jmpShiftBytes[0];
//             bin_out[bin_out.size() - 2] = jmpShiftBytes[1];
//             bin_out[bin_out.size() - 3] = jmpShiftBytes[2];
//             bin_out[bin_out.size() - 4] = jmpShiftBytes[3];

//             DUMP_COLLECTION(jmpShiftBytes);

//             //FOURTH: Calculate the shift between NEXT AFTER the je instruction and the end of the loop
//             // 9 corresponds number of bytes cmp instruction + je instruction itself.
//             unsigned jeShift = bin_out.size() - ( lStart + 9 );

//             //split by bytes and fill by little endian encoding rules
//             auto jeShiftBytes = splitToBytes(jeShift);

//             //move forward to skip 3 cmp instruction bytes and 2 je opcode bytes
//             lStart += 5;

//             bin_out[lStart]     = jeShiftBytes[3];
//             bin_out[lStart + 1] = jeShiftBytes[2];
//             bin_out[lStart + 2] = jeShiftBytes[1];
//             bin_out[lStart + 3] = jeShiftBytes[0];

//             DUMP_COLLECTION(jeShiftBytes);

//             DUMP_COLLECTION(bin_out);
//             continue;
//         }
//         //Skip every unrecognized token
//     }

//     //add return opcodes
//     //mov eax. 1 B801000000
//     pushBackArray(bin_out, {'\xB8','\x01','\x00','\x00','\x00'} );
//     //mov ebx. 0 BB5D000000
//     pushBackArray(bin_out, {'\xBB','\x5D','\x00','\x00','\x00'} );
//     //int 0x80
//     pushBackArray(bin_out, {'\xCD', '\x80'} );

//     //calculate adress that starts immediately after our .tex segment
//     //this is an andress of .bss segment

//     unsigned bssAdr = LOAD_ADDRESS + sizeof(Binary) + bin_out.size();
//     DUMP_VAR(bssAdr);

//     //replace undifined .bss segment adress with calculated.
//     //follow little endiad rules

//     auto bssAdrBytes = splitToBytes(bssAdr);
//     bin_out[1] = bssAdrBytes[3];
//     bin_out[2] = bssAdrBytes[2];
//     bin_out[3] = bssAdrBytes[1];
//     bin_out[4] = bssAdrBytes[0];

//     DUMP_COLLECTION(bin_out);

//     //Write output to elf
//     std::stringstream out;

//     Binary bin = createBinary( bin_out.size() );

//     out.write((char*)&bin, sizeof(bin));
//     out.write((char*)&bin_out[0], bin_out.size()* sizeof(char));

//     return out.str();
// }

ASMTemplates createAsmTemplates()
{
    std::stringstream buffer;
    ASMTemplates asm_tpls;

    std::ifstream templateFile("template_windows.asm");
    buffer << templateFile.rdbuf();
    templateFile.close();
    asm_tpls.src = buffer.str();
    buffer.str(""); //clear buffer;

    std::ifstream cmdFile("command_windows.asm");
    buffer << cmdFile.rdbuf();
    templateFile.close();

    asm_tpls.cmds = split(buffer.str(), '/');
    return asm_tpls;
}

std::string assembly(const std::string& source)
{
    ASMTemplates asm_tpls = createAsmTemplates();

    std::string cmds;

    unsigned lastID = 0;
    std::stack<unsigned> stack;

    for(auto token : source) {
        if(token == '>') {
            cmds.append( asm_tpls.cmds[Operations::INC_PTR] );
            continue;
        }
        if(token == '<') {
            cmds.append( asm_tpls.cmds[Operations::DEC_PTR] );
            continue;
        }
        if (token == '+') {
            cmds.append( asm_tpls.cmds[Operations::INC_VALUE] );
            continue;
        }
        if (token == '-') {
            cmds.append( asm_tpls.cmds[Operations::DEC_VALUE] );
            continue;
        }
        if (token == '.') {
            cmds.append( asm_tpls.cmds[Operations::PUT_CHAR] );
            continue;
        }
        if (token == ',') {
            cmds.append( asm_tpls.cmds[Operations::GET_CHAR] );
            continue;
        }
        if (token == '[') {
            auto templ = asm_tpls.cmds[Operations::START_LOOP];

            stack.push(lastID);
            replaceAll( templ, "NUM", std::to_string(lastID) );
            lastID++;

            cmds.append( templ );
            continue;
        }
        if (token == ']') {
            auto templ = asm_tpls.cmds[Operations::END_LOOP];

            auto id = stack.top();
            stack.pop();
            replaceAll( templ, "NUM", std::to_string(id) );
            lastID++;

            cmds.append( templ );
            continue;
        }
        //Skip every unrecognized token
    }


    if (stack.size() != 0) {
        printf(" Number of '[' does not equal to number of ']'");
        std::exit(255);
    }

    replaceAll(asm_tpls.src, "{SOURCE}", cmds);
    asm_tpls.src.append("\n");

    return asm_tpls.src;
}

}// namespace Compiller
