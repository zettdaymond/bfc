#include "exe_builder.h"

BinaryHeader createBinaryHeader(unsigned raw_code_size)
{
    const unsigned CODE_SECTION_SIZE = roundToAlign(raw_code_size, SECTION_ALIGN);
    const unsigned CODE_TO_FILE_ROUNDED_SIZE = roundToAlign(raw_code_size, FILE_ALIGN);
    const unsigned IMPORT_DESCRIPTOR = HEADER_SECTION_SIZE + CODE_SECTION_SIZE + BSS_SECTION_SIZE; //Starts immidiatly after bss section

    //Less understandable, but much less verbose style.

    IMAGE_DOS_HEADER DOS_HEADER{};
    DOS_HEADER.e_magic = IMAGE_DOS_SIGNATURE;
    DOS_HEADER.e_lfanew = offsetof( BinaryHeader, pe_Header_Signature );

    IMAGE_FILE_HEADER FILE_HEADER{};
    FILE_HEADER.Machine = IMAGE_FILE_MACHINE_I386;
    FILE_HEADER.NumberOfSections = NUMBER_OF_SECTIONS;
    FILE_HEADER.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER32);
    FILE_HEADER.Characteristics = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE;

    IMAGE_OPTIONAL_HEADER32 OPTIONAL_HEADER{};
    OPTIONAL_HEADER.Magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
    OPTIONAL_HEADER.AddressOfEntryPoint =  ENTRY_POINT;
    OPTIONAL_HEADER.ImageBase =  IMAGEBASE;
    OPTIONAL_HEADER.SectionAlignment = SECTION_ALIGN;
    OPTIONAL_HEADER.FileAlignment = FILE_ALIGN;
    OPTIONAL_HEADER.MajorSubsystemVersion = 4;
    OPTIONAL_HEADER.SizeOfImage = HEADER_SECTION_SIZE + CODE_SECTION_SIZE + BSS_SECTION_SIZE + IMPORT_SECTION_SIZE;
    OPTIONAL_HEADER.SizeOfHeaders = HEADER_IN_FILE_SIZE;
    OPTIONAL_HEADER.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
    OPTIONAL_HEADER.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

    IMAGE_DATA_DIRECTORY IMPORT_DATA_DIRECTORY{};
    IMPORT_DATA_DIRECTORY.VirtualAddress = IMPORT_DESCRIPTOR;

    //export place in 0, import place in 1.
    OPTIONAL_HEADER.DataDirectory[1] = IMPORT_DATA_DIRECTORY;

    //code section
    IMAGE_SECTION_HEADER SECTION_HEADER_TEXT{};
    memcpy(SECTION_HEADER_TEXT.Name, ".text", 5);
    SECTION_HEADER_TEXT.Misc.VirtualSize = CODE_SECTION_SIZE;
    SECTION_HEADER_TEXT.VirtualAddress = ENTRY_POINT;
    SECTION_HEADER_TEXT.SizeOfRawData = CODE_TO_FILE_ROUNDED_SIZE;
    SECTION_HEADER_TEXT.PointerToRawData = HEADER_IN_FILE_SIZE;
    SECTION_HEADER_TEXT.Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;

    //uninitialized data .bss section
    IMAGE_SECTION_HEADER SECTION_HEADER_BSS{};
    memcpy(SECTION_HEADER_BSS.Name, ".bss", 4);
    SECTION_HEADER_BSS.Misc.VirtualSize =  BSS_SECTION_SIZE;
    SECTION_HEADER_BSS.VirtualAddress = HEADER_SECTION_SIZE + CODE_SECTION_SIZE;
    //    SECTION_HEADER_BSS.SizeOfRawData = BSS_MEM_SIZE;
    //    SECTION_HEADER_BSS.PointerToRawData = HeaderToFileRoundedSize +  CodeToFileRoundedSize + FILE_ALIGN;
    SECTION_HEADER_BSS.Characteristics = IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;

    //import section
    IMAGE_SECTION_HEADER SECTION_HEADER_IMPORTS{};
    memcpy(SECTION_HEADER_IMPORTS.Name, ".rdata", 6);
    SECTION_HEADER_IMPORTS.Misc.VirtualSize = IMPORT_SECTION_SIZE;
    SECTION_HEADER_IMPORTS.VirtualAddress = HEADER_SECTION_SIZE + CODE_SECTION_SIZE + BSS_SECTION_SIZE;
    SECTION_HEADER_IMPORTS.SizeOfRawData = IMPORT_IN_FILE_SIZE;
    SECTION_HEADER_IMPORTS.PointerToRawData = HEADER_IN_FILE_SIZE +  CODE_TO_FILE_ROUNDED_SIZE;
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

KernelFunctionsAddresses getKernelFunctionAddresses(unsigned raw_code_size) {
    const unsigned CODE_SECTION_SIZE = roundToAlign(raw_code_size, SECTION_ALIGN);
    const unsigned IMPORT_SECTION_SHIFT = HEADER_SECTION_SIZE +
                                          CODE_SECTION_SIZE +
                                          BSS_SECTION_SIZE;
    return KernelFunctionsAddresses {
        IMAGEBASE + IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints[0]),
        IMAGEBASE + IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints[1]),
        IMAGEBASE + IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints[2]),
        IMAGEBASE + IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints[3]),
    };
}

ImportSection createImportSection(unsigned raw_code_size)
{
    const unsigned CODE_SECTION_SIZE = roundToAlign(raw_code_size, SECTION_ALIGN);
    const unsigned IMPORT_SECTION_SHIFT = HEADER_SECTION_SIZE +
                                          CODE_SECTION_SIZE +
                                          BSS_SECTION_SIZE;

    const unsigned HINT_NAMES_ENTRY = IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_hintnames);
    const unsigned KERNEL32_NAME_ENTRY = IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_k32);
    const unsigned LOAD_POINTS_ENTRY = IMPORT_SECTION_SHIFT + offsetof(ImportSection, is_loadpoints);

    IMAGE_IMPORT_DESCRIPTOR IMPORT_DESCRIPTOR_KERNEL32{};
    IMPORT_DESCRIPTOR_KERNEL32.OriginalFirstThunk = HINT_NAMES_ENTRY;
    IMPORT_DESCRIPTOR_KERNEL32.Name = KERNEL32_NAME_ENTRY;
    IMPORT_DESCRIPTOR_KERNEL32.FirstThunk = LOAD_POINTS_ENTRY;

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
