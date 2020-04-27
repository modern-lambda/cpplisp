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

namespace run_time {

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
  template <typename T, typename U,
            typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>* >>>
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
  requires ((N < _list_len<Cons<T, U>* >::value) && listp_v<Cons<T, U>* >)
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
  template <typename T>
  constexpr bool nullp_v = _nullp<T>::value;


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
            typename R = typename _append_t<Cons<T, U>*, Cons<S, Y>* >::type>
    R _append(Cons<T, U>* a, Cons<S, Y>* b) {
    return cons(car(a), _append(cdr(a), b));
  }
#ifdef __HAS_CONCEPTS__
  template <typename T, typename U, typename S, typename Y>
  requires (listp_v<Cons<T, U>* >&& listp_v<Cons<S, Y>* >)
#else
  template <typename T, typename U, typename S, typename Y,
            typename BothProperLists = std::enable_if_t<listp_v<Cons<T, U>* >&& listp_v<Cons<S, Y>* >>>
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
  struct _reverse_t <Cons<T, nil_t>* > {
    using type = Cons<T, nil_t>*;
  };
  template <typename T, typename U>
  struct _reverse_t <Cons<T, Cons<U, nil_t>* >* > {
    using type = Cons<U, Cons<T, nil_t>* >*;
  };
  template <typename T, typename U>
  struct _reverse_t <Cons<T, U>* > {
    using type = typename _append_t<typename _reverse_t<U>::type, Cons<T, nil_t>* >::type;
  };
  inline nil_t _reverse(nil_t) {
    return nil;
  }
  template <typename T, typename U, typename R = typename _reverse_t<Cons<T, U>* >::type>
  R _reverse(Cons<T, U>* lst) {
    return append(_reverse(cdr(lst)), list(car(lst)));
  }
#ifdef __HAS_CONCEPTS__
  template <typename T, typename U>
  requires listp_v<Cons<T, U>* >
#else
  template <typename T, typename U,
            typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>* >>>
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
  struct _mapcar_t<F, Cons<T, nil_t>* > {
    using type = Cons<typename std::result_of<F& (T)>::type, nil_t>*;
  };
  template <typename F, typename T, typename U>
  struct _mapcar_t<F, Cons<T, U>* > {
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
  requires listp_v<Cons<T, U>* >
#else
  template <typename F, typename T, typename U,
            typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>* >>>
#endif
    auto mapcar(F fn, Cons<T, U>* lst) {
    return _mapcar(fn, lst);
  }
#ifdef __HAS_CONCEPTS__
  template <typename F, typename T, typename U, typename ...S>
  requires listp_v<Cons<T, U>* >
#else
  template <typename F, typename T, typename U,
            typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>* >>, typename ...S>
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
  template <typename T, typename U, typename IsProperList = std::enable_if_t<listp_v<Cons<T, U>* >>, typename ... Ss>
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
  requires std::is_same_v<lambda_type<F>::arg_type, Cons<T, U>* >
#else
  template< class T, class U >
  constexpr bool is_same_v = std::is_same<T, U>::value;
  template <typename T, typename U, typename F,
            typename ArgTypesMatch = std::enable_if_t<cpplisp::run_time::is_same_v<lambda_type<F>::arg_type, Cons<T, U>* >>>
#endif
    auto apply(F fn, Cons<T, U>* lst) -> typename lambda_type<F>::return_type {
    return _apply(fn, lst, std::make_index_sequence<lambda_type<F>::arity>());
  }

} // namespace run_time


namespace util {

  struct EmptyType;

  template<bool>
  struct ComileTimeCheck;
  template<>
  struct ComileTimeCheck<true> {};

  template<class T1, class T2>
  struct StaticCheckEQ;
  template<class T>
  struct StaticCheckEQ<T, T> {};

  template<class T1, class T2>
  struct IsEQ {
    static const bool value = false;
  };
  template<class T>
  struct IsEQ<T, T> {
    static const bool value = true;
  };

  template<class T1, class T2>
  struct NotEQ {
    static const bool value = true;
  };
  template<class T>
  struct NotEQ<T, T> {
    static const bool value = false;
  };

