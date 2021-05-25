cmake_minimum_required(VERSION 3.13)
project(hw3)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} "-Wall -pedantic"" )

add_executable(hw3_output hw3_output.cpp semantics.cpp lex.yy.cpp parser.tab.cpp SymbolTable.cpp SymbolTable.h)