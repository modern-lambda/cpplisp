#ifdef _MSC_VER
#if _MSC_VER < 1900
#error At least Visual Studio 2015 is required
#endif
#else
#if __cplusplus <= 201103L
#error At least C++ 14 is required
#endif
#endif // _MSC_VER

#ifdef __GNUG__
#ifndef __clang__
#define __USING_GCC__
#if __cpp_concepts > 201507
#define __HAS_CONCEPTS__
#endif
#endif
#endif

#ifndef __CPPLISP_H__
#define __CPPLISP_H__

#include <cstdlib>
#include <vector>
#include <functional>
#include <string>
#include <sstream>
#include <type_traits>
#include <iostream>
#include <utility>

using std::nullptr_t;

namespace cpplisp {

#define caar(X) car(car(X))
#define cadr(X) car(cdr(X))
#define cdar(X) cdr(car(X))
#define caaar(X) car(car(car(X)))
#define caadr(X) car(car(cdr(X)))
#define cadar(X) car(cdr(car(X)))
#define cdaar(X) cdr(car(car(X)))
#define caddr(X) car(cdr(cdr(X)))
#define cdadr(X) cdr(car(cdr(X)))
#define cddar(X) cdr(cdr(car(X)))
#define caaaar(X) car(car(car(car(X))))
#define caaadr(X) car(car(car(cdr(X))))
#define caadar(X) car(car(cdr(car(X))))
#define cadaar(X) car(cdr(car(car(X))))
#define cdaaar(X) cdr(car(car(car(X))))
#define caaddr(X) car(car(cdr(cdr(X))))
#define cadadr(X) car(cdr(car(cdr(X))))
#define cdaadr(X) cdr(car(car(cdr(X))))
#define caddar(X) car(cdr(cdr(car(X))))
#define cdadar(X) cdr(car(cdr(car(X))))
#define cddaar(X) cdr(cdr(car(car(X))))
#define cadddr(X) car(cdr(cdr(cdr(X))))
#define cdaddr(X) cdr(car(cdr(cdr(X))))
#define cddadr(X) cdr(cdr(car(cdr(X))))
#define cdddar(X) cdr(cdr(cdr(car(X))))
#define cddddr(X) cdr(cdr(cdr(cdr(X))))

#define var auto

  template <typename T, typename U> class Cons;
  using nil_t = Cons<nullptr_t, nullptr_t>*;
  static const nil_t nil = nullptr;

  template <typename T, typename U>
  class Cons {
  private:
    T _car;
    U _cdr;
  public:
    Cons(T car) : _car(car), _cdr(nil) {}
    Cons(T car, U cdr) : _car(car), _cdr(cdr) {}

    T car() const {
      return _car;
    }

    U cdr() const {
      return _cdr;
    }

    Cons<T, U>* set_car(T car) {
      _car = car;
      return this;
    }

    Cons<T, U>* set_cdr(U cdr) {
      _cdr = cdr;
      return this;
    }
  };

  template <typename T, typename U>
  inline auto cons(T car, U cdr) {
    return new Cons<T, U>(car, cdr);
  }

  template <typename T>
  inline auto list(T car) {
    return cons(car, nil);
  }

  template <typename T, typename U>
  inline auto list(T car, U cadr) {
    return cons(car, cons(cadr, nil));
  }

  template <typename T, typename... Us>
  inline auto list(T car, Us... rest) {
    auto tmp = list(rest...);
    return cons(car, tmp);
  }

  template <typename T, typename U>
  inline T car(Cons<T, U>* c) {
    return c->car();
  }

  template <typename T, typename U>
  inline U cdr(Cons<T, U>* c) {
    return c->cdr();
  }

  template <typename T>
  struct _consp : std::false_type {};

  template <typename T, typename U>
  struct _consp<Cons<T, U>* > : std::true_type {};

  template <typename T>
  inline bool consp(T obj) {
    return _consp<T>::value;
  }