  template<int N>
  struct Int2Type {
    static const int v = N;
  };

  template<bool B, class T1, class T2> struct Select;
  template<class T1, class T2>
  struct Select<true, T1, T2> {
    using value = T1;
  };
  template<class T1, class T2>
  struct Select<false, T1, T2> {
    using value = T2;
  };

  template<bool B, class T> struct Enable_If;
  template<class T>
  struct Enable_If<true, T> {
    using type = T;
  };

  template<class Head, class Tail> struct Cons;

  template<class Pair> struct Car;
  template<class Head, class Tail>
  struct Car<Cons<Head, Tail>> {
    using value = Head;
  };

  template<class Pair> struct Cdr;
  template<class Head, class Tail>
  struct Cdr<Cons<Head, Tail>> {
    using value = Tail;
  };

  template<class Head, class... Tail>
  struct TypeList {
    using value = Cons<Head, typename TypeList<Tail...>::value>;
  };
  template<class Head>
  struct TypeList<Head> {
    using value = Cons<Head, EmptyType>;
  };

} // namespace util

namespace internal {

  using cpplisp::util::EmptyType;
  using cpplisp::util::Select;
  using cpplisp::util::IsEQ;

  struct EmptyVarValList;
  template<class Varible, class Val, class Tail>
  struct VarValList;
  template<class Varible, class Val>
  struct VarValList<Varible, Val, EmptyVarValList>;


  template<class Varible, class Val, class L>
  struct VarValListExtend {
    using value = VarValList<Varible, Val, L>;
  };


  template<class V, class L> struct VarValListLookup;
  template<class V, class Varible, class Val, class Tail>
  struct VarValListLookup<V, VarValList<Varible, Val, Tail>> {
    using value = typename VarValListLookup<V, Tail>::value;
  };
  template<class V, class Val, class Tail>
  struct VarValListLookup<V, VarValList<V, Val, Tail>> {
    using value = Val;
  };
  template<class V>
  struct VarValListLookup<V, EmptyVarValList> {
    using value = EmptyType;
  };


  struct EmptyEnv;
  template<class Head, class Tail>
  struct Env;
  template<class T>
  struct Env<T, EmptyEnv>;


  template<class Head, class E>
  struct EnvExtend {
    using value = Env<Head, E>;
  };


  template<class V, class E>
  struct EnvLookup;

  template<class V, class L, class ETail>
  struct EnvLookup<V, Env<L, ETail>> {
      using ResultType = typename VarValListLookup<V, L>::value;
      using value = typename Select<IsEQ<ResultType, EmptyType>::value,
                                    typename EnvLookup<V, ETail>::value,
                                    ResultType >::value;
  };
  template<class V>
  struct EnvLookup<V, EmptyEnv> {
    using value = EmptyType;
  };


  template<class Kont, class Val>
  struct ApplyKont;

  struct EndKont;

  template<class Val>
  struct ApplyKont<EndKont, Val> {
    using value = Val;
  };


  template<class Exp, class Environ, class Kont>
  struct EvalUnderEnvKont;

  template<class Exp>
  struct Eval {
    using value = typename EvalUnderEnvKont<Exp, EmptyEnv, EndKont>::value;
  };


  template<int N>
  struct Int {
    static const int value = N;
  };
  template<bool B>
  struct Bool {
    static const bool value = B;
  };
  template<int N>
  struct Var;

  struct Unit;

  template<int N, class Environ, class Kont>
  struct EvalUnderEnvKont<Int<N>, Environ, Kont> {
    using value = typename ApplyKont<Kont, Int<N> >::value;
  };

  template<bool B, class Environ, class Kont>
  struct EvalUnderEnvKont<Bool<B>, Environ, Kont> {
    using value = typename ApplyKont<Kont, Bool<B> >::value;
  };

  template<int N, class Environ, class Kont>
  struct EvalUnderEnvKont<Var<N>, Environ, Kont> {
    using value = typename ApplyKont<Kont,
                                    typename EnvLookup<Var<N>,
                                                        Environ>::value>::value;
  };

  template<class Environ, class Kont>
  struct EvalUnderEnvKont<Unit, Environ, Kont> {
    using value = typename ApplyKont<Kont, Unit>::value;
  };


