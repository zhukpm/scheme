#pragma once

#include <scheme.h>
#include <catch.hpp>

struct SchemeTest {
    SchemeTest() {
        engine_.Init();
    }

    void ExpectEq(std::string expression, std::string result) {
        REQUIRE(engine_.Interpret(expression) == result);
    }

    void ExpectNoError(std::string expression) {
        engine_.Interpret(expression);
    }

    void ExpectSyntaxError(std::string expression) {
        REQUIRE_THROWS_AS(engine_.Interpret(expression), SyntaxError);
    }

    void ExpectRuntimeError(std::string expression) {
        REQUIRE_THROWS_AS(engine_.Interpret(expression), RuntimeError);
    }

    void ExpectNameError(std::string expression) {
        REQUIRE_THROWS_AS(engine_.Interpret(expression), NameError);
    }

private:
    SchemeEngine engine_;
};