  template <typename T> constexpr bool consp_v = _consp<T>::value;

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
    using type = Cons<T, nil_t>*;
  };

  template <typename T, typename... U>
  struct _list_t<T, U...> {
    using type = Cons<T, typename _list_t<U...>::type>*;
  };

  template <typename T>
  struct _car_t {};
  template <>
  struct _car_t<nil_t> {
    using type = nil_t;
  };
  template <typename T, typename U>
  struct _car_t<Cons<T, U>* > {
    using type = T;
  };

  template <typename T>
  struct _cdr_t {};
  template <>
  struct _cdr_t<nil_t> {
    using type = nil_t;
  };
  template <typename T, typename U>
  struct _cdr_t<Cons<T, U>* > {
    using type = U;
  };

  template <typename T>
  struct _listp : std::false_type {};
  template <>
  struct _listp<nil_t> : std::true_type {};
  template <typename T, typename U>
  struct _listp<Cons<T, U>* > : _listp<U> {
    //static const bool value = _listp<U>::value;
  };
  template <typename T>
  inline bool listp(T) {
    return _listp<T>::value;
  }
  template <typename T> constexpr bool listp_v = _listp<T>::value;

  template <typename T>
  struct _list_len {
    static const int value = 0;
  };
  template <>
  struct _list_len<nil_t> {
    static const int value = 0;
  };
  template <typename T>
  struct _list_len<Cons<T, nil_t>* > {
    static const int value = 1;
  };
  template <typename T, typename U>
  struct _list_len<Cons<T, U>* > {
    static const int value = 1 + _list_len<U>::value;
  };

#ifdef __HAS_CONCEPTS__
  template <typename T, typename U>
  // This requires Concepts TS (Available in GCC >= 6.1 with flag `-fconcepts`)
  requires listp_v<Cons<T, U>* >
#else
  template <typename T, typename U, typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>* >>>
#endif
  inline constexpr int length(Cons<T, U>* l) {
    return _list_len<Cons<T, U>* >::value;
  }

  template <std::size_t N, typename T>
  struct _nth_t {};
  template <typename T, typename U>
  struct _nth_t <0, Cons<T, U>* > {
    using type = T;
  };
  template <std::size_t N, typename T, typename U>
  struct _nth_t <N, Cons<T, U>* > {
    using type = typename _nth_t<N - 1, U>::type;
  };

  template <std::size_t N>
  struct _nth;
  template <>
  struct _nth<0> {
    template <typename T, typename U>
    T operator() (Cons<T, U>* lst) const {
      return car(lst);
    }
  };
  template <std::size_t N>
  struct _nth {
    template <typename T, typename U>
    typename _nth_t<N, Cons<T, U>* >::type
      operator() (Cons<T, U>* lst) const {
      return _nth<N - 1>()(cdr(lst));
    }
  };

#ifdef __HAS_CONCEPTS__
  template <std::size_t N, typename T, typename U>
  requires ((N < _list_len<Cons<T, U>*>::value) && listp_v<Cons<T, U>*>)
#else
  template <std::size_t N, typename T, typename U,
            typename WithInListRange = std::enable_if_t<(N < _list_len<Cons<T, U>* >::value) && listp_v<Cons<T, U>* >>>