  template<class T1, class T2> struct Add;


  template<class AddendExp, class Environ, class Kont>
  struct AddKont1;
  template<class Addend, class Kont>
  struct AddKont2;


  template<class AddendExp, class Environ, class Kont, int N>
  struct ApplyKont<AddKont1<AddendExp, Environ, Kont>, Int<N>> {
    using value = typename EvalUnderEnvKont<AddendExp, Environ,
                                            AddKont2<Int<N>, Kont>>::value;
  };


  template<int N1, class Kont, int N2>
  struct ApplyKont< AddKont2<Int<N1>, Kont>, Int<N2>> {
    using value = typename ApplyKont<Kont, Int<N1+N2>>::value;
  };


  template<class T1, class T2, class Environ, class Kont>
  struct EvalUnderEnvKont<Add<T1, T2>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T1, Environ,
                                            AddKont1<T2, Environ, Kont>>::value;
  };


  template<class T1, class T2>
  struct Mul;


  template<class MultiplierExp, class Environ, class Kont>
  struct MulKont1;
  template<class Multiplier, class Kont>
  struct MulKont2;


  template<class MultiplierExp, class Environ, class Kont, int N>
  struct ApplyKont<MulKont1<MultiplierExp, Environ, Kont>, Int<N>> {
    using value = typename EvalUnderEnvKont<MultiplierExp, Environ,
                                            MulKont2<Int<N>, Kont>>::value;
  };


  template<int N1, class Kont, int N2>
  struct ApplyKont<MulKont2<Int<N1>, Kont>, Int<N2>> {
    using value = typename ApplyKont<Kont, Int<N1*N2>>::value;
  };

