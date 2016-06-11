#ifndef EXE_BUILDER_H
#define EXE_BUILDER_H

#include <windows.h>
#include <cstdlib>
#include <stddef.h>
#include <time.h>
#include <fstream>
#include <vector>

#include "utils.h"

const unsigned NUMBER_OF_SECTIONS = 3;
const unsigned IMAGEBASE = 0x400000;
const unsigned SECTION_ALIGN = 0x1000;
const unsigned FILE_ALIGN = 0x200;

struct BinaryHeader {
    IMAGE_DOS_HEADER dos_Header;
    DWORD pe_Header_Signature;
    IMAGE_FILE_HEADER pe_File_Header;
    IMAGE_OPTIONAL_HEADER32 opt_Header;
    IMAGE_SECTION_HEADER scn_Headers[3]; //.text, .rdata, .bss section headers;
    char code[];
};

struct ImportSection {
    IMAGE_IMPORT_DESCRIPTOR is_kernel32;
    IMAGE_IMPORT_DESCRIPTOR is_term;
    DWORD is_hintnames[4 + 1];

    //series of IMPORT_BY_NAME structs:
    WORD ibn_ep_hint = 0;
    char ibn_ep_name[12] = "ExitProcess";

    WORD ibn_gdh_hint = 0;
    char ibn_gdh_name[13] = "GetStdHandle";

    WORD ibn_rf_hint = 0;
    char ibn_rf_name[9] = "ReadFile";

    WORD ibn_wf_hint = 0;
    char ibn_wf_name[10] = "WriteFile";

    DWORD is_loadpoints[4 + 1];
    char is_k32[13] = "kernel32.dll";
};

const unsigned HEADER_SECTION_SIZE = roundToAlign(sizeof(BinaryHeader), SECTION_ALIGN);
const unsigned ENTRY_POINT = HEADER_SECTION_SIZE; //code starts immidiatly after header.
const unsigned HEADER_IN_FILE_SIZE = roundToAlign(offsetof(BinaryHeader, code), FILE_ALIGN);
const unsigned BSS_MEM_SIZE = 0x1D4C0 + sizeof(DWORD); /* 30'000 cells + dummy */
const unsigned BSS_SECTION_SIZE = roundToAlign(BSS_MEM_SIZE ,SECTION_ALIGN);
const unsigned IMPORT_SECTION_SIZE = roundToAlign(sizeof(ImportSection), SECTION_ALIGN);
const unsigned IMPORT_IN_FILE_SIZE = roundToAlign(sizeof(ImportSection), FILE_ALIGN);


struct KernelFunctionsAddresses {
    unsigned exit_process;
    unsigned get_std_handle;
    unsigned read_file;
    unsigned write_file;
};

/**
 * @brief Creates PE file header.
 * @param raw_code_size - count of bytes of all generated opcodes.
 * @return PE file header.
 */
BinaryHeader createBinaryHeader(unsigned raw_code_size);

/**
 * @brief Creates PE file Import Section, that describes imported functions from 'linked' kernel32 library.
 * @param raw_code_size - count of bytes of all generated opcodes.
 * @return PE file import section.
 */
ImportSection createImportSection(unsigned raw_code_size);

/**
 * @brief Determines virtual addresses of imported functions from kernel32 library.
 * @param raw_code_size - count of bytes of all generated opcodes.
 * @return struct, that contains virtual addresses of functions.
 */
KernelFunctionsAddresses getKernelFunctionAddresses(unsigned raw_code_size);

#endif //EXE_BUILDER_H
