#include <iostream>
#include <typeinfo>
#include <string>
#include <gtest/gtest.h>

#include "cpplisp.hpp"

TEST(Cpplisp, cons) {
  using namespace cpplisp::runtime;
  using namespace std;

  var cons_1 = cons(1, 2);
  var cons_2 = cons("foo", "bar");
  var cons_3 = cons(1, "foo");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}