#endif
    auto nth(Cons<T, U>* lst) -> typename _nth_t<N, Cons<T, U>* >::type {
    return _nth<N>()(lst);
  }

  template <typename T>
  struct _nullp : std::false_type {};
  template <>
  struct _nullp<nil_t> : std::true_type {};
  template <typename T>
  inline bool nullp(T) {
    return _nullp<T>::value;
  }
  template <typename T> constexpr bool nullp_v = _nullp<T>::value;


  inline std::ostream& operator<<(std::ostream& os, Cons<nullptr_t, nullptr_t>*) {
    os << "nil";
    return os;
  }
  template <typename T>
  inline std::ostream& operator<<(std::ostream& os, Cons<T, nil_t>* c) {
    os << "(" << car(c) << ". nil)";
    return os;
  }
  template <typename T, typename U>
  inline std::ostream& operator<<(std::ostream& os, Cons<T, U>* c) {
    os << "(";
    os << car(c);
    if (cdr(c)) {
      os << " . ";
      os << cdr(c);
    }
    os << ")";
    return os;
  }

  template <typename T, typename U>
  inline std::string to_string(Cons<T, U>* c) {
    std::stringstream ss;
    ss << c;
    return ss.str();
  }

  template <typename T>
  inline bool equals(T a, T b) {
    return a == b;
  }

  template <typename T, typename U>
  inline bool equals(Cons<T, U>* lhs, Cons<T, U>* rhs) {
    if (car(lhs) == car(rhs)) {
      if (consp(cdr(lhs)) && consp(cdr(rhs))) {
        return equals(cdr(lhs), cdr(lhs));
      }
      return cdr(lhs) == cdr(rhs);
    }
    return false;
  }

  template <typename T>
  class List {
  private:
    T _head;
    List<T>* _tail;

  public:
    List() : _head(nullptr), _tail(nullptr) {}
    List(T hd) : _head(hd), _tail(nullptr) {}
    List(T hd, T tl) : _head(hd), _tail(new List(tl)) {}
    List(T hd, List<T>* lst) : _head(hd), _tail(lst) {}

    T head() const { return _head; }
    List<T>* tail() const { return _tail; }
    static List<T>* Nil() { return nullptr; }

    bool equals(List<T>& b,
      std::function<bool(T, T)> pred = [](T a, T b) { return a == b; }) {
      if (length(this) != length(b)) {
        return false;
      }
      if (pred(_head, b->head())) {
        if (_tail != nullptr) {
          return _tail->equals(b->tail(), pred);
        }
        else {
          return true;
        }
      }
      return false;
    }
  };

  template <typename T>
  inline T car(List<T>* lst) {
    return lst->head();
  }

  template <typename T>
  inline List<T>* cdr(List<T>* lst) {
    return lst->tail();
  }

  template <typename T>
  inline List<T>* lcons(T ele, List<T>* lst) {
    return new List<T>(ele, lst);
  }

  template <typename T>
  inline List<T>* tlist(T head) {
    return new List<T>(head, nullptr);
  }

  template <typename T, typename... Ts>
  inline List<T>* tlist(T head, Ts... rest) {
    return lcons(head, list(rest...));
  }

  // List-of
  template <typename T, typename U>
  struct _list_of : std::false_type {};
  template <typename T>
  struct _list_of<T, Cons<T, nil_t>* > : std::true_type {};
  template <typename T, typename S>
  struct _list_of<T, Cons<T, S>* > : _list_of<T, S> {};
  template <typename T, typename U>
  constexpr bool list_of_v = _list_of<T, U>::value;

  // M-List-Of : Multiple-level list-of, e.g.: [[T], [[T]]] gives true
  template <typename T, typename U>
  struct _m_list_of : std::false_type {};
  template <typename T>
  struct _m_list_of<T, Cons<T, nil_t>* > : std::true_type {};
  template <typename T, typename U>
  struct _m_list_of<T, Cons<U, nil_t>* > : _m_list_of<T, U> {};
  template <typename T, typename S>
  struct _m_list_of<T, Cons<T, S>* > : _m_list_of<T, S> {};
  template <typename T, typename U, typename S>
  struct _m_list_of<T, Cons<U, S>* > {
    static const bool value = _m_list_of<T, U>::value && _m_list_of<T, S>::value;
  };
  template <typename T, typename U>
  constexpr bool m_list_of_v = _m_list_of<T, U>::value;

  // APPEND
  template <typename T, typename U>
  struct _append_t {
    using type = nullptr_t;
  };
  template <typename T, typename U>
  struct _append_t<nil_t, Cons<T, U>* > {
    using type = Cons<T, U>*;
  };
  template <typename T, typename U>
  struct _append_t<Cons<T, U>*, nil_t> {
    using type = Cons<T, U>*;
  };
  template <typename T, typename S, typename Y>
  struct _append_t<T, Cons<S, Y>* > {
    using type = Cons<T, Cons<S, Y>* >*;
  };
  template <typename T, typename S, typename Y>
  struct _append_t<Cons<T, nil_t>*, Cons<S, Y>* > {
    using type = Cons<T, Cons<S, Y>* >*;
  };
  template <typename T, typename U, typename S, typename Y>
  struct _append_t<Cons<T, U>*, Cons<S, Y>* > {
    using type = Cons<T, typename _append_t<U, Cons<S, Y>* >::type>*;
  };
  template <typename S, typename Y>
  inline Cons<S, Y>* _append(nil_t, Cons<S, Y>* b) {
    return b;
  }
  template <typename T, typename U>
  inline Cons<T, U>* _append(Cons<T, U>* a, nil_t) {
    return a;
  }
  template <typename T, typename U, typename S, typename Y,
    typename R = typename _append_t<Cons<T, U>*, Cons<S, Y>*>::type>
    R _append(Cons<T, U>* a, Cons<S, Y>* b) {
    return cons(car(a), _append(cdr(a), b));
  }
#ifdef __HAS_CONCEPTS__
  template <typename T, typename U, typename S, typename Y>
  requires (listp_v<Cons<T, U>*>&& listp_v<Cons<S, Y>*>)
#else
  template <typename T, typename U, typename S, typename Y,
            typename BothProperLists = std::enable_if_t<listp_v<Cons<T, U>*>&& listp_v<Cons<S, Y>*>>>