  // Mul Eval
  template<class T1, class T2, class Environ, class Kont>
  struct EvalUnderEnvKont<Mul<T1, T2>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T1, Environ,
                                            MulKont1<T2, Environ, Kont>>::value;
  };


  template<class T1, class T2>
  struct Mod;


  template<class ModendExp, class Environ, class Kont>
  struct ModKont1;
  template<class Modend, class Kont>
  struct ModKont2;


  template<class ModendExp, class Environ, class Kont, int N>
  struct ApplyKont<ModKont1<ModendExp, Environ, Kont>, Int<N>> {
    using value = typename EvalUnderEnvKont<ModendExp, Environ,
                                            ModKont2<Int<N>, Kont>>::value;
  };


  template<int N1, class Kont, int N2>
  struct ApplyKont<ModKont2< Int<N1>, Kont>, Int<N2>> {
    using value = typename ApplyKont<Kont, Int<N1%N2>>::value;
  };


  template<class T1, class T2, class Environ, class Kont>
  struct EvalUnderEnvKont<Mod<T1, T2>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T1, Environ,
                                            ModKont1<T2, Environ, Kont>>::value;
  };


  template<class T1, class T2>
  struct IsGreater;


  template<class GreaterExp, class Environ, class Kont>
  struct IsGreaterKont1;
  template<class Greaterend, class Kont>
  struct IsGreaterKont2;


  template<class GreaterExp, class Environ, class Kont, int N>
  struct ApplyKont<IsGreaterKont1<GreaterExp, Environ, Kont>, Int<N>> {
    using value = typename EvalUnderEnvKont<GreaterExp, Environ,
                                            IsGreaterKont2<Int<N>, Kont>>::value;
  };


  template<int N1, class Kont, int N2>
  struct ApplyKont<IsGreaterKont2<Int<N1>, Kont>, Int<N2>> {
    using value = typename ApplyKont<Kont, Bool<(N1>N2)>>::value;
  };


  template<class T1, class T2, class Environ, class Kont>
  struct EvalUnderEnvKont<IsGreater<T1, T2>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T1, Environ,
                                            IsGreaterKont1<T2, Environ, Kont>>::value;
  };


  template<class T1, class T2>
  struct IsLess;


  template<class LessExp, class Environ, class Kont>
  struct IsLessKont1;
  template<class Lessend, class Kont>
  struct IsLessKont2;


  template<class LessExp, class Environ, class Kont, int N>
  struct ApplyKont<IsLessKont1<LessExp, Environ, Kont>, Int<N>> {
    using value = typename EvalUnderEnvKont<LessExp, Environ,
                                            IsLessKont2< Int<N>, Kont>>::value;
  };


  template<int N1, class Kont, int N2>
  struct ApplyKont<IsLessKont2<Int<N1>, Kont>, Int<N2>> {
    using value = typename ApplyKont<Kont, Bool<(N1<N2)>>::value;
  };


  template<class T1, class T2, class Environ, class Kont>
  struct EvalUnderEnvKont<IsLess<T1, T2>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T1, Environ,
                                            IsLessKont1<T2, Environ, Kont>>::value;
  };


  template<class T1, class T2>
  struct IsEqual;


  template<class EqualExp, class Environ, class Kont>
  struct IsEqualKont1;
  template<class Equalend, class Kont>
  struct IsEqualKont2;


  template<class EqualExp, class Environ, class Kont, int N>
  struct ApplyKont<IsEqualKont1<EqualExp, Environ, Kont>, Int<N>> {
    using value = typename EvalUnderEnvKont<EqualExp, Environ,
                                            IsEqualKont2<Int<N>, Kont>>::value;
  };


  template<int N1, class Kont, int N2>
  struct ApplyKont<IsEqualKont2<Int<N1>, Kont>, Int<N2>> {
    using value = typename ApplyKont<Kont, Bool<(N1==N2)>>::value;
  };


  template<class T1, class T2, class Environ, class Kont>
  struct EvalUnderEnvKont<IsEqual<T1, T2>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T1, Environ, 
                                            IsEqualKont1<T2, Environ, Kont>>::value;
  };


  template<class T>
  struct IsUnit;


  template<class Kont>
  struct IsUnitKont;

  template<class Kont>
  struct ApplyKont<IsUnitKont<Kont>, Unit> {
    using value = typename ApplyKont<Kont, Bool<true>>::value;
  };

  template<class Kont, class Val>
  struct ApplyKont< IsUnitKont<Kont>, Val> {
    using value = typename ApplyKont<Kont, Bool<false>>::value;
  };


  template<class T, class Environ, class Kont>
  struct EvalUnderEnvKont<IsUnit<T>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T, Environ,
                                            IsUnitKont<Kont>>::value;
  };



  template<class T1, class T2>
  struct Pair;


  template<class PairExp, class Environ, class Kont>
  struct PairKont1;
  template<class Pairend, class Kont>
  struct PairKont2;


  template<class PairExp, class Environ, class Kont, class Val>
  struct ApplyKont<PairKont1<PairExp, Environ, Kont>, Val> {
    using value = typename EvalUnderEnvKont<PairExp, Environ,
                                            PairKont2<Val, Kont>>::value;
  };


  template<class Val1, class Kont, class Val2>
  struct ApplyKont<PairKont2<Val1, Kont>, Val2> {
    using value = typename ApplyKont<Kont, Pair<Val1, Val2>>::value;
  };


  template<class T1, class T2, class Environ, class Kont>
  struct EvalUnderEnvKont<Pair<T1, T2>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T1, Environ,
                                            PairKont1<T2, Environ, Kont>>::value;
  };


  template<class T>
  struct IsPair;


  template<class Kont>
  struct IsPairKont;

  template<class Kont, class T1, class T2>
  struct ApplyKont<IsPairKont<Kont>, Pair<T1, T2>> {
    using value = typename ApplyKont<Kont, Bool<true>>::value;
  };

  template<class Kont, class Val>
  struct ApplyKont<IsPairKont<Kont>, Val> {
    using value = typename ApplyKont<Kont, Bool<false>>::value;
  };


  template<class T, class Environ, class Kont>
  struct EvalUnderEnvKont<IsPair<T>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T, Environ,
                                            IsPairKont<Kont>>::value;
  };


  template<class T>
  struct Fst;


  template<class Kont>
  struct FstKont;

  template<class Kont, class T1, class T2>
  struct ApplyKont<FstKont<Kont>, Pair<T1, T2>> {
    using value = typename ApplyKont<Kont, T1>::value;
  };


  template<class T, class Environ, class Kont>
  struct EvalUnderEnvKont<Fst<T>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T, Environ,
                                            FstKont<Kont>>::value;
  };


  template<class T>
  struct Snd;


  template<class Kont>
  struct SndKont;

  template<class Kont, class T1, class T2>
  struct ApplyKont<SndKont<Kont>, Pair<T1, T2>> {
    using value = typename ApplyKont<Kont, T2>::value;
  };


  template<class T, class Environ, class Kont>
  struct EvalUnderEnvKont<Snd<T>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<T, Environ, SndKont<Kont>>::value;
  };


  template<class... T>
  struct List;


  template<class T, class... TRest, class Environ, class Kont>
  struct EvalUnderEnvKont<List<T, TRest...>, Environ, Kont> {
    static const int ArgNum = sizeof...(TRest) + 1;
    using TmpKont = typename Select<ArgNum == 1,
                                    PairKont1<Unit, Environ, Kont>,
                                    PairKont1<List<TRest...>, Environ, Kont>>::value;
    using value = typename EvalUnderEnvKont<T, Environ, TmpKont>::value;
  };



  template<class Cond, class T1, class T2> 
  struct If_Then_Else;


  template<class Branch1, class Branch2, class Environ, class Kont> 
  struct BranchKont;

  template<class Branch1, class Branch2, class Environ, class Kont>
  struct ApplyKont<BranchKont<Branch1, Branch2, Environ, Kont>, Bool<true>> {
    using value = typename EvalUnderEnvKont<Branch1, Environ, Kont>::value;
  };

  template<class Branch1, class Branch2, class Environ, class Kont>
  struct ApplyKont<BranchKont<Branch1, Branch2, Environ, Kont>, Bool<false>> {
    using value = typename EvalUnderEnvKont<Branch2, Environ, Kont>::value;
  };


  template<class Cond, class Branch1, class Branch2, class Environ, class Kont>
  struct EvalUnderEnvKont<If_Then_Else<Cond, Branch1, Branch2>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<Cond, Environ,
                                            BranchKont<Branch1, Branch2, Environ, Kont>>::value;
  };


  template<class Environ, class Fn>
  struct Closure;

  template<class E, class Fn, class Environ, class Kont>
  struct EvalUnderEnvKont<Closure<E, Fn>, Environ, Kont> {
    using value = typename ApplyKont<Kont, Closure<E, Fn>>::value;
  };


  template<class... Param>
  struct ParamList;


  template<class ParamL, class Body>
  struct Lambda;

  template<class Param, class... ParamTail, class Body, class Environ, class Kont>
  struct EvalUnderEnvKont<Lambda<ParamList<Param, ParamTail...>, Body>, Environ, Kont> {
    using PL = ParamList<Param, ParamTail...>;
    using value = typename ApplyKont<Kont, Closure<Environ, Lambda<PL, Body>>>::value;
  };


  template<class Kont,
          class VarValL,
          class Fn,
          class Environ, class... Val>
  struct BindKont;


  template<class Kont,
           class VarValL,
           class E, class P, class... PRest, class Body,
           class Environ, class Val, class... ValRest,
           class V>
  struct ApplyKont<BindKont<Kont,
                            VarValL,
                            Closure<E, Lambda<ParamList<P, PRest...>, Body>>,
                            Environ, Val, ValRest...>,
                   V> {
    using value = typename EvalUnderEnvKont<Val,
                                            Environ,
                                            BindKont<Kont,
                                                     typename VarValListExtend<P, V, VarValL>::value,
                                                     Closure<E, Lambda<ParamList<PRest...>, Body>>,
                                                     Environ, ValRest...>>::value;
  };

  template<class Kont,
          class VarValL,
          class E, class P, class Body,
          class Environ,
          class V>
  struct ApplyKont<BindKont<Kont,
                            VarValL,
                            Closure<E, Lambda<ParamList<P>, Body>>,
                            Environ>,
                  V> {
    using value = typename EvalUnderEnvKont<Body,
                                            typename EnvExtend<typename VarValListExtend<P, V, VarValL>::value,
                                                               E>::value,
                                            Kont>::value;
  };


  template<class Fn, class... Val>
  struct Call;


  template<class Kont, class Environ, class... Val>
  struct CallKont;

  template<class Kont, class Environ, class Val, class... ValRest, class Fn>
  struct ApplyKont<CallKont<Kont, Environ, Val, ValRest...>, Fn> {
    using value = typename EvalUnderEnvKont<Val,
                                            Environ,
                                            BindKont<Kont,
                                                     EmptyVarValList,
                                                     Fn,
                                                     Environ, ValRest...>>::value;
  };


  template<class Fn, class... Val, class Environ, class Kont>
  struct EvalUnderEnvKont<Call<Fn, Val...>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<Fn, Environ,
                                            CallKont<Kont, Environ, Val...>>::value;
  };


  template<class Kont> struct CC;

  template<class Kont, class Environ, class Cont>
  struct EvalUnderEnvKont<CC<Kont>, Environ, Cont> {
    using value = typename ApplyKont<Kont, typename EnvLookup<Var<0>, Environ>::value>::value;
  };


  template<class Fn>
  struct CallCC;


  template<class Kont>
  struct CallCCKont;

  template<class Kont, class Environ, int N, class Body>
  struct ApplyKont<CallCCKont<Kont>, Closure<Environ, Lambda<ParamList<Var<N>>, Body>>> {
    using CC_Closure = Closure<EmptyEnv, Lambda<ParamList<Var<0>>, CC<Kont>>>;

    using value = typename EvalUnderEnvKont<Body,
                                            typename EnvExtend<typename VarValListExtend<Var<N>,
                                                                                         CC_Closure,
                                                                                         EmptyVarValList>::value,
                                                               Environ>::value,
                                            Kont>::value;
  };


  template<class Fn, class Environ, class Kont>
  struct EvalUnderEnvKont<CallCC<Fn>, Environ, Kont> {
    using value = typename EvalUnderEnvKont<Fn, Environ,
                                            CallCCKont<Kont>>::value;
  };


} // namespace internal

