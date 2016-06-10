#ifndef EXE_BUILDER_H
#define EXE_BUILDER_H

#include <windows.h>
#include <cstdlib>
#include <stddef.h>
#include <time.h>
#include <fstream>
#include <vector>

const unsigned NUMBER_OF_SECTIONS = 3;
const unsigned IMAGEBASE = 0x400000;
const unsigned SECTION_ALIGN = 0x1000;
const unsigned FILE_ALIGN = 0x200;

struct BinaryHeader {
    IMAGE_DOS_HEADER dos_Header; //DOS Header
    DWORD pe_Header_Signature;
    IMAGE_FILE_HEADER pe_File_Header;
    IMAGE_OPTIONAL_HEADER32 opt_Header;
    IMAGE_SECTION_HEADER scn_Headers[3]; //.text, .rdata, .bss section headers;
    char code[];
};

template<class T, class U>
T roundToAlign( T v, U align) {
    return v + (align - (v % align));
}

const unsigned ENTRY_POINT = roundToAlign(offsetof(BinaryHeader, code), SECTION_ALIGN);
const unsigned HEADER_TO_SECTION_ROUNDED_SIZE = ENTRY_POINT; //All between file start and EntryPoint is header.
const unsigned HEADER_TO_FILE_ROUNDED_SIZE = roundToAlign(offsetof(BinaryHeader, code), FILE_ALIGN);
const unsigned BSS_MEM_SIZE = 0x1D4C0 + 4; /* 30'000 cells + dummy */
const unsigned BSS_SECTION_SIZE = roundToAlign(BSS_MEM_SIZE ,SECTION_ALIGN);

BinaryHeader createBinaryHeader(unsigned code_size)
{
    const unsigned CODE_SECTION_SIZE = roundToAlign(code_size, SECTION_ALIGN);
    const unsigned CodeToFileRoundedSize = roundToAlign(code_size, FILE_ALIGN);
    const unsigned Import_Descriptor = HEADER_TO_SECTION_ROUNDED_SIZE + CODE_SECTION_SIZE + BSS_SECTION_SIZE; //Starts immidiatly after bss section

    //Less understandable, but much more verbose style.
    //+
    IMAGE_DOS_HEADER DOS_HEADER{};
    DOS_HEADER.e_magic = IMAGE_DOS_SIGNATURE;
    DOS_HEADER.e_lfanew = offsetof( BinaryHeader, pe_Header_Signature );

    //+
    IMAGE_FILE_HEADER FILE_HEADER{};
    FILE_HEADER.Machine = IMAGE_FILE_MACHINE_I386;
    FILE_HEADER.NumberOfSections = NUMBER_OF_SECTIONS;
    FILE_HEADER.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER32);
    FILE_HEADER.Characteristics = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE;

    //+
    IMAGE_OPTIONAL_HEADER32 OPTIONAL_HEADER{};
    OPTIONAL_HEADER.Magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
    OPTIONAL_HEADER.AddressOfEntryPoint =  ENTRY_POINT;
    OPTIONAL_HEADER.ImageBase =  IMAGEBASE;
    OPTIONAL_HEADER.SectionAlignment = SECTION_ALIGN;
    OPTIONAL_HEADER.FileAlignment = FILE_ALIGN;
    OPTIONAL_HEADER.MajorSubsystemVersion = 4;
    OPTIONAL_HEADER.SizeOfImage = HEADER_TO_SECTION_ROUNDED_SIZE + CODE_SECTION_SIZE + BSS_SECTION_SIZE + SECTION_ALIGN;
    OPTIONAL_HEADER.SizeOfHeaders = HEADER_TO_FILE_ROUNDED_SIZE;
    OPTIONAL_HEADER.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
    OPTIONAL_HEADER.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

    //
    IMAGE_DATA_DIRECTORY IMPORT_DATA_DIRECTORY{};
    IMPORT_DATA_DIRECTORY.VirtualAddress = Import_Descriptor;

    //export place in 0, import place in 1.
    OPTIONAL_HEADER.DataDirectory[1] = IMPORT_DATA_DIRECTORY;

    //code section
    IMAGE_SECTION_HEADER SECTION_HEADER_TEXT{};
    memcpy(SECTION_HEADER_TEXT.Name, ".text", 5);
    SECTION_HEADER_TEXT.Misc.VirtualSize = CODE_SECTION_SIZE;
    SECTION_HEADER_TEXT.VirtualAddress = ENTRY_POINT;
    SECTION_HEADER_TEXT.SizeOfRawData = CodeToFileRoundedSize;
    SECTION_HEADER_TEXT.PointerToRawData = HEADER_TO_FILE_ROUNDED_SIZE;
    SECTION_HEADER_TEXT.Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;

    IMAGE_SECTION_HEADER SECTION_HEADER_BSS{};
    memcpy(SECTION_HEADER_BSS.Name, ".bss", 4);
    SECTION_HEADER_BSS.Misc.VirtualSize =  BSS_SECTION_SIZE;
    SECTION_HEADER_BSS.VirtualAddress = HEADER_TO_SECTION_ROUNDED_SIZE + CODE_SECTION_SIZE;
