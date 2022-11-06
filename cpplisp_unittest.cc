#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "cpplisp.hpp"

TEST(Cpplisp, cons) {
  using namespace cpplisp::runtime;

  var cons_1 = cons(1, 2);
  var cons_2 = cons("foo", "bar");
  var cons_3 = cons(1, "foo");

  EXPECT_EQ(consp(cons_1), true);
  EXPECT_EQ(consp(cons_2), true);
  EXPECT_EQ(consp(cons_3), true);
}

TEST(Cpplisp, listp) {
  using namespace cpplisp::runtime;

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

  var cons_1 = cons(1, cons("foo", nil));
  var ls_1 = list(1, 2, 3);
  var ls_2 = list(1, "bar", "foo");

  var lt = list(1, "bar", std::vector<int>{1, 2, 3}, std::string("foo"));

  EXPECT_EQ(length(cons_1), 2);
  EXPECT_EQ(length(ls_1), 3);
  EXPECT_EQ(length(ls_2), 3);
  EXPECT_EQ(length(lt), 4);
}

TEST(Cpplisp, car) {
  using namespace cpplisp::runtime;

  var cons_1 = cons(1, cons("foo", nil));
  var ls_1 = list(1, 2, 3);
  var ls_2 = list(1, "bar", "foo");

  var lt = list(1, "bar", std::vector<int>{1, 2, 3}, std::string("foo"));

  EXPECT_EQ(car(cons_1), 1);
  EXPECT_EQ(cadr(cons_1), "foo");
}

TEST(Cpplisp, nth) {
  using namespace cpplisp::runtime;

  var cons_1 = cons(1, cons("foo", nil));
  var ls_1 = list(1, 2, 3);
  var ls_2 = list(1, "bar", "foo");

  var lt = list(1, "bar", std::vector<int>{1, 2, 3}, std::string("foo"));

  EXPECT_EQ(nth<0>(cons_1), 1);
  EXPECT_EQ(nth<0>(lt), 1);
}

TEST(Cpplisp, nullp) {
  using namespace cpplisp::runtime;

  var cons_1 = cons(1, cons("foo", nil));
  var ls_1 = list(1, 2, 3);
  var ls_2 = list(1, "bar", "foo");

  var lt = list(1, "bar", std::vector<int>{1, 2, 3}, std::string("foo"));

  EXPECT_EQ(nullp(nil), true);
  EXPECT_EQ(nullp(cddr(cons_1)), true);
  EXPECT_EQ(nullp(cddr(lt)), false);
  EXPECT_EQ(nullp(cddddr(lt)), true);
}

TEST(Cpplisp, prettyprint) {
  using namespace cpplisp::runtime;
  using namespace cpplisp::runtime::prettyprint;

  var cons_1 = cons(1, cons("foo", nil));
  var ls_1 = list(1, 2, 3);
  var ls_2 = list(1, "bar", "foo");

  //var lt = list(1, "bar", std::vector<int>{1, 2, 3}, std::string("foo"));

  EXPECT_EQ(to_string(cons_1), "(1 . (foo . nil))");
  EXPECT_EQ(to_string(ls_1), "(1 . (2 . (3 . nil)))");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}