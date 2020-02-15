#include "compiller.h"

#include <iostream>
#include <stack>
#include "utils.h"

using namespace bfc;

std::vector<char> bfc::lexAnalysis(const std::string& source) {
    //Grammar
    //S -> <S|>S|+S|-S|[S|]S|.S|,S|<|>|+|-|[|]|.|,
    //Comments
    //# .. \n
    std::vector<char> out;
    bool error_occur = false;

    for ( auto i{0u}; i < source.length(); i++) {
        const char c = source[i];
        if (c == '#') {
            while (source[i] != '\n' || i == source.length() - 1) {
                i++;
            }
            if( i == source.length() - 1) {
                std::cout << "[ERROR] Nothing to compile." << std::endl;
            }
            continue;
        }
        if (c == ' ' || c == '\t' || c == '\n') {
            continue;
        }
        if (c == '<' || c == '>' || c == '+' || c == '-'
            || c == '[' || c == ']' || c == '.' || c == ',')
        {
            out.push_back(c);
            continue;
        }

        //default
        error_occur = true;
        auto occ = getLineAndOccurrence(i, source);
        std::cout << "[ERROR] at Line : " << occ.first << ", Column : " << occ.second << "\n"
                  << "\tCould not recognize symbol '" << c << "'" << std::endl;
    }
    return error_occur == false ? out : std::vector<char>();
}

bool isSimpleOp(char c) {
    return c == '<' || c == '>' || c ==  '+' || c == '-' || c == '.' || c == ',';
}


bool bfc::syntaxAnalysis(const std::vector<char> &v)
{
    //TODO:
    //Grammar 1
    //S -> <S|>S|+S|-S|.S|,S|<|>|+|-|.|,| [S]| [S]S | []
    //LR(k) Parser

    //Grammar 2
    //S -> <S|>S|+S|-S|.S|,S|[S]S| 'empty'
    //LL(k) parser

    unsigned brackets_count = 0;
    for (auto i{0u}; i < v.size(); i++) {
        if(isSimpleOp(v[i])) {
            continue;
        }
        if (v[i] == ']') {
            if (brackets_count == 0) {
                std::cout << "[ERROR] Could not find pair for ']'."
                          << std::endl;
                return false;
            } else {
                brackets_count--;
            }
        }
        if (v[i] == '[') {
            brackets_count++;
        }
    }

    if (brackets_count != 0) {
        std::cout << "[ERROR] Could not find pair for '['"
                  << std::endl;
        return false;
    } else {
        return true;
    }

}

bool canJoin (char c1, char c2) {
    return c1 == c2 && (c1 == '>' || c1 == '<' || c1 == '+' || c1 == '-');
}


bool bfc::semanticAnalysis(const std::vector<char> &v) {
    //Step 1. Detect empty loop []
    for (auto i{0u}; i < v.size() - 1; i++) {
        if (v[i] == '[' && v[i+1] == ']') {
            std::cout << "[WARNING] Detect empty infinite loop. Your program will never ends"
                      << std::endl;
        }
    }

    //Step 2. Detect simple 'Out of range' exeption
    auto pos{0};
    for (auto i{0u}; i < v.size() - 1; i++) {
        if (v[i] == '[') {
            if (pos >= 0) {
                break;
            } else {
                return false; //error
            }
        }
        if (v[i] == '<') {
            pos--;
        }
        if (v[i] == '>') {
            pos++;
        }
    }

    return true;

}

std::vector<ByteCode> bfc::optimize(const std::vector<char> &v) {
    //Step 1: Remove unneeded operators
    std::vector<char> code;
    code.push_back(v[0]);
    for (auto i{1u}; i < v.size(); i++) {
        if (code.back() == '<') {
            if (v[i] == '>') {
                code.pop_back();
                continue;
            }
        }
        if (code.back() == '>') {
            if (v[i] == '<') {
                code.pop_back();
                continue;
            }
        }
        if (code.back() == '-') {
            if (v[i] == '+') {
                code.pop_back();
                continue;
            }
        }
        if (code.back() == '+') {
            if (v[i] == '-') {
                code.pop_back();
                continue;
            }
        }
        code.push_back(v[i]);
    }

    if (code.size() == 0) {
        std::cout << "[WARNING] Program does nothing."
                  << std::endl;
        return std::vector<ByteCode>();
    }

    //Step 2: Generate internal represention with opimization (join same operations)
    std::vector<ByteCode> out;
    std::stack<unsigned> s;
    unsigned next_loop_id = 0;

    out.push_back(ByteCode{code[0], 0});
    if (out[0].op == '+' || '-' || '>' || '<') {
        out[0].arg = 1;
    } else if ( out[0].op == '[') {
        out[0].arg = s.top();
        s.push(next_loop_id);
        next_loop_id++;
    }

    for (auto i{1u}; i < code.size(); i++) {
        if (canJoin( out.back().op , code[i])) {
            out.back().arg++;
            continue;
        }
        out.push_back(ByteCode{code[i], 0});
        if (out.back().op == '+' || out.back().op == '-' ||
            out.back().op == '>' || out.back().op == '<')
        {
            out.back().arg = 1;
        } else if (out.back().op == '[') {
            s.push(next_loop_id);
            out.back().arg = s.top();
            next_loop_id++;
        } else if (out.back().op == ']') {
            out.back().arg = s.top();
            s.pop();
        } else  {
            out.back().arg = 0;
        }
    }
    return out;
}