//    SECTION_HEADER_BSS.SizeOfRawData = BSS_MEM_SIZE;
//    SECTION_HEADER_BSS.PointerToRawData = HeaderToFileRoundedSize +  CodeToFileRoundedSize + FILE_ALIGN;
    SECTION_HEADER_BSS.Characteristics = IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;


    IMAGE_SECTION_HEADER SECTION_HEADER_IMPORTS{};
    memcpy(SECTION_HEADER_IMPORTS.Name, ".rdata", 6);
    SECTION_HEADER_IMPORTS.Misc.VirtualSize = 1 * SECTION_ALIGN;
    SECTION_HEADER_IMPORTS.VirtualAddress = HEADER_TO_SECTION_ROUNDED_SIZE + CODE_SECTION_SIZE + BSS_SECTION_SIZE;
    SECTION_HEADER_IMPORTS.SizeOfRawData = 1 * FILE_ALIGN;
    SECTION_HEADER_IMPORTS.PointerToRawData = HEADER_TO_FILE_ROUNDED_SIZE +  CodeToFileRoundedSize;
    SECTION_HEADER_IMPORTS.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;

    return BinaryHeader {
        DOS_HEADER,
        IMAGE_NT_SIGNATURE,
        FILE_HEADER,
        OPTIONAL_HEADER,
        {
            SECTION_HEADER_TEXT,
            SECTION_HEADER_BSS,
            SECTION_HEADER_IMPORTS
        }

    };
}

struct ImportSection {
    IMAGE_IMPORT_DESCRIPTOR is_kernel32;
    IMAGE_IMPORT_DESCRIPTOR is_term;
    DWORD is_hintnames[4 + 1];

    //IMPORT_BY_NAME:
    WORD ibn_ep_hint = 0;
    char ibn_ep_name[12] = "ExitProcess";

    WORD ibn_gdh_hint = 0;
    char ibn_gdh_name[13] = "GetStdHandle";

    WORD ibn_rf_hint = 0;
    char ibn_rf_name[9] = "ReadFile";

    WORD ibn_wf_hint = 0;
    char ibn_wf_name[10] = "WriteFile";

    DWORD is_loadpoints[4 + 1];
    char is_k32[13] = "kernel32.dll";//{'k','e','r','n','e','l','3','2','.','d','l','l','\0'};
};

ImportSection createImportsSection(unsigned raw_code_size)
{
    const unsigned CODE_SECTION_SIZE = roundToAlign(raw_code_size, SECTION_ALIGN);
    const unsigned IMPORT_SECTION_SHIFT = HEADER_TO_SECTION_ROUNDED_SIZE +
                                          CODE_SECTION_SIZE +
                                          BSS_SECTION_SIZE;
    const unsigned HintNamesEntry = IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_hintnames);
    const unsigned Kernel32NameEntry = IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_k32);
    const unsigned LoadPointsEntry = IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints);

    IMAGE_IMPORT_DESCRIPTOR IMPORT_DESCRIPTOR_KERNEL32{};
    IMPORT_DESCRIPTOR_KERNEL32.OriginalFirstThunk = HintNamesEntry;
    IMPORT_DESCRIPTOR_KERNEL32.Name = Kernel32NameEntry;
    IMPORT_DESCRIPTOR_KERNEL32.FirstThunk = LoadPointsEntry;

    DWORD hintnames[4 + 1] = {
        IMPORT_SECTION_SHIFT + offsetof(ImportSection, ibn_ep_hint),
        IMPORT_SECTION_SHIFT + offsetof(ImportSection, ibn_gdh_hint),
        IMPORT_SECTION_SHIFT + offsetof(ImportSection, ibn_rf_hint),
        IMPORT_SECTION_SHIFT + offsetof(ImportSection, ibn_wf_hint),
        0
    };

    ImportSection is{};

    is.is_kernel32 = IMPORT_DESCRIPTOR_KERNEL32;
    std::copy(std::begin(hintnames), std::end(hintnames), std::begin(is.is_hintnames));
    std::copy(std::begin(hintnames), std::end(hintnames), std::begin(is.is_loadpoints));

    return is;
}

struct KernelFunctions {
    unsigned exit_process;
    unsigned get_std_handle;
    unsigned read_file;
    unsigned write_file;
};

KernelFunctions getKernelFunctionAddresses(unsigned raw_code_size) {
    const unsigned CODE_SECTION_SIZE = roundToAlign(raw_code_size, SECTION_ALIGN);
    const unsigned IMPORT_SECTION_SHIFT = HEADER_TO_SECTION_ROUNDED_SIZE +
                                          CODE_SECTION_SIZE +
                                          BSS_SECTION_SIZE;
    return KernelFunctions {
        IMAGEBASE + IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints[0]),
        IMAGEBASE + IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints[1]),
        IMAGEBASE + IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints[2]),
        IMAGEBASE + IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints[3]),
    };
}

void writeWithAlign(std::ostream& out, char* data, unsigned size, unsigned align)
{
    auto bytes_to_align = (align - (size % align));
    std::vector<char> padd;
    padd.resize(bytes_to_align);
    for (auto& c : padd) {
        c = 0;
    }

    out.write(data, size);
    out.write(&padd[0], padd.size() * sizeof(char));
}

#endif //EXE_BUILDER_H
