// This file is distributed under the GNU GENERAL PUBLIC LICENSE.
// See "LICENSE" for details.
// Copyright 2018-2022, CanftIn (wwc7033@gmail.com)
// This is an open source non-commercial project.

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
  using ConsPtr = std::shared_ptr<Cons<T, U>>;

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

  /// @brief [symbols] cons: T -> U -> Cons<T, U>
  /// @tparam T 
  /// @tparam U 
  /// @param car 
  /// @param cdr 
  /// @return 
  template <typename T, typename U>
  inline auto cons(T&& car, U&& cdr) noexcept {
    return std::make_shared<Cons<T, U>>(
      std::forward<T>(car),
      std::forward<U>(cdr)
    );
  }

  /// @brief [symbols] list: T -> Cons<T, nil_t>
  /// @tparam T 
  /// @param car 
  /// @return 
  template <typename T>
  inline auto list(T&& car) {
    return cons(std::forward<T>(car), nil);
  }

  /// @brief [symbols] list: T -> U -> Cons<T, Cons<U, nil_t>>
  /// @tparam T 
  /// @tparam U 
  /// @param car 
  /// @param cadr 
  /// @return 
  template <typename T, typename U>
  inline auto list(T&& car, U&& cadr) {
    return cons(
      std::forward<T>(car),
      std::move(cons(std::forward<U>(cadr), nil))
    );
  }

  /// @brief [symbols] list: T -> ...Rest -> cons(T, list(Rest...))
  /// @tparam T 
  /// @tparam ...Rest 
  /// @param car 
  /// @param ...rest 
  /// @return 
  template <typename T, typename... Rest>
  inline auto list(T&& car, Rest&&... rest) {
    return cons(
      std::forward<T>(car),
      std::move(list(std::forward<Rest>(rest)...))
    );
  }

  /// @brief [symbols] car: ConsPtr<T, U> -> T
  /// @tparam T 
  /// @tparam U 
  /// @param c 
  /// @return 
  template <typename T, typename U>
  inline T car(ConsPtr<T, U> c) {
    return c->car();
  }

  /// @brief [symbols] cdr: ConsPtr<T, U> -> U
  /// @tparam T 
  /// @tparam U 
  /// @param c 
  /// @return 
  template <typename T, typename U>
  inline U cdr(ConsPtr<T, U> c) {
    return c->cdr();
  }

  template <typename T>
  struct _consp : std::false_type { };

  template <typename T, typename U>
  struct _consp<ConsPtr<T, U>> : std::true_type { };

  /// @brief [symbols] consp: (type a) => a -> bool
  /// @tparam T 
  /// @param  
  /// @return 
  template <typename T>
  inline bool consp(T) {
    return _consp<T>::value;
  }

  template <typename T>
  constexpr bool consp_v = _consp<T>::value;

  template <typename... T>
  struct _list_t {
    using type = nullptr_t;
  };

  template <>
  struct _list_t<nullptr_t> {
    using type = nil_t;
  };

  template <typename T>
  struct _list_t<T> {
    using type = ConsPtr<T, nil_t>;
  };

  template <typename T, typename... U>
  struct _list_t<T, U...> {
    using type = ConsPtr<T, typename _list_t<U...>::type>;
  };

  template <typename T>
  struct _car_t { };

  template <>
  struct _car_t<nil_t> {
    using type = nil_t;
  };

  template <typename T, typename U>
  struct _car_t<ConsPtr<T, U>> {
    using type = T;
  };

  template <typename T>
  struct _cdr_t { };

  template <>
  struct _cdr_t<nil_t> {
    using type = nil_t;
  };

  template <typename T, typename U>
  struct _cdr_t<ConsPtr<T, U>> {
    using type = U;
  };

  template <typename T>
  struct _listp : std::false_type { };

  template <>
  struct _listp<nil_t> : std::true_type { };

  template <>
  struct _listp<nil_t&> : std::true_type { };

  template <>
  struct _listp<const nil_t&> : std::true_type { };

  template <typename T, typename U>
  struct _listp<ConsPtr<T, U>> : _listp<U> { };

  /// @brief [symbols] listp: (type a) => a -> bool
  /// @tparam T 
  /// @param 
  /// @return 
  template <typename T>
  inline bool listp(T) {
    return _listp<T>::value;
  }

  template <typename T>
  constexpr bool listp_v = _listp<T>::value;

  template <typename T>
  struct _list_len {
    static const int value = 0;
  };

  template <>
  struct _list_len<nil_t> {
    static const int value = 0;
  };

  template <typename T>
  struct _list_len<ConsPtr<T, nil_t>> {
    static const int value = 1;
  };

  template <typename T, typename U>
  struct _list_len<ConsPtr<T, U>> {
    static const int value = 1 + _list_len<U>::value;
  };

  /// @brief [symbols] length: ConsPtr<T, U> -> int
  /// @tparam T 
  /// @tparam U 
  /// @tparam  
  /// @param l 
  /// @return 
  //template <typename T, typename U,
  //          typename IsProperList = std::enable_if_t<listp_v<ConsPtr<T, U>>>>
  //inline constexpr int length(ConsPtr<T, U> l) {
  //  return _list_len<ConsPtr<T, U>>::value;
  //}


} // namespace runtime


} // namespace cpplisp


#endif // CPPLISP_HPP__
