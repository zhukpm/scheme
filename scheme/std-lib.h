#pragma once

#include <deque>
#include <algorithm>

#include <error.h>
#include <object.h>
#include <scope.h>
#include <function.h>

inline std::shared_ptr<Number> RequireNumberArgument(std::shared_ptr<Object> object) {
    auto element = As<Number>(object);
    if (!element) {
        throw RuntimeError{"A Number is expected, got " + object->Repr()};
    }
    return element;
}

inline std::shared_ptr<Symbol> RequireSymbolArgument(std::shared_ptr<Object> object) {
    auto element = As<Symbol>(object);
    if (!element) {
        throw RuntimeError{"A Symbol is expected, got " + object->Repr()};
    }
    return element;
}

inline std::shared_ptr<Cell> RequirePairArgument(std::shared_ptr<Object> object) {
    auto element = As<Cell>(object);
    if (!element) {
        throw RuntimeError{"A Pair is expected, got " + object->Repr()};
    }
    return element;
}

class QuoteFunction : public Function {
public:
    QuoteFunction() : Function("quote") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (!arguments[0]) {
            return kSchemeNull;
        }
        return arguments[0];
    }
};

class MaxFunction : public Function {
public:
    MaxFunction() : Function("max") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return !arguments.empty();
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto it = arguments.begin();
        auto end = arguments.end();
        auto element = RequireNumberArgument(Eval(*it, scope));
        int max = element->GetValue();
        while (++it < end) {
            element = RequireNumberArgument(Eval(*it, scope));
            max = std::max(max, element->GetValue());
        }
        return std::make_shared<Number>(max);
    }
};

class MinFunction : public Function {
public:
    MinFunction() : Function("min") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return !arguments.empty();
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto it = arguments.begin();
        auto end = arguments.end();
        auto element = RequireNumberArgument(Eval(*it, scope));
        int min = element->GetValue();
        while (++it < end) {
            element = RequireNumberArgument(Eval(*it, scope));
            min = std::min(min, element->GetValue());
        }
        return std::make_shared<Number>(min);
    }
};

class AbsFunction : public Function {
public:
    AbsFunction() : Function("abs") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto element = RequireNumberArgument(Eval(arguments[0], scope));
        return std::make_shared<Number>(std::abs(element->GetValue()));
    }
};

class LambdaFunction : public SpecialFormFunction {
public:
    LambdaFunction() : SpecialFormFunction("lambda") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() > 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto bindings = arguments[0];
        arguments.pop_front();
        std::deque<std::string> parameters;
        if (Is<Cell>(bindings)) {
            for (const auto& object: As<Cell>(bindings)->ToVector()) {
                parameters.push_back(RequireSymbolArgument(object)->GetName());
            }
        } else if (Is<Symbol>(bindings)) {
            parameters.push_back(As<Symbol>(bindings)->GetName());
        } else {
            throw SyntaxError{"Symbol or a sequence of Symbols is expected in lambda, got " +
                              bindings->Repr()};
        }
        return std::make_shared<CustomLambdaFunction>(scope, std::move(parameters),
                                                      std::move(arguments));
    }
};

class ConsFunction : public Function {
public:
    ConsFunction() : Function("cons") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 2;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        return std::make_shared<Cell>(Eval(arguments[0], scope), Eval(arguments[1], scope));
    }
};

class CarFunction : public Function {
public:
    CarFunction() : Function("car") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto elem = RequirePairArgument(Eval(arguments[0], scope));
        if (!elem->GetFirst()) {
            throw RuntimeError{"Couldn't take head of an empty list: " + elem->Repr()};
        }
        return elem->GetFirst();
    }
};

class CdrFunction : public Function {
public:
    CdrFunction() : Function("cdr") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto elem = RequirePairArgument(Eval(arguments[0], scope));
        if (!elem->GetFirst()) {
            throw RuntimeError{"Couldn't take tail of an empty list: " + elem->Repr()};
        }
        if (!elem->GetSecond()) {
            return kSchemeNull;
        }
        return elem->GetSecond();
    }
};

