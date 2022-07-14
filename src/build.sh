#!/bin/bash
LLVM_BIN=/usr/local/opt/llvm/bin
clang++ `${LLVM_BIN}/llvm-config --cxxflags --ldflags --libs` -lclang-cpp DumpFileAST.cpp -o dump-file-ast
