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

namespace cpplisp {

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