class ListFunction : public Function {
public:
    ListFunction() : Function("list") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return true;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            return kSchemeNull;
        }
        return BuildList(arguments, scope);
    }

private:
    std::shared_ptr<Object> BuildList(std::deque<std::shared_ptr<Object>>& arguments,
                                      std::shared_ptr<Scope> scope) {
        if (arguments.empty()) {
            return nullptr;
        }
        auto head = Eval(arguments[0], scope);
        arguments.pop_front();
        return std::make_shared<Cell>(head, BuildList(arguments, scope));
    }
};

class ListRefFunction : public Function {
public:
    ListRefFunction() : Function("list-ref") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 2;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto pair = RequirePairArgument(Eval(arguments[0], scope));
        auto index = RequireNumberArgument(Eval(arguments[1], scope));
        auto list = pair->ToVector();
        if (static_cast<int64_t>(index->GetValue()) >= static_cast<int64_t>(list.size())) {
            throw RuntimeError{"List index out of range: " + pair->Repr() + ", " + index->Repr()};
        }
        return list[index->GetValue()];
    }
};

class ListTailFunction : public Function {
public:
    ListTailFunction() : Function("list-tail") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 2;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto pair = RequirePairArgument(Eval(arguments[0], scope));
        auto index = RequireNumberArgument(Eval(arguments[1], scope))->GetValue();
        auto list = pair->ToVector();
        if (static_cast<int64_t>(index) > static_cast<int64_t>(list.size())) {
            throw RuntimeError{"List tail index out of range: " + pair->Repr() + ", " +
                               std::to_string(index)};
        }
        std::shared_ptr<Object> tail = pair;
        while (index > 0 && Is<Cell>(tail)) {
            tail = As<Cell>(tail)->GetSecond();
            --index;
        }
        if (!tail) {
            return kSchemeNull;
        }
        return tail;
    }
};

class DefineFunction : public SpecialFormFunction {
public:
    DefineFunction() : SpecialFormFunction("define") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 2;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (Is<Cell>(arguments[0])) {
            HandleLambdaSugar(arguments, scope);
        } else {
            auto element = RequireSymbolArgument(arguments[0]);
            scope->DefineVariable(element->GetName(), Eval(arguments[1], scope));
        }
        return kSchemeNull;
    }

private:
    void HandleLambdaSugar(std::deque<std::shared_ptr<Object>>& arguments,
                           std::shared_ptr<Scope> scope) {
        auto lambda_signature = As<Cell>(arguments[0]);
        if (!lambda_signature->GetFirst()) {
            throw SyntaxError{"Symbol is expected as the first element in the list, got " +
                              arguments[0]->Repr()};
        }
        auto lambda_name = RequireSymbolArgument(lambda_signature->GetFirst())->GetName();
        if (lambda_signature->GetSecond()) {
            arguments[0] = lambda_signature->GetSecond();
        } else {
            arguments[0] = kSchemeNull;
        }
        scope->DefineVariable(lambda_name,
                              As<Function>(scope->GetVariable("lambda"))->Apply(arguments, scope));
    }
};

class SetFunction : public SpecialFormFunction {
public:
    SetFunction() : SpecialFormFunction("set!") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 2;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto element = RequireSymbolArgument(arguments[0]);
        auto var_name = element->GetName();
        if (!scope->HasVariable(var_name)) {
            throw NameError{"Variable '" + var_name + "' is not defined in the current scope"};
        }
        scope->SetVariable(var_name, Eval(arguments[1], scope));
        return kSchemeNull;
    }
};

class SetCarFunction : public SpecialFormFunction {
public:
    SetCarFunction() : SpecialFormFunction("set-car!") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 2;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto element = RequirePairArgument(Eval(arguments[0], scope));
        element->SetFirst(Eval(arguments[1], scope));
        return kSchemeNull;
    }
};

class SetCdrFunction : public SpecialFormFunction {
public:
    SetCdrFunction() : SpecialFormFunction("set-cdr!") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 2;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto element = RequirePairArgument(Eval(arguments[0], scope));
        element->SetSecond(Eval(arguments[1], scope));
        return kSchemeNull;
    }
};

