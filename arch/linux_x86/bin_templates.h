#ifndef BIN_TEMPLATES_H
#define BIN_TEMPLATES_H

#include <elf.h>
#include <cstdlib>
#include <stddef.h>
#include <unordered_map>
#include <functional>

#include "utils.h"

const unsigned LOAD_ADDRESS = 0x8048000;
const unsigned BSS_MEM_SIZE = 0x1D4C0; /*30'000 * 4*/

struct BinaryHeader {
  Elf32_Ehdr e_hdr;
  Elf32_Phdr text_hdr;
  Elf32_Phdr bss_hdr;
  char code[];
};

BinaryHeader createBinaryHeader(unsigned code_size)
{
    return BinaryHeader {
        /* ELF HEADER */
        /*.ehdr = */
        {
          /* general */

          /*.e_ident   = */ {
                ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,
                ELFCLASS32,
                ELFDATA2LSB,
                EV_CURRENT,
                ELFOSABI_LINUX,
              },
          /*.e_type    =*/  ET_EXEC,
          /*.e_machine = */ EM_386,
          /*.e_version = */ EV_CURRENT,
          /*.e_entry   = */ LOAD_ADDRESS + ( offsetof (struct BinaryHeader, code) ),
          /*.e_phoff   = */ offsetof (struct BinaryHeader, text_hdr),
          /*.e_shoff   = */ 0,
          /*.e_flags   = */ 0,
          /*.e_ehsize  = */ sizeof (Elf32_Ehdr),

          /* program header */
          /*.e_phentsize = */sizeof (Elf32_Phdr),
          /*.e_phnum     = */2,

          /* section header */
          /*.e_shentsize = */sizeof (Elf32_Shdr),
          /*.e_shnum     = */0,
          /*.e_shstrndx  = */0
        },

        /* PROGRAM HEADER .text */
        /*.phdr =*/ {
          /*.p_type   =*/ PT_LOAD,
          /*.p_offset =*/ 0,
          /*.p_vaddr = */ LOAD_ADDRESS,
          /*.p_paddr = */ LOAD_ADDRESS,
          /*.p_filesz = */cast_to_U32( sizeof (struct BinaryHeader) + code_size ),
          /*.p_memsz = */ cast_to_U32(sizeof (struct BinaryHeader) + code_size ),
          /*.p_flags = */ PF_R | PF_X,
          /*.p_align = */ 0x1000
        },

        /* PROGRAM HEADER .bss*/
        /*.phdr =*/ {
          /*.p_type   =*/ PT_LOAD,
          /*.p_offset =*/ cast_to_U32( (offsetof (struct BinaryHeader, code) ) + code_size ),
          /*.p_vaddr = */ cast_to_U32( LOAD_ADDRESS + (offsetof (struct BinaryHeader, code) ) + code_size ),
          /*.p_paddr = */ cast_to_U32( LOAD_ADDRESS + (offsetof (struct BinaryHeader, code) ) + code_size ),
          /*.p_filesz = */0,
          /*.p_memsz = */ BSS_MEM_SIZE,
          /*.p_flags = */ PF_R | PF_W,
          /*.p_align = */ 0x1000
        },

        /* CODE */

      };
}

using transform = std::function<std::string(unsigned)>;

static std::unordered_map<char, transform> bin_templates = {
    {
        '>',
        [](auto arg) -> std::string {
            auto b = splitToBytes(arg * 4);
            return std::string {
                '\x81','\xC6', b[3], b[2], b[1], b[0]
            };
        }
    },
    {
        '<',
        [](auto arg) -> std::string {
            auto b = splitToBytes(arg * 4);
            return std::string{
                '\x81','\xEE', b[3], b[2], b[1], b[0]
            };
        }
    },
    {
        '+',
        [](auto arg) -> std::string {
            auto b = splitToBytes(arg);
            return std::string {
                '\x81','\x06', b[3], b[2], b[1], b[0]
            };
        }
    },
    {
        '-',
        [](auto arg) -> std::string {
            auto b = splitToBytes(arg);
            return std::string {
                '\x81','\x2E', b[3], b[2], b[1], b[0]
            };
        }
    },
    {
        '.',
        [](auto arg) -> std::string {
            return std::string {
                '\xB8','\x04','\x00','\x00','\x00',
                '\xBB','\x01','\x00','\x00','\x00',
                '\x89','\xF1',
                '\xBA','\x01','\x00','\x00','\x00',
                '\xCD','\x80'
            };
        },
    }, {
        ',',
        [](auto arg) -> std::string {
            return std::string {
                '\xB8','\x03','\x00','\x00','\x00',
                '\xBB','\x01','\x00','\x00','\x00',
                '\x89','\xF1',
                '\xBA','\x01','\x00','\x00','\x00',
                '\xCD','\x80'
            };
        }
    },
    {
        '[',
        [](auto arg) -> std::string {
            auto b = splitToBytes(arg);
            return std::string {
                '\x83','\x3E','\x00',
                '\x0F','\x84', b[3], b[2], b[1], b[0]
            };
        }
    },
    {
        ']',
        [](auto arg) -> std::string {
            auto b = splitToBytes(arg);
            return std::string {
                '\xE9',b[3], b[2], b[1], b[0]
            };
        },
    }
};

std::string bin_end_template = {
    //mov eax 1 B801000000
    '\xB8','\x01','\x00','\x00','\x00',
    //mov ebx. 0 BB5D000000
    '\xBB','\x5D','\x00','\x00','\x00',
    //int 0x80
    '\xCD', '\x80',
};

std::string bin_start_template(unsigned file_size) {
    auto bss_address_bytes = splitToBytes(file_size + 5); // 5 - opcode size
    //mov esi, DATAPTR
    return std::string{ '\xBE', bss_address_bytes[3], bss_address_bytes[2], bss_address_bytes[1], bss_address_bytes[0]};
}

#endif //BIN_TEMPLATES_H
