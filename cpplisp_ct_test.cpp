#include "cpplisp_ct.h"
#include <iostream>
#include <string>

using namespace std;

int main(int, char**){

  {
    using namespace cpplisp::lib;
    using namespace cpplisp::internal;
    using namespace cpplisp::util;
    using namespace cpplisp;
    StaticCheckEQ< Eval< Int<4> >::value, Int<4> >();
    StaticCheckEQ< Eval< Bool<true> >::value, Bool<true> >();
    StaticCheckEQ< Eval< Unit >::value, Unit >();


    StaticCheckEQ< Eval< Add< Add< Int<1>, Int<3> >, Int<4> > >::value, Int<8> >();
    StaticCheckEQ< Eval< Add< Int<-9>, Int<45> > >::value, Int<36> >();

    StaticCheckEQ< Eval< Mul< Mul< Int<1>, Int<3> >, Int<4> > >::value, Int<12> >();
    StaticCheckEQ< Eval< Mul< Add< Int<-7>, Int<-2> >, Add< Int<3>, Int<3> > > >::value, Int<-54> >();

    StaticCheckEQ< Eval< Mod< Mul< Int<1>, Int<3> >, Int<4> > >::value, Int<3> >();
    StaticCheckEQ< Eval< Mod< Int<9>, Int<6> > >::value, Int<3> >();


    StaticCheckEQ< Eval< IsGreater< Add< Int<1>, Int<4> >, Int<8> > >::value, Bool<false> >();
    StaticCheckEQ< Eval< IsLess< Add< Int<1>, Int<4> >, Int<8> > >::value, Bool<true> >();
    StaticCheckEQ< Eval< IsEqual< Add< Int<4>, Int<4> >, Int<8> > >::value, Bool<true> >();

    StaticCheckEQ< Eval< IsUnit<Unit> >::value, Bool<true> >();
    StaticCheckEQ< Eval< IsUnit< Add< Int<2>, Int<2> > > >::value, Bool<false> >();
    StaticCheckEQ< Eval< IsUnit< List< Int<9>, Int<2> > > >::value, Bool<false> >();

    typedef Pair< Pair< Int<4>, Bool<true> >, Pair< Pair<Int<2>, Unit>, Bool<false> > > P;
    StaticCheckEQ< Eval<P>::value, P >();
    StaticCheckEQ< Eval< IsPair<P> >::value, Bool<true> >();
    StaticCheckEQ< Eval< IsPair< Add< Int<1>, Int<2> > > >::value, Bool<false> >();
    StaticCheckEQ< Eval< Fst< Fst<P> > >::value, Int<4> >();
    StaticCheckEQ< Eval< Snd< Fst< Snd<P> > > >::value, Unit >();
    StaticCheckEQ< Eval< Snd< Snd<P> > >::value, Bool<false> >();

    typedef List< List< Int<2>, Bool<false> >,
                  List< Int<4>, Bool<true> >,
                  List< Int<6> > >
            L1;
    typedef Pair< Pair< Int<2>, Pair<Bool<false>, Unit> >,
                  Pair< Pair< Int<4>, Pair<Bool<true>, Unit> >,
                        Pair< Pair<Int<6>, Unit>,
                              Unit > > >
            P1;
    StaticCheckEQ< Eval<L1>::value, Eval<P1>::value >();
    StaticCheckEQ< Eval< List< Int<5> > >::value, Pair< Int<5>, Unit > >();


    StaticCheckEQ< Eval< Call< lib::IsList, L1 > >::value, Bool<true> >();
    StaticCheckEQ< Eval< Call< lib::IsList, P1 > >::value, Bool<true> >();
    StaticCheckEQ< Eval< Call< lib::IsList, Unit > >::value, Bool<true> >();
    StaticCheckEQ< Eval< Call< lib::IsList, Snd< Snd< P1> > > >::value, Bool<true> >();
    StaticCheckEQ< Eval< Call< lib::IsList, P > >::value, Bool<false> >();


    typedef List< Int<0>, Int<1>, Int<2>, Int<3>, Int<4> > L3;
    StaticCheckEQ< Eval< Call< lib::ListRef, L3, Add< Int<2>, Int<0> > > >::value, Int<2> >();
    StaticCheckEQ< Eval< Call< lib::ListRef,
                               Call< lib::ListRef,
                                     L1, Add< Int<0>, Int<0> > >,
                               Add< Int<-2>, Int<2> > > >::value,
                   Int<2> >();


    typedef List< Int<0>, Int<1>, Int<2>, Int<3>, Int<4>, Int<5> > L4;
    StaticCheckEQ< Eval< Call< lib::ListAppend, L3, Int<5> > >::value, Eval<L4>::value >();
    StaticCheckEQ< Eval< Call< lib::ListAppend,
                               Call< lib::ListAppend, L3, Add< Int<1>, Int<4> > >,
                               List< Int<9> > > >::value,
                   Eval< Call< lib::ListAppend, L4, List< Int<9> > > >::value >();


    StaticCheckEQ< Eval< If_Then_Else< IsGreater< Int<5>, Int<8> >,
                                       L1,
                                       P > >::value,
                    P >();
    StaticCheckEQ< Eval< If_Then_Else< IsLess< Int<5>, Int<8> >,
                                       L1,
                                       P> >::value,
                   Eval<L1>::value >();


    typedef VarValList< Var<0>, Int<0>,
                        VarValList< Var<1>, Int<1>, EmptyVarValList > >
            VarValL0;
    typedef VarValList< Var<2>, Int<2>,
                        VarValList< Var<0>, Int<0>,
                                    VarValList< Var<1>, Int<1>,
                                                EmptyVarValList > > >
            VarValL1;

    StaticCheckEQ< VarValListExtend< Var<2>, Int<2>, VarValL0 >::value,
                   VarValL1 >();


    StaticCheckEQ< VarValListLookup<Var<2>, VarValL1>::value, Int<2> >();
    StaticCheckEQ< VarValListLookup<Var<0>, VarValL1>::value, Int<0> >();
    StaticCheckEQ< VarValListLookup<Var<1>, VarValL1>::value, Int<1> >();
    StaticCheckEQ< VarValListLookup<Var<3>, VarValL1>::value, EmptyType >();

    typedef VarValList< Var<8>, Int<8>,
                        VarValList< Var<9>, Int<9>, EmptyVarValList > >
            VarValL2;
    typedef VarValList< Var<5>, Int<5>,
                        VarValList< Var<6>, Int<6>,
                                    VarValList< Var<7>, Int<7>,
                                                EmptyVarValList > > >
            VarValL3;

    typedef Env< VarValL2,
                 Env< VarValL3, EmptyEnv > >
            E0;
    typedef Env< VarValL0,
                 Env< VarValL1,
                      Env< VarValL2,
                           Env< VarValL3, EmptyEnv > > > >
            E1;


    StaticCheckEQ< EnvExtend<VarValL0, EnvExtend<VarValL1, E0>::value>::value, E1 >();


    StaticCheckEQ< EnvLookup<Var<0>, E1>::value, Int<0> >();
    StaticCheckEQ< EnvLookup<Var<0>, E0>::value, EmptyType >();
    StaticCheckEQ< EnvLookup<Var<8>, E1>::value, Int<8> >();
    StaticCheckEQ< EnvLookup<Var<7>, E1>::value, Int<7> >();
    StaticCheckEQ< EnvLookup<Var<6>, E0>::value, Int<6> >();


    typedef Lambda< ParamList< Var<0>, Var<1> >,
                    Add< Var<0>, Var<1> > >
            Plus;
    StaticCheckEQ< Eval<Plus>::value, Closure<EmptyEnv, Plus> >();


    StaticCheckEQ< Eval< Call< Plus, Int<2>, Int<1> > >::value,
                   Int<3> >();
    StaticCheckEQ< Eval< Call< Lambda< ParamList< Var<0> >, Var<0> >,
                               Int<1> > >::value,
                   Int<1> >();
    StaticCheckEQ< Eval< Call< If_Then_Else< Bool<true>,
                                             Plus,
                                             Int<1> >,
                               Int<2>, Int<1> > >::value,
                   Int<3> >();


    // lexcal binding, lambda y.(lambda x.(lambda y.x+y 3) y) 7
    StaticCheckEQ< Eval< Call< Lambda< ParamList< Var<0> >,
                                       Call< Lambda< ParamList< Var<1> >,
                                                     Call< Lambda< ParamList< Var<0> >,
                                                                   Add< Var<0>, Var<1> > >,
                                                           Int<3> > >,
                                             Var<0> > >,
                               Int<7> > >::value,
                   Int<10> >();

    // syntatic closure
    StaticCheckEQ< Eval< Call< Call< Lambda< ParamList< Var<0> >,
                                             Lambda< ParamList< Var<1> >,
                                                     Add< Var<0>, Var<1> > > >,
                                     Int<3> >,
                               Int<4> > >::value,
                   Int<7> >();



    // anonymous recursion, Factorial, F = lambda f. lambda n. n==0 ? 1 : n*( (f f) (n-1) )
    // ( (F F) n ) = n!
    StaticCheckEQ< Eval< Call< Call< Lambda< ParamList< Var<1> >, // F
                                             Lambda< ParamList< Var<0> >,
                                                     If_Then_Else< IsEqual< Var<0>, Int<0> >,
                                                                   Int<1>,
                                                                   Mul< Var<0>,
                                                                        Call< Call< Var<1>, Var<1> >,
                                                                              Add< Var<0>, Int<-1> > > > > > >,
                                     Lambda< ParamList< Var<1> >, // F
                                             Lambda< ParamList< Var<0> >,
                                                     If_Then_Else< IsEqual< Var<0>, Int<0> >,
                                                                   Int<1>,
                                                                   Mul< Var<0>,
                                                                        Call< Call< Var<1>, Var<1> >,
                                                                              Add< Var<0>, Int<-1> > > > > > > >,
                               Int<5> > >::value,
                   Int<120> >();


    // --------------------------------------------------------------------------------------------
    // Y Combinator, lambda f. (lambda x. (f lambda y. ((x x) y))
    //                          lambda x. (f lambda y. ((x x) y)))
    typedef Lambda< ParamList< Var<0> >, // Y Combinator
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




    // F = lambda f. lambda n. n==0 ? 1 : n*(f n-1)
    // (Fact 5) = ((Y F) 5)
    StaticCheckEQ< Eval< Call<
                             Call<YCombinater, // Fact = (Y F)
                                  Lambda< ParamList< Var<0> >, // F
                                          Lambda< ParamList< Var<1> >,
                                                  If_Then_Else< IsEqual< Var<1>, Int<0> >,
                                                                Int<1>,
                                                                Mul< Var<1>,
                                                                     Call< Var<0>,
                                                                           Add< Var<1>, Int<-1> > > > > > > >,
                             Int<5> > >::value, // (Fact 5)
                   Int<120> >();


    // Reduce = lambda List. lambda Op. lambda Init. List==Nil ? Init : Fst(list) Op (((Reduce Snd(list)) Op) Init)

    // F = lambda f. lambda List. lambda Op. lambda Init. List==Nil ? Init : (Op Fst(list) (((f Snd(list)) Op) Init))
    typedef Lambda< ParamList< Var<0> >, // f
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
                                                                      Var<3> > > > > > > >
            F;
    // Reduce = (Y F)
    typedef Call<YCombinater, F> Reduce;

    // Sum = lambda List. (((Reduce List) lambda x y. Add(x, y)) 0)
    typedef Lambda< ParamList< Var<0> >, // List
                    Call< Call< Call< Reduce, Var<0> >,
                                Lambda< ParamList< Var<2>, Var<1> >,
                                        Add< Var<2>, Var<1> > > >,
                          Int<0> > >
        Sum;
    StaticCheckEQ< Eval< Call< Sum, List< Int<1>, Int<2>, Int<3> > > >::value, Int<6> >();


    //----------------------------------------------------------------------------------------------------------
    // call/cc
    StaticCheckEQ< Eval< CallCC< Lambda< ParamList< Var<0> >, Int<1> > > >::value, Int<1> >();
    StaticCheckEQ< Eval< CallCC< Lambda< ParamList< Var<0> >, Call< Var<0>, Int<1> > > > >::value, Int<1> >();
    StaticCheckEQ< Eval< Add< Int<1>,
                              CallCC< Lambda< ParamList< Var<0> >,
                                              Add< Int<2>, Int<3> > > > > >::value,
                   Int<6> >();
    StaticCheckEQ< Eval< Add< Int<1>,
                              CallCC< Lambda< ParamList< Var<0> >,
                                              Add< Int<2>,
                                                   Call< Var<0>, Int<3> > > > > > >::value,
                   Int<4> >();
    // (((call/cc (lambda (k) k)) (lambda (k) k)) 3) = 3
    StaticCheckEQ< Eval< Call< Call< CallCC< Lambda< ParamList< Var<0> >,
                                                     Var<0> > >,
                                     Lambda< ParamList< Var<0> >,
                                             Var<0> > >,
                               Int<3> > >::value,
                   Int<3> >();
    // ((lambda (k) (k (lambda (k) 2))) (call/cc (lambda (k) k))) = 2
    StaticCheckEQ< Eval< Call< Lambda< ParamList< Var<0> >,
                                       Call< Var<0>,
                                             Lambda< ParamList< Var<0> >,
                                                     Int<2> > > >,
                               CallCC< Lambda< ParamList< Var<0> >,
                                               Var<0> > > > >::value,
                   Int<2> >();
  } // compile_time test



  {
    using namespace cpplisp::lib;
    using namespace cpplisp::internal;
    using namespace cpplisp;
    cout<<endl;
    cout<<"4 + 5 = ";
    cout<< Eval< Add< Int<4>, Int<5> > >::value::value;
    cout<<endl;
    cout<<"4 * 5 = ";
    cout<< Eval< Mul< Int<4>, Int<5> > >::value::value;
    cout<<endl;
    cout<<"4 % 5 = ";
    cout<< Eval< Mod< Int<4>, Int<5> > >::value::value;
    cout<<endl;

    cout<<endl;
    cout<<"4 > 5 : ";
    cout<< Eval< IsGreater< Int<4>, Int<5> > >::value::value;
    cout<<endl;
    cout<<"4 < 5 : ";
    cout<< Eval< IsLess< Int<4>, Int<5> > >::value::value;
    cout<<endl;
    cout<<"4 = 4 : ";
    cout<< Eval< IsEqual< Int<4>, Int<4> > >::value::value;
    cout<<endl;

    cout<<endl;
    cout<<"First  of Pair<4, 5> : ";
    cout<< Eval< Fst< Pair< Int<4>, Int<5> > > >::value::value;
    cout<<endl;
    cout<<"Second of Pair<4, 5> : ";
    cout<< Eval< Snd< Pair< Int<4>, Int<5> > > >::value::value;
    cout<<endl;

    cout<<endl;
    cout<<"Is Pair<4, 5> a List?                                   : ";
    cout<< Eval< Call< lib::IsList, Pair< Int<4>, Int<5> > > >::value::value;
    cout<<endl;
    cout<<"Is Pair< 4, Pair<5, Unit> > a List?                     : ";
    cout<< Eval< Call< lib::IsList, List< Int<4>, Int<5> > > >::value::value;
    cout<<endl;
    cout<<"The num at index 2 of List<0, 1, 2, 3>                  : ";
    cout<< Eval< Call< lib::ListRef, List< Int<0>, Int<1>, Int<2>, Int<3> >, Int<2> > >::value::value;
    cout<<endl;
    cout<<"The num at index 4 of ListAppend< List<0, 1, 2, 3>, 9 > : ";
    cout<< Eval< Call< lib::ListRef,
                       Call< lib::ListAppend,
                             List< Int<0>, Int<1>, Int<2>, Int<3> >, Int<9> >,
                       Int<4> > >::value::value;
    cout<<endl;

  }

  return 0;
}