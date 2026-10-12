#include <gtest/gtest.h>
#include <cstring>

#include "fn_ref.hpp"


namespace {
    double add(double a, double b) {
        return a + b;
    }


    TEST(memtest, FreeFunctionSizePreservation) {

        /* @test FunctionPointerSizePreservation: checks size
           preservation from free function pointer. */

        using fptr_t = double (*)(double, double);
        FnRef<fptr_t> f(nullptr);

        ASSERT_EQ(sizeof(f), sizeof(fptr_t));
    }

    TEST(memtest, NonConstMemberFunctionSizePreservation) {

        /* @test MemberFunctionPointerZero...: checks size
           preservation from non-const member function pointers. */

        struct foo {
            int mmf(int a, int b) {
                return a + b;
            }
        };

        using mmfptr_t = int(foo::*)(int, int);
        FnRef<mmfptr_t> f(nullptr);

        ASSERT_EQ(sizeof(f), sizeof(mmfptr_t)); 
    }

    TEST(memtest, ConstMemberFunctionSizePreservation) {

        /* @test ConstMemberFunctionZero...: verifies size
           preservation from const member function pointers. */

        struct foo {
            int mmf(int a, int b) const {
                return a + b;
            }
        };

        using mmfptr_t = int(foo::*)(int, int) const;
        FnRef<mmfptr_t> f(nullptr);

        ASSERT_EQ(sizeof(f), sizeof(mmfptr_t)); 
    }

    TEST(memtest, CapturelessLambdaSizePreservation) {

        /* @test CapturelessLambdaSizePreservation: checks size
           preservation from captureless lambdas. */

        auto foo = [](int a, int b) -> int {
            return a + b;
        };

        using fnobj_t = decltype(foo);
        FnRef<fnobj_t> f(&foo);

        ASSERT_EQ(sizeof(f), sizeof(fnobj_t*));
    }


    TEST(memtest, StatefulLambdaSizePreservation) {

        /* @test StatefulLambdaSizePreservation: checks size 
           match for capturing lambda objects. */

        int factor = 2;
        auto foo = [=](int a, int b) -> int {
            return factor * (a + b);
        };

        using fnobj_t = decltype(foo);
        FnRef<fnobj_t> f(&foo);

        ASSERT_EQ(sizeof(f), sizeof(fnobj_t*));
    }


    TEST(memtest, FunctorSizePreservation) {

        /* @test FunctorSizePreservation: verifies size 
           preservation for true functor calls. */

        struct foo {
            int operator()(int a, int b) {
                return a + b;
            }
        };

        foo inst{};
        FnRef<foo> f(&inst);

        ASSERT_EQ(sizeof(f), sizeof(foo*));
    }


    TEST(memtest, FreeFunctionMemoryInvariance) {

        /* @test FunctionPointerMemoryInvariance: veryfies bitwise 
           identity preservation from free function pointer.  */

        using fptr_t = double (*)(double, double);
        fptr_t ffptr = &add;                        
        FnRef f(ffptr);                

        unsigned char faddr[sizeof(f)];
        std::memcpy(faddr, &f, sizeof(f));

        unsigned char ffaddr[sizeof(fptr_t)];
        std::memcpy(ffaddr, &ffptr, sizeof(fptr_t));

        ASSERT_EQ(std::memcmp(faddr, ffaddr, sizeof(fptr_t)), 0);
    }

    TEST(memtest, NonConstMemberFunctionMemoryInvariance) {

        /* @test NonConst...: checks bitwise identity preservation 
           from non-const member function pointers. */

        struct foo {
            int mmf(int a, int b) {
                return a + b;
            }
        };

        using mmfptr_t = int(foo::*)(int, int);
        mmfptr_t mmfptr = &foo::mmf;
        FnRef f(mmfptr);

        unsigned char faddr[sizeof(f)];
        std::memcpy(faddr, &f, sizeof(f));

        unsigned char mmfaddr[sizeof(mmfptr_t)];
        std::memcpy(mmfaddr, &mmfptr, sizeof(mmfptr_t));

        ASSERT_EQ(std::memcmp(faddr, mmfaddr, sizeof(mmfptr_t)), 0);
    }


    TEST(memtest, ConstMemberFunctionMemoryInvariance) {

        /* @test ConstMember...: checks bitwise identity 
           preservation from const member function pointers. */

        struct foo {
            int mmf(int a, int b) const {
                return a + b;
            }
        };

        using mmfptr_t = int(foo::*)(int, int) const;
        mmfptr_t mmfptr = &foo::mmf;
        FnRef f(mmfptr);

        unsigned char faddr[sizeof(f)];
        std::memcpy(faddr, &f, sizeof(f));

        unsigned char mmfaddr[sizeof(mmfptr_t)];
        std::memcpy(mmfaddr, &mmfptr, sizeof(mmfptr_t));

        ASSERT_EQ(std::memcmp(faddr, mmfaddr, sizeof(mmfptr_t)), 0);
    }


    TEST(memtest, CapturelessLambdaMemoryInvariance) {

        /* @test CapturelessLambda...: checks bitwise identity 
           preservation from captureless lambdas. */

        auto foo = [](int a, int b) -> int {
            return a + b;
        };

        using fnobj_t = decltype(foo);
        fnobj_t* fnobj = &foo;
        FnRef f(fnobj);

        unsigned char faddr[sizeof(f)];
        std::memcpy(faddr, &f, sizeof(f));

        unsigned char fnaddr[sizeof(fnobj_t*)];
        std::memcpy(fnaddr, &fnobj, sizeof(fnobj_t*));

        ASSERT_EQ(std::memcmp(faddr, fnaddr, sizeof(fnobj_t*)), 0);
    }

    TEST(memtest, StatefulLambdaMemoryInvariance) {

        /* @test StatefulLambda...: checks bitwise identity 
           preservation from capturing lambdas. */

        int factor = 2;
        auto foo = [=](int a, int b) -> int {
            return factor * (a + b);
        };

        using fnobj_t = decltype(foo);
        fnobj_t* fnobj = &foo;
        FnRef f(fnobj);

        unsigned char faddr[sizeof(f)];
        std::memcpy(faddr, &f, sizeof(f));

        unsigned char fnaddr[sizeof(fnobj_t*)];
        std::memcpy(fnaddr, &fnobj, sizeof(fnobj_t*));

        ASSERT_EQ(std::memcmp(faddr, fnaddr, sizeof(fnobj_t*)), 0);
    }


    TEST(memtest, FunctorMemoryInvariance) {

        /* @test FunctorMemoryInvariance: checks bitwise 
           identity preservation from functor objects. */

        class foo {
        public:
            double operator()(double a) {
                return a * a;
            }
        };

        foo inst{};
        foo* fnobj = &inst;
        FnRef f(fnobj);

        unsigned char faddr[sizeof(f)];
        std::memcpy(faddr, &f, sizeof(f));

        unsigned char fnaddr[sizeof(foo*)];
        std::memcpy(fnaddr, &fnobj, sizeof(foo*));

        ASSERT_EQ(std::memcmp(faddr, fnaddr, sizeof(foo*)), 0);
    }
}
