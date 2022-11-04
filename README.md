# cpplisp
lisp favour dsl in cpp. header only. runtime and compile time will be supported.

example:

```cpp
#include <vector>
#include <string>
#include "cpplisp.hpp"

using namespace cpplisp::runtime;

int main() {
  var lt = list(1, "bar", std::vector<int>{1, 2, 3}, std::string("foo"));
  var ret = cadr(lt); // "bar"
}

```