namespace compile_time {

  // Eval
  using cpplisp::internal::Eval;          // Eval<Exp>

  // Basic Data Type
  using cpplisp::internal::Int;           // Int<N>
  using cpplisp::internal::Bool;          // Bool<N>
  using cpplisp::internal::Var;           // Var<N>
  using cpplisp::internal::Unit;          // Unit

  // Add
  using cpplisp::internal::Add;           // Add<E1, E2>
  // Mul
  using cpplisp::internal::Mul;           // Mul<E1, E2>
  // Mod
  using cpplisp::internal::Mod;           // Mod<E1, E2>

  // >
  using cpplisp::internal::IsGreater;     // IsGreater<E1, E2>
  // <
  using cpplisp::internal::IsLess;        // IsLess<E1, E2>
  // ==
  using cpplisp::internal::IsEqual;       // IsEqual<E1, E2>
  // IsUnit
  using cpplisp::internal::IsUnit;        // IsUnit<E>

  // Pair type
  using cpplisp::internal::Pair;          // Pair<E1, E2>
  // IsPair
  using cpplisp::internal::IsPair;        // IsPair<E>
  // Pair.1
  using cpplisp::internal::Fst;           // Fst<P>
  // Pair.2
  using cpplisp::internal::Snd;           // Snd<P>

