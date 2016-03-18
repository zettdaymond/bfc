#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <elf.h>
#include <iostream>
#include <sstream>

#include <fstream>

#include "compiller.h"

using namespace Compiller;

void usage();
CompillerState parseComandLineArgs(int argc, char const *argv[]);


int main(int argc, char const *argv[])
{
    auto compillerState = parseComandLineArgs(argc, argv);

    std::ifstream sourceFile(compillerState.src);

    if (!sourceFile.is_open()) {
        std::cout << "Could not open file '"
                  << compillerState.src << "'." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::stringstream buffer;
    buffer << sourceFile.rdbuf();
    sourceFile.close();

    std::string out;

    auto src = buffer.str();
    if (compillerState.assembly == true) {
        out = assembly(src);
    } else {
        out = compile(src);
    }
    std::ofstream outFile( compillerState.outFile );
    outFile << out;
    outFile.flush();

    return 0;
}


CompillerState parseComandLineArgs(int argc, char const *argv[])
{
    CompillerState state;
    if( argc <= 1 ) {
        printf("Error: No file specified (use -h for help)\n");
        std::exit(EXIT_FAILURE);
    }

    for(auto i = 1; i < argc; i++) {
        if ( not strcmp(argv[i], "-h") || not strcmp(argv[i], "--help") ) {
            usage();
            std::exit(0);
        }

        if ( not strcmp(argv[i], "-a") || not strcmp(argv[i], "--assembly") ) {
            state.assembly = true;
            continue;
        }

        if ( not strcmp(argv[i], "-o") || not strcmp(argv[i], "--output") ) {
            if (i + 1 != argc){
                state.outFile = argv[i+1];
                i++;
                continue;
            }
        }

        state.src = argv[i];

    }

    return state;
}


void usage()
{
    printf("Usage: bfc [OPTIONS] file...\n"
           "Options: \n");
    printf("\t-h, --help\t"
           "Display this message and exit\n");
    printf("\t-a, --assembly\t"
           "Generate NASM assembly listing\n");
    printf("\t-o, --output\t"
           "Generate NASM assembly listing\n");
}