class PlusOperator : public Function {
public:
    PlusOperator() : Function("+") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return true;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        int sum = 0;
        for (auto& arg: arguments) {
            auto number = RequireNumberArgument(Eval(arg, scope));
            sum += number->GetValue();
        }
        return std::make_shared<Number>(sum);
    }
};

class MinusOperator : public Function {
public:
    MinusOperator() : Function("-") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return !arguments.empty();
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto it = arguments.begin();
        auto end = arguments.end();
        auto element = RequireNumberArgument(Eval(*it, scope));
        int result = element->GetValue();
        if (++it == end) {
            return std::make_shared<Number>(-result);
        }
        while (it < end) {
            element = RequireNumberArgument(Eval(*it, scope));
            result -= element->GetValue();
            ++it;
        }
        return std::make_shared<Number>(result);
    }
};

class MultiplyOperator : public Function {
public:
    MultiplyOperator() : Function("*") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return true;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        int product = 1;
        for (auto& arg: arguments) {
            auto number = RequireNumberArgument(Eval(arg, scope));
            product *= number->GetValue();
        }
        return std::make_shared<Number>(product);
    }
};

class DivisionOperator : public Function {
public:
    DivisionOperator() : Function("/") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() > 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto it = arguments.begin();
        auto end = arguments.end();
        auto element = RequireNumberArgument(Eval(*it, scope));
        int result = element->GetValue();
        while (++it < end) {
            element = RequireNumberArgument(Eval(*it, scope));
            result /= element->GetValue();
        }
        return std::make_shared<Number>(result);
    }
};

class IfOperator : public SpecialFormFunction {
public:
    IfOperator() : SpecialFormFunction("if") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() > 1 && arguments.size() < 4;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (Eval(arguments[0], scope)->AsBoolean()) {
            return Eval(arguments[1], scope);
        }
        if (arguments.size() > 2) {
            return Eval(arguments[2], scope);
        }
        return kSchemeNull;
    }
};

class AndOperator : public Function {
public:
    AndOperator() : Function("and") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return true;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            return kSchemeTrue;
        }
        auto it = arguments.begin();
        auto last = arguments.end() - 1;
        while (it < last) {
            auto element = Eval(*it, scope);
            if (!element->AsBoolean()) {
                return element;
            }
            ++it;
        }
        return Eval(*last, scope);
    }
};

class OrOperator : public Function {
public:
    OrOperator() : Function("or") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return true;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            return kSchemeFalse;
        }
        auto it = arguments.begin();
        auto last = arguments.end() - 1;
        while (it < last) {
            auto element = Eval(*it, scope);
            if (element->AsBoolean()) {
                return element;
            }
            ++it;
        }
        return Eval(*last, scope);
    }
};

class NotOperator : public Function {
public:
    NotOperator() : Function("not") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (Eval(arguments[0], scope)->AsBoolean()) {
            return kSchemeFalse;
        }
        return kSchemeTrue;
    }
};

class LessOperator : public Function {
public:
    LessOperator() : Function("<") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.empty() || arguments.size() > 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            return kSchemeTrue;
        }
        auto it = arguments.begin();
        auto end = arguments.end();
        auto previous = RequireNumberArgument(Eval(*it, scope));
        while (++it < end) {
            auto current = RequireNumberArgument(Eval(*it, scope));
            if (!(previous->GetValue() < current->GetValue())) {
                return kSchemeFalse;
            }
            previous = current;
        }
        return kSchemeTrue;
    }
};

class LeqOperator : public Function {
public:
    LeqOperator() : Function("<=") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.empty() || arguments.size() > 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            return kSchemeTrue;
        }
        auto it = arguments.begin();
        auto end = arguments.end();
        auto previous = RequireNumberArgument(Eval(*it, scope));
        while (++it < end) {
            auto current = RequireNumberArgument(Eval(*it, scope));
            if (!(previous->GetValue() <= current->GetValue())) {
                return kSchemeFalse;
            }
            previous = current;
        }
        return kSchemeTrue;
    }
};