  // List type
  using cpplisp::internal::List;          // List<E, ERest...>

  // If_Then_Else
  using cpplisp::internal::If_Then_Else;  // If_Then_Else<Cond, E1, E2>

  // ParamList
  using cpplisp::internal::ParamList;     // ParamList< Var<N1>, Var<N>... >
  // Lambda
  using cpplisp::internal::Lambda;        // Lambda< Param< Var<N1>, Var<N>... >,
                                          //         Body >

  // Call
  using cpplisp::internal::Call;          // Call< Func, Val0, ValRest... >

  // CallCC
  using cpplisp::internal::CallCC;        // CallCC< Lambda< ParamList< Var<K> >,
                                          //                 BOdy > >

}

namespace lib
{
  using namespace internal;

  // Y Combinator, lambda f. (lambda x. (f lambda y. ((x x) y))
  //                          lambda x. (f lambda y. ((x x) y)))
  typedef Lambda< ParamList< Var<0> >,
                  Call< Lambda< ParamList< Var<1> >,
                                Call< Var<0>,
                                      Lambda< ParamList< Var<2> >,
                                              Call< Call< Var<1>, Var<1> >,
                                                    Var<2> > > > >,
                        Lambda< ParamList< Var<1> >,
                                Call< Var<0>,
                                      Lambda< ParamList< Var<2> >,
                                              Call< Call< Var<1>, Var<1> >,
                                                    Var<2> > > > > > >
          YCombinater;


