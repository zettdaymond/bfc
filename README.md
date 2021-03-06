# Description #
**BFC** - is a dead simple **Brainfuck Compiler** written in C++14. It has been written with learning purposes especially to dive into **ELF** and **PE** binary file formats and to give some understanding of compilation process.

BFC reads target file, then passes its content to lexical analysis to generate Brainfuck tokens. If tokens successfully verified by syntactic and semantic analysis, then BFC generates optimized byte code as internal representation and, finally, translates internal representation into assembly language or binary machine code on target platform(creates executable file).

Note, that BFC creates executable file for target platform instead of object file, that normally creates any other compilers. Thus, BFC does not requires linker. This behavior was selected because of my goal - to explore PE and ELF file formats, as I already said before. 

# Brainfuck #
You may read about Brainfuck on corresponding wiki page : https://en.wikipedia.org/wiki/Brainfuck. 

Differences between this and discribed in wiki implementation:

* Each cell size is **u32** (double word) instead of **u8** (byte).
* Program may contain one-line comments starting with **#**.
* All other symbols, exept Brainfuck tokens and comments are be treated as invalid.

# Supported Platforms #
BFC can generate NASM assembly and binnary machine code for 

* **Linux x86** (ELF file format) 
* **Windows x86** (PE file format).

Maybe in future I will add support for x86_64 architecture.

# Build #
There is no any external dependencies, but you need compiler with **C++14** support, for example:

* g++ 5.1
* clang 3.4

and **cmake >= 3.0** buld system.

## Linux ##
Clone the repo to your machine:
```
$ git clone https://bitbucket.org/zettdaymond/bfc
$ cd ./bfc
```
Then to compile just type:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

You can also specify debug build. For this, instead of
```
$ cmake ..
```
Write
```
$ cmake -DCMAKE_BUILD_TYPE=debug ..
```
## Windows ##
I build BFC on windows using [MSYS2](https://msys2.github.io/) and MinGW compiler.

Clone the repo to your machine:
```
$ git clone https://bitbucket.org/zettdaymond/bfc
$ cd ./bfc
```
Create build folder
```
$ mkdir build
$ cd build
```
Then, if you in MSYS2 environment, type:
```
cmake -G "MSYS Makefiles" ..
make
```
I've tested that BFC may be build using Visual Studio 2019. To build bfc using Visual Studio 2019 instead of
```
cmake -G "MSYS Makefiles" ..
```
in previous instruction, write, for example,
```
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build ./
```
See full list of all available generators by typing
```
cmake --help
```
