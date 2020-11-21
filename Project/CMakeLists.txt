cmake_minimum_required(VERSION 3.17)
project(Project C)

set(CMAKE_C_STANDARD 99)

add_executable(Project main.c lex_analyzer.c lex_analyzer.h lex_token.c lex_token.h error.c error.h symbol_table.c symbol_table.h syntax_analyzer.c syntax_analyzer.h syntax_tree.c syntax_tree.h)