  // _Reduce = lambda List. lambda Op. lambda Init. List==Nil ? Init : Fst(list) Op (((_Reduce Snd(list)) Op) Init)
  // F = lambda f. lambda List. lambda Op. lambda Init. List==Nil ? Init : (Op Fst(list) (((f Snd(list)) Op) Init))
  // _Reduce = (Y F)
  typedef Call< YCombinater, Lambda< ParamList< Var<0> >, // f
                                      Lambda< ParamList< Var<1> >, // List
                                              Lambda< ParamList< Var<2> >, // Op
                                                      Lambda< ParamList< Var<3> >, // Init
                                                              If_Then_Else< IsUnit< Var<1> >,
                                                                            Var<3>,
                                                                            Call< Var<2>,
                                                                                  Fst< Var<1> >,
                                                                                  Call< Call< Call< Var<0>,
                                                                                                    Snd< Var<1> > >,
                                                                                              Var<2> >,
                                                                                        Var<3> > > > > > > > >
          _Reduce;
  // Reduce = lambda List Op Init. (((_Reduce, List) Op) Init)
  typedef Lambda< ParamList< Var<0>, Var<1>, Var<2> >,
                  Call< Call< Call< _Reduce, Var<0> >,
                              Var<1> >,
                        Var<2> > >
          Reduce;


  // _Map = lambda List. lambda Op. List==Nil ? Nil : (Pair (Op Fst(List)) ((_Map Snd(List)) Op)
  // _Map = lambda List. lambda Op. (Reduce List lambda x y. Pair((Op x) y)) Nil)
  typedef Lambda< ParamList< Var<0> >, // List
                  Lambda< ParamList< Var<1> >, //Op
                          Call< Reduce,
                                Var<0>,
                                Lambda< ParamList< Var<2>, Var<3> >,
                                        Pair< Call< Var<1>, Var<2> >,
                                              Var<3> > >,
                                Unit > > >
          _Map;
  // Map = lambda List Op. ((_Map List) Op)
  typedef Lambda< ParamList< Var<0>, Var<1> >,
                  Call< Call< _Map, Var<0> >,
                        Var<1> > >
          Map;


  // _Filter = lambda List. lambda Cond. List==Nil ? Nil :
  //                                                 (Cond Fst(List))==true ? Pair(Fst(List),
  //                                                                               ((_Filter Snd(List)) Cond)) :
  //                                                                          ((_Filter Snd(List)) Cond)
  //F = lambda f. lambda List. lambda Cond. List==Nil ? Nil :
  //                                                   (Cond Fst(List))==true ? Pair(Fst(List),
  //                                                                                 ((f Snd(List)) Cond)) :
  //                                                                          ((f Snd(List)) Cond)
  // _Filter = (Y F)
  typedef Call< YCombinater,
                Lambda< ParamList< Var<0> >, // f
                        Lambda< ParamList< Var<1> >, // List
                                Lambda< ParamList< Var<2> >, // Cond
                                        If_Then_Else< IsUnit< Var<1> >,
                                                      Unit,
                                                      If_Then_Else< Call< Var<2>, Fst< Var<1> > >,
                                                                    Pair< Fst< Var<1> >,
                                                                          Call< Call< Var<0>, Snd< Var<1> > >,
                                                                                Var<2> > >,
                                                                    Call< Call< Var<0>, Snd< Var<1> > >,
                                                                          Var<2> > > > > > > >
          _Filter;
  // Filter = lambda List Cond. ((_Filter List) Cond)
  typedef Lambda< ParamList< Var<0>, Var<1> >,
                  Call< Call< _Filter, Var<0> >,
                        Var<1> > >
          Filter;


