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

  EXPECT_EQ(consp(cons_1), true);
  EXPECT_EQ(consp(cons_2), true);
  EXPECT_EQ(consp(cons_3), true);
}

TEST(Cpplisp, listp) {
  using namespace cpplisp::runtime;
  using namespace std;

  var cons_1 = cons(1, 2);
  var cons_2 = cons("foo", "bar");
  var cons_3 = cons(1, "foo");

  var null_cons = cons(nullptr, nullptr);
  var cons_4 = cons(3, nil);
  var cons_5 = cons("foo", nil);
  var cons_6 = cons(4, cons(nullptr, nullptr));
  var cons_7 = cons("foo", null_cons);

  var ls_1 = list(1, 2, 3);
  var ls_2 = list(1, "bar", "foo");

  EXPECT_EQ(listp(cons_1), false);
  EXPECT_EQ(listp(cons_2), false);
  EXPECT_EQ(listp(cons_3), false);
  EXPECT_EQ(listp(cons_4), true);
  EXPECT_EQ(listp(cons_5), true);
  EXPECT_EQ(listp(cons_6), true);
  EXPECT_EQ(listp(cons_7), true);
  EXPECT_EQ(listp(ls_1), true);
  EXPECT_EQ(listp(ls_2), true);
}

TEST(Cpplisp, length) {
  using namespace cpplisp::runtime;
  //using namespace std;

  var cons_1 = cons(1, 2);
  var cons_2 = cons("foo", "bar");
  var cons_3 = cons(1, "foo");
  std::cout << "+++++++++++++++++++++" << std::endl;
  //std::cout << "cons1 size: " << length(cons_1) << std::endl;

  //EXPECT_EQ(length(cons_1), 2);
  //EXPECT_EQ(length(cons_2), 2);
  //EXPECT_EQ(length(cons_3), 2);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}