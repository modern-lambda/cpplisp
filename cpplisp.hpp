#ifndef CPPLISP_HPP__
#define CPPLISP_HPP__

#include <cstdlib>
#include <vector>
#include <functional>
#include <string>
#include <sstream>
#include <type_traits>
#include <iostream>
#include <utility>
#include <memory>

using std::nullptr_t;

namespace cpplisp {
  
namespace runtime {
  
#define caar(X) car(car(X))
#define cadr(X) car(cdr(X))
#define cdar(X) cdr(car(X))
#define cddr(X) cdr(cdr(X))
#define caaar(X) car(car(car(X)))
#define caadr(X) car(car(cdr(X)))
#define cadar(X) car(cdr(car(X)))
#define cdaar(X) cdr(car(car(X)))
#define caddr(X) car(cdr(cdr(X)))
#define cdadr(X) cdr(car(cdr(X)))
#define cddar(X) cdr(cdr(car(X)))
#define cdddr(X) cdr(cdr(cdr(X)))
#define caaaar(X) car(car(car(car(X))))
#define caaadr(X) car(car(car(cdr(X))))
#define caadar(X) car(car(cdr(car(X))))
#define caaddr(X) car(car(cdr(cdr(X))))
#define cadaar(X) car(cdr(car(car(X))))
#define cadadr(X) car(cdr(car(cdr(X))))
#define caddar(X) car(cdr(cdr(car(X))))
#define cadddr(X) car(cdr(cdr(cdr(X))))
#define cdaaar(X) cdr(car(car(car(X))))
#define cdaadr(X) cdr(car(car(cdr(X))))
#define cdadar(X) cdr(car(cdr(car(X))))
#define cdaddr(X) cdr(car(cdr(cdr(X))))
#define cddaar(X) cdr(cdr(car(car(X))))
#define cddadr(X) cdr(cdr(car(cdr(X))))
#define cdddar(X) cdr(cdr(cdr(car(X))))
#define cddddr(X) cdr(cdr(cdr(cdr(X))))

#define first(X) (car(X))
#define second(X) (car(cdr(X)))
#define third(X) (car(cddr(X)))
#define fourth(X) (car(cdddr(X)))
#define fifth(X) (car(cddddr(X)))
#define sixth(X) (car(cdr(cddddr(X))))
#define seventh(X) (car(cddr(cddddr(X))))
#define eighth(X) (car(cdddr(cddddr(X))))
#define ninth(X) (car(cddddr(cddddr(X))))
#define tenth(X) (car(cdr(cddddr(cddddr(X)))))

#define var auto

  template <typename T, typename U> class Cons;

  template <typename T, typename U>
  using ConsPtr = std::unique_ptr<Cons<T, U>>;

  using nil_t = ConsPtr<nullptr_t, nullptr_t>;

  static const nil_t nil = nullptr;

  template <typename T, typename U>
  class Cons {
   private:
    T _car;
    U _cdr;
   public:
    Cons(T car)
      : _car(car),
        _cdr(nil)
    { }

    Cons(T car, U cdr)
      : _car(car),
        _cdr(cdr)
    { }

    T car() const {
      return _car;
    }

    U cdr() const {
      return _cdr;
    }

    ConsPtr<T, U> set_car(T car) {
      _car = car;
      return ConsPtr<T, U>(this);
    }

    ConsPtr<T, U> set_cdr(U cdr) {
      _cdr = cdr;
      return ConsPtr<T, U>(this);
    }
  };

  template <typename T, typename U>
  inline auto cons(T&& car, U&& cdr) noexcept {
    return std::make_unique<Cons<T, U>>(
      std::forward<T>(car),
      std::forward<U>(cdr)
    );
  }

  template <typename T>
  inline auto list(T&& car) {
    return cons(std::forward<T>(car), nil);
  }

  template <typename T, typename U>
  inline auto list(T&& car, U&& cadr) {
    return cons(
      std::forward<T>(car),
      std::move(cons(std::forward<U>(cadr), nil))
    );
  }

  template <typename T, typename... Rest>
  inline auto list(T&& car, Rest&&... rest) {
    return cons(
      std::forward<T>(car),
      std::move(list(std::forward<Rest>(rest)...))
    );
  }

  template <typename T, typename U>
  inline T car(ConsPtr<T, U> c) {
    return c->car();
  }

  template <typename T, typename U>
  inline U cdr(ConsPtr<T, U> c) {
    return c->cdr();
  }

  template <typename T>
  struct _consp : std::false_type { };
  template <typename T, typename U>
  struct _consp<ConsPtr<T, U>> : std::true_type { };
  template <typename T>
  inline bool consp(T) {
    return _consp<T>::value;
  }

} // namespace runtime


} // namespace cpplisp


#endif // CPPLISP_HPP__
