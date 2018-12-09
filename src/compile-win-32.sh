#!/bin/bash

parser=Core/QueryParser

flex -o $parser/Generated/lexer.cpp $parser/flex.ll
bison -o $parser/Generated/parser.cpp $parser/grammar.yy

incw32="-I$HOME/dev/iaw32/include"

src=`find . -name "*.cpp"`

i586-mingw32msvc-g++ -DBOOST_DISABLE_THREADS -DNDEBUG -DDISABLE_ASSERTX -static -O3 -Wall \
    $src $incw32 -o verifydtapn32.exe && \
i586-mingw32msvc-strip verifydtapn32.exe && \
echo "32-bit win OK" 

