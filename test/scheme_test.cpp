#include "scheme_test.h"

TEST_CASE_METHOD(SchemeTest, "CarCdr") {
    ExpectNoError("(define x '())");
    ExpectEq("x", "()");

    ExpectEq("(car '(1))", "1");
    ExpectEq("(cdr '(1))", "()");

    ExpectRuntimeError("(car x)");
    ExpectRuntimeError("(cdr x)");
}

TEST_CASE_METHOD(SchemeTest, "Lambdas") {
    ExpectNoError(R"EOF(
        (define range (lambda (x) (lambda () (set! x (+ x 1)) x)))
                    )EOF");
    ExpectNoError("(define my-range10 (range 10))");
    ExpectEq("(my-range10)", "11");
    ExpectEq("(my-range10)", "12");
    ExpectEq("(my-range10)", "13");

    ExpectNoError("(define my-range1 (range 1))");
    ExpectEq("(my-range1)", "2");
    ExpectEq("(my-range1)", "3");

    ExpectEq("(my-range10)", "14");
}