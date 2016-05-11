#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "compiller.h"
#include "utils.h"

using namespace bfc;

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

    auto tokens = lexAnalysis(src);
    if (tokens.empty()) {
        DEBUG_PRINT("Lex analyse failed");
        return 0;
    }
    DEBUG_LAMBDA_TRAVERSE(tokens, [](auto& t){std::cout << t;});

    auto analyse_secusess = syntaxAnalysis(tokens);
    if (not analyse_secusess) {
        DEBUG_PRINT( "Syntax analyse failed");
        return 0;
    }

    analyse_secusess = semanticAnalysis(tokens);
    if (not analyse_secusess) {
        DEBUG_PRINT( "Semantic analyse failed");
        return 0;
    }

    auto byte_code = optimize(tokens);

    DEBUG_LAMBDA_TRAVERSE(byte_code, [](auto& t){std::cout << t.op << " : " << t.arg << std::endl;});

    if (compillerState.assembly == true) {
        std::cout << "Translate to NASM..." << std::endl;
        out = toAssembly(byte_code);
    } else {
        std::cout << "Translate to Binary..." << std::endl;
        out = toBinary(byte_code);
    }

    std::ofstream outFile( compillerState.outFile );
    outFile << out;
    outFile.flush();

    std::cout << "Done." << std::endl;

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
    std::cout   << "Usage: bfc [OPTIONS] [FILE]\n"
                << "Options: \n"
                << "\t-h, --help       Display this message and exit\n"
                << "\t-a, --assembly   Generate NASM assembly listing instead of binary data\n"
                << "\t-o, --output     Specify output file\n";
}
