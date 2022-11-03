cmake_minimum_required (VERSION 3.10)

project (cpplisp)

SET(CMAKE_DEFAULT_STARTUP_PROJECT ALL_BUILD)

SET( CMAKE_OBJ_PATH obj )

IF( CMAKE_COMPILER_IS_GNUCC )
  SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17 -ftemplate-depth=2000 -Wall -O0 -g -ggdb" )
  SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -ftemplate-depth=2000 -O3" )
  SET( CMAKE_CXX_FLAGS_PROFILE "-std=c++17 -ftemplate-depth=2000 -O0 -pg" )
ENDIF()

aux_source_directory(. DIR_SRCS)

add_executable(cpplisp ${DIR_SRCS})