class GreaterOperator : public Function {
public:
    GreaterOperator() : Function(">") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.empty() || arguments.size() > 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            return kSchemeTrue;
        }
        auto it = arguments.begin();
        auto end = arguments.end();
        auto previous = RequireNumberArgument(Eval(*it, scope));
        while (++it < end) {
            auto current = RequireNumberArgument(Eval(*it, scope));
            if (!(previous->GetValue() > current->GetValue())) {
                return kSchemeFalse;
            }
            previous = current;
        }
        return kSchemeTrue;
    }
};

class GeqOperator : public Function {
public:
    GeqOperator() : Function(">=") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.empty() || arguments.size() > 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            return kSchemeTrue;
        }
        auto it = arguments.begin();
        auto end = arguments.end();
        auto previous = RequireNumberArgument(Eval(*it, scope));
        while (++it < end) {
            auto current = RequireNumberArgument(Eval(*it, scope));
            if (!(previous->GetValue() >= current->GetValue())) {
                return kSchemeFalse;
            }
            previous = current;
        }
        return kSchemeTrue;
    }
};

class EqualsOperator : public Function {
public:
    EqualsOperator() : Function("=") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.empty() || arguments.size() > 1;
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        if (arguments.empty()) {
            return kSchemeTrue;
        }
        auto it = arguments.begin();
        auto end = arguments.end();
        auto previous = RequireNumberArgument(Eval(*it, scope));
        while (++it < end) {
            auto current = RequireNumberArgument(Eval(*it, scope));
            if (!(previous->GetValue() == current->GetValue())) {
                return kSchemeFalse;
            }
            previous = current;
        }
        return kSchemeTrue;
    }
};

template <class T>
class TypeCheckPredicate : public Function {
protected:
    TypeCheckPredicate(std::string name) : Function(name) {
    }

public:
    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 1;
    }

    virtual std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                          std::shared_ptr<Scope> scope) override {
        if (Is<T>(Eval(arguments[0], scope))) {
            return kSchemeTrue;
        }
        return kSchemeFalse;
    }
};

class BooleanPredicate : public TypeCheckPredicate<Boolean> {
public:
    BooleanPredicate() : TypeCheckPredicate<Boolean>("boolean?") {
    }
};

class NumberPredicate : public TypeCheckPredicate<Number> {
public:
    NumberPredicate() : TypeCheckPredicate<Number>("number?") {
    }
};

class SymbolPredicate : public TypeCheckPredicate<Symbol> {
public:
    SymbolPredicate() : TypeCheckPredicate<Symbol>("symbol?") {
    }
};

class PairPredicate : public TypeCheckPredicate<Cell> {
public:
    PairPredicate() : TypeCheckPredicate<Cell>("pair?") {
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto elem = Eval(arguments[0], scope);
        if (Is<Cell>(elem) && bool(As<Cell>(elem)->GetFirst())) {
            return kSchemeTrue;
        }
        return kSchemeFalse;
    }
};

class ListPredicate : public TypeCheckPredicate<Cell> {
public:
    ListPredicate() : TypeCheckPredicate<Cell>("list?") {
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        auto elem = Eval(arguments[0], scope);
        if (!Is<Cell>(elem)) {
            return kSchemeFalse;
        }
        while (elem && Is<Cell>(elem)) {
            elem = As<Cell>(elem)->GetSecond();
        }
        if (elem) {
            return kSchemeFalse;
        }
        return kSchemeTrue;
    }
};

class NullPredicate : public Function {
public:
    NullPredicate() : Function("null?") {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == 1;
    }

    virtual std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                          std::shared_ptr<Scope> scope) override {
        auto elem = Eval(arguments[0], scope);
        if (elem == kSchemeNull || (Is<Cell>(elem) && (!bool(As<Cell>(elem)->GetFirst())))) {
            return kSchemeTrue;
        }
        return kSchemeFalse;
    }
};
