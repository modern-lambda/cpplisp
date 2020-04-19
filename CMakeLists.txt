cmake_minimum_required (VERSION 3.0)

project (cpplisp)

aux_source_directory(. DIR_SRCS)

add_executable(cpplisp ${DIR_SRCS})