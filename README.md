# cpplisp
lisp favour dsl in cpp. header only. runtime and compile time will be supported.

example:

```cpp
#include <vector>
#include <string>
#include "cpplisp.hpp"

using namespace cpplisp::runtime;
using namespace cpplisp::runtime::prettyprint;

int main() {
  var lt = list(1, "bar", std::vector<int>{1, 2, 3}, std::string("foo"));
  var ret = cadr(lt); // "bar"

  var ls_1 = list(1, 2, 3);
  int v1, v2, v3;
  std::string v4;
  var ls_4 = multiple_value_bind(append(ls_1, list((std::string)"foo")), &v1, &v2, &v3, &v4)([=] () {
    std::cout << "v1: " << v1 << " v2: " << v2 << " v3: " << v3 << " v4: " << v4 << std::endl;
    return list(v4, v1, v2, v3);
  });
}

```