#endif
    auto append(Cons<T, U>* a, Cons<S, Y>* b) {
    return _append(a, b);
  }

  // REVERSE
  template <typename T>
  struct _reverse_t {
    using type = nullptr_t;
  };
  template <>
  struct _reverse_t <nil_t> {
    using type = nil_t;
  };
  template <typename T>
  struct _reverse_t <Cons<T, nil_t>*> {
    using type = Cons<T, nil_t>*;
  };
  template <typename T, typename U>
  struct _reverse_t <Cons<T, Cons<U, nil_t>* >*> {
    using type = Cons<U, Cons<T, nil_t>*>*;
  };
  template <typename T, typename U>
  struct _reverse_t <Cons<T, U>*> {
    using type = typename _append_t<typename _reverse_t<U>::type, Cons<T, nil_t>* >::type;
  };
  inline nil_t _reverse(nil_t) {
    return nil;
  }
  template <typename T, typename U, typename R = typename _reverse_t<Cons<T, U>*>::type>
  R _reverse(Cons<T, U>* lst) {
    return append(_reverse(cdr(lst)), list(car(lst)));
  }
#ifdef __HAS_CONCEPTS__
  template <typename T, typename U>
  requires listp_v<Cons<T, U>*>
#else
  template <typename T, typename U,
            typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>*>>>
#endif
    auto reverse(Cons<T, U>* lst) {
    return _reverse(lst);
  }

  // MAPCAR
  template <typename T, typename U, typename ...Us>
  struct _mapcar_t {};
  template <typename F>
  struct _mapcar_t<F, nil_t> {
    using type = nil_t;
  };
  template <typename F, typename T>
  struct _mapcar_t<F, Cons<T, nil_t>*> {
    using type = Cons<typename std::result_of<F& (T)>::type, nil_t>*;
  };
  template <typename F, typename T, typename U>
  struct _mapcar_t<F, Cons<T, U>*> {
    using type = Cons<typename std::result_of<F& (T)>::type, typename _mapcar_t<F, U>::type>*;
  };
  template <typename F, typename ...S>
  struct _packed_result_t {
    using type = typename std::result_of<F& (S...)>::type;
  };
  template <typename F, typename T, typename ...S>
  struct _mapcar_t<F, Cons<T, nil_t>*, S...> {
    using type = Cons<typename _packed_result_t<F, T, typename _car_t<S>::type ...>::type,
      nil_t>*;
  };
  template <typename F, typename T, typename U, typename ...S>
  struct _mapcar_t<F, Cons<T, U>*, S...> {
    using type = Cons<typename _packed_result_t<F, T, typename _car_t<S>::type ...>::type,
      typename _mapcar_t<F, U, typename _cdr_t<S>::type ...>::type>*;
  };

  template <typename F, typename T>
  auto _mapcar(F fn, Cons<T, nil_t>* lst) {
    return list(fn(car(lst)));
  }
  template <typename F, typename T, typename U,
    typename R = typename _mapcar_t<Cons<T, U>*, F>::type>
    R _mapcar(F fn, Cons<T, U>* lst) {
    return cons(fn(car(lst)), _mapcar(fn, cdr(lst)));
  }
  template <typename F, typename T, typename ...S>
  auto _mapcar(F fn, Cons<T, nil_t>* lst, S... rest) {
    return list(fn(car(lst), car(rest)...));
  }
  template <typename F, typename T, typename U, typename ...S>
  auto _mapcar(F fn, Cons<T, U>* lst, S... rest) {
    return cons(fn(car(lst), car(rest)...), _mapcar(fn, cdr(lst), cdr(rest)...));
  }
#ifdef __HAS_CONCEPTS__
  template <typename F, typename T, typename U>
  requires listp_v<Cons<T, U>*>
#else
  template <typename F, typename T, typename U,
            typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>*>>>
#endif
    auto mapcar(F fn, Cons<T, U>* lst) {
    return _mapcar(fn, lst);
  }
#ifdef __HAS_CONCEPTS__
  template <typename F, typename T, typename U, typename ...S>
  requires listp_v<Cons<T, U>*>
#else
  template <typename F, typename T, typename U,
            typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>*>>, typename ...S>
