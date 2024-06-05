cmake_minimum_required(VERSION 3.28)
project(sibasic)

set(CMAKE_CXX_STANDARD 17)

add_executable(sibasic main.cpp
        Token.h
        Lexer.h
        lexer.cpp
        Parser.h
        parser.cpp
        Interpreter.h
        interpreter.cpp)