  // _Range = lambda x. lambda y. x<y ? Nil : Pair(x ((_Range x-1) y))
  // F = lambda f. lambda x. lambda y. x>y ? Nil : Pair(x ((f x+1) y))
  // _Range = (Y F)
  typedef Call< YCombinater, Lambda< ParamList< Var<0> >, // f
                                      Lambda< ParamList< Var<1> >, // x
                                              Lambda< ParamList< Var<2> >, // y
                                                      If_Then_Else< IsGreater< Var<1>, Var<2> >,
                                                                    Unit,
                                                                    Pair< Var<1>,
                                                                          Call< Call< Var<0>,
                                                                                      Add< Var<1>, Int<1> > >,
                                                                                Var<2> > > > > > > >
          _Range;
  // Range = lambda x y. ((_Range x) y)
  typedef Lambda< ParamList< Var<0>, Var<1> >,
                  Call< Call< _Range, Var<0> >,
                        Var<1> > >
          Range;


  // Sum = lambda List. (((Reduce List) lambda x y. Add(x, y)) 0)
  typedef Lambda< ParamList< Var<0> >, // List
                  Call< Reduce,
                        Var<0>,
                        Lambda< ParamList< Var<2>, Var<1> >,
                                Add< Var<2>, Var<1> > >,
                        Int<0> > >
          Sum;


  // IsList = lambda L. IsPair(L) ? IsList(Snd(L)) : IsUnit(L)
  // F = lambda f. lambda L. IsPair(L) ? f(Snd(L)) : IsUnit(L)
  // IsList = (Y F)
  typedef Call< YCombinater, Lambda< ParamList< Var<0> >, // f
                                      Lambda< ParamList< Var<1> >, // L
                                              If_Then_Else< IsPair< Var<1> >,
                                                            Call< Var<0>, Snd< Var<1> > >,
                                                            IsUnit< Var<1> > > > > >
          IsList;


  // _ListRef = lambda L. lambda N. N==0 ? Fst(L) : ((_ListRef Snd(L)) N-1)
  // F = lambda f. lambda L. lambda N. N==0 ? Fst(L) : ((f Snd(L)) N-1)
  // _ListRef = (Y F)
  typedef Call< YCombinater, Lambda< ParamList< Var<0> >, // f
                                      Lambda< ParamList< Var<1> >, // L
                                              Lambda< ParamList< Var<2> >, // N
                                                      If_Then_Else< IsEqual< Var<2>, Int<0> >,
                                                                    Fst< Var<1> >,
                                                                    Call< Call< Var<0>,
                                                                                Snd< Var<1> > >,
                                                                          Add< Var<2>, Int<-1> > > > > > > >
          _ListRef;
  // ListRef = lambda L N. ((_ListRef L) N)
  typedef Lambda< ParamList< Var<0>, Var<1> >,
                  Call< Call< _ListRef, Var<0> >,
                        Var<1> > >
          ListRef;


  // _ListAppend = lambda L. lambda E. IsUnit(L) ? Pair<E, Unit> : Pair< Fst(L), ((_ListAppend Snd(L)) E) >
  // F = lambda f. lambda L. lambda E. IsUnit(L) ? Pair<E, Unit> : Pair< Fst(L), ((f Snd(L)) E) >
  // _ListAppend = (Y F)
  typedef Call< YCombinater, Lambda< ParamList< Var<0> >, // f
                                      Lambda< ParamList< Var<1> >, // L
                                              Lambda< ParamList< Var<2> >, // E
                                                      If_Then_Else< IsUnit< Var<1> >,
                                                                    Pair< Var<2>, Unit >,
                                                                    Pair< Fst< Var<1> >,
                                                                          Call< Call< Var<0>,
                                                                                      Snd< Var<1> > >,
                                                                                Var<2> > > > > > > >
          _ListAppend;
  // ListAppend = lambda L E. ((_ListAppend L) E)
  typedef Lambda< ParamList< Var<0>, Var<1> >,
                  Call< Call< _ListAppend, Var<0> >,
                        Var<1> > >
          ListAppend;

} // namespace lib


} // namespace cpplisp

#endif