#endif
    auto mapcar(F fn, Cons<T, U>* lst, S... rest) {
    return _mapcar(fn, lst, rest...);
  }

  template <typename R, typename C, bool cp, typename ... As>
  struct _lambda_type {
    static const bool constp = cp;
    enum { arity = sizeof...(As) };
    using return_type = R;
    using arg_type = typename _list_t<As...>::type;
  };
  template <typename L>
  struct lambda_type : lambda_type<decltype(&L::operator())> {};
  template <typename R, typename C, typename ... As>
  struct lambda_type<R(C::*)(As...)> : _lambda_type<R, C, false, As...> {};
  template <typename R, typename C, typename ... As>
  struct lambda_type<R(C::*)(As...) const> : _lambda_type<R, C, true, As...> {};

  // MULTIPLE-VALUE-BIND
  template <typename T>
  struct _values_t {
    using type = nullptr_t;
  };
  template <typename T>
  struct _values_t <Cons<T, nil_t>* > {
    using type = Cons<T*, nil_t>*;
  };
  template <typename T, typename U>
  struct _values_t <Cons<T, U>* > {
    using type = Cons<T*, typename _values_t<U>::type>*;
  };

  template <typename T, typename S = typename _values_t<Cons<T, nil_t>* >::type>
  void _mvb(Cons<T, nil_t>* lst, S sym) {
    *(car(sym)) = car(lst);
  }
  template <typename T, typename U, typename S = typename _values_t<Cons<T, U>* >::type>
  void _mvb(Cons<T, U>* lst, S sym) {
    *(car(sym)) = car(lst);
    _mvb(cdr(lst), cdr(sym));
  }


  template <typename L>
  struct get_return : get_return<decltype(&L::operator())>
  {};
  template <typename C, typename ...A>
  struct get_return <void (C::*)(A...)> {
    template <typename L>
    Cons<bool, nullptr_t>* operator()(L fn) {
      fn();
      return cons(false, nullptr);
    }
  };
  template <typename R, typename C, typename ...A>
  struct get_return <R(C::*)(A...)> {
    template <typename L>
    Cons<bool, R>* operator()(L fn) {
      return cons(true, fn());
    }
  };
  template <typename C, typename ...A>
  struct get_return <void (C::*)(A...) const> {
    template <typename L>
    Cons<bool, nullptr_t>* operator()(L fn) {
      fn();
      return cons(false, nullptr);
    }
  };
  template <typename R, typename C, typename ...A>
  struct get_return <R(C::*)(A...) const> {
    template <typename L>
    Cons<bool, R>* operator()(L fn) {
      return cons(true, fn());
    }
  };

#ifdef __HAS_CONCEPTS__
  template <typename T, typename U, typename ... Ss>
  // This requires Concepts TS (Available in GCC >= 6.1 with flag `-fconcepts`)
  requires std::is_same<typename _values_t<Cons<T, U>* >::type,
    typename _list_t<Ss ...>::type>::value&&
    _listp<Cons<T, U>* >::value
#else
  template <typename T, typename U, typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>*>>, typename ... Ss>
#endif
  auto multiple_value_bind(Cons<T, U>* c, Ss ... sym) {
    if (!std::is_same<typename _values_t<Cons<T, U>* >::type,
      typename _list_t<Ss ...>::type>::value) {
      throw "The type of Pointer-List does not match the type of the given List";
    }
    using sym_lst_t = typename _values_t<Cons<T, U>* >::type;
    sym_lst_t sym_lst = list(sym...);
    auto orig = mapcar([](auto p) { return *p; }, sym_lst);
    _mvb(c, sym_lst);
    return [orig, sym_lst](auto fn) {
      auto retval = get_return<decltype(fn)>()(fn);
      _mvb(orig, sym_lst);
      if (car(retval)) {
        return cdr(retval);
      }
      typename _cdr_t<decltype(retval)>::type ret = 0;
      return ret;
    };
  }

  // APPLY
  template <typename T, typename U, typename F, std::size_t ... A>
  auto _apply(F fn, Cons<T, U>* lst, std::index_sequence<A...>) {
    return fn(nth<A>(lst)...);
  }
#ifdef __HAS_CONCEPTS__
  template <typename T, typename U, typename F>
  requires std::is_same_v<lambda_type<F>::arg_type, Cons<T, U>*>
#else
  template< class T, class U >
  constexpr bool is_same_v = std::is_same<T, U>::value;
  template <typename T, typename U, typename F,
            typename ArgTypesMatch = std::enable_if_t<cpplisp::is_same_v<lambda_type<F>::arg_type, Cons<T, U>*>>>
#endif
    auto apply(F fn, Cons<T, U>* lst) -> typename lambda_type<F>::return_type {
    return _apply(fn, lst, std::make_index_sequence<lambda_type<F>::arity>());
  }
}

#endif