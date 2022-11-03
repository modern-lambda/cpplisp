set_project("cpplisp")
set_languages("c++17")
set_warnings("allextra")
--set_warnings("all", "error")

-- set_config("cxxflags", "-Wno-attributes")
set_config("cc", "clang")
set_config("cxx", "clang++")
set_config("ld", "clang++")

add_rules("mode.debug", "mode.release")

add_requires("gtest")
add_packages("gtest")

target("cpplisp")
    set_kind("binary")
    add_files("./test.cpp")

target("cpplisp_unittest")
    add_files("cpplisp_unittest.cc")