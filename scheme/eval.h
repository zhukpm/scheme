#pragma once

#include <memory>

#include <error.h>
#include <object.h>
#include <scope.h>
#include <function.h>

inline std::shared_ptr<Object> EvalSymbol(const std::shared_ptr<Symbol>& expression,
                                          const std::shared_ptr<Scope>& scope) {
    if (scope->HasVariable(expression->GetName())) {
        return scope->GetVariable(expression->GetName());
    }
    throw NameError{"No variable with name '" + expression->GetName() + "' is defined"};
}

inline std::shared_ptr<Object> EvalInstruction(std::shared_ptr<Cell> expression,
                                               std::shared_ptr<Scope> scope) {
    auto objects = expression->ToVector();
    auto function = Eval(objects[0], scope);
    if (Is<Function>(function)) {
        objects.pop_front();
        if (!As<Function>(function)->ApprovesNumberOfArguments(objects)) {
            if (Is<SpecialFormFunction>(function)) {
                throw SyntaxError{"Wrong number of arguments (" + std::to_string(objects.size()) +
                                  ") for Function " + function->Repr()};
            } else {
                throw RuntimeError{"Wrong number of arguments (" + std::to_string(objects.size()) +
                                   ") for Function " + function->Repr()};
            }
        }
        return As<Function>(function)->Apply(objects, scope);
    }
    throw RuntimeError{"First element of '" + expression->Repr() + "' has to be a Function"};
}

inline std::shared_ptr<Object> Eval(std::shared_ptr<Object> expression,
                                    std::shared_ptr<Scope> scope) {
    if (expression == nullptr || expression == kSchemeNull) {
        throw RuntimeError{"Can not evaluate Null"};
    }
    if (Is<Number>(expression) || Is<Boolean>(expression)) {
        return expression;
    }
    if (Is<Symbol>(expression)) {
        return EvalSymbol(As<Symbol>(expression), scope);
    }
    if (Is<Cell>(expression)) {
        return EvalInstruction(As<Cell>(expression), scope);
    }
    throw SyntaxError{"How come???? Lol"};
}
