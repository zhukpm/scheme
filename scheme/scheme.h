#pragma once

#include <sstream>

#include <error.h>
#include <object.h>
#include <scope.h>
#include <function.h>
#include <eval.h>
#include <std-lib.h>
#include <tokenizer.h>
#include <parser.h>

class SchemeEngine {
public:
    SchemeEngine() : scope_(std::make_shared<Scope>()) {
    }

    void Init() {
        PopulateScope();
    }

    std::string Interpret(const std::string& expression) {
        std::stringstream ss{expression};
        Tokenizer tokenizer{&ss};

        auto obj = Read(&tokenizer);
        if (!tokenizer.IsEnd()) {
            throw SyntaxError{"Only one expression is expected. Given: " + expression};
        }

        return Eval(obj, scope_)->Repr();
    }

private:
    std::shared_ptr<Scope> scope_;

    void PopulateScope() {
        AddFunctionToScope(std::make_shared<QuoteFunction>());
        AddFunctionToScope(std::make_shared<MaxFunction>());
        AddFunctionToScope(std::make_shared<MinFunction>());
        AddFunctionToScope(std::make_shared<AbsFunction>());
        AddFunctionToScope(std::make_shared<LambdaFunction>());
        AddFunctionToScope(std::make_shared<ConsFunction>());
        AddFunctionToScope(std::make_shared<CarFunction>());
        AddFunctionToScope(std::make_shared<CdrFunction>());
        AddFunctionToScope(std::make_shared<ListFunction>());
        AddFunctionToScope(std::make_shared<ListRefFunction>());
        AddFunctionToScope(std::make_shared<ListTailFunction>());

        AddFunctionToScope(std::make_shared<DefineFunction>());
        AddFunctionToScope(std::make_shared<SetFunction>());
        AddFunctionToScope(std::make_shared<SetCarFunction>());
        AddFunctionToScope(std::make_shared<SetCdrFunction>());

        AddFunctionToScope(std::make_shared<PlusOperator>());
        AddFunctionToScope(std::make_shared<MinusOperator>());
        AddFunctionToScope(std::make_shared<MultiplyOperator>());
        AddFunctionToScope(std::make_shared<DivisionOperator>());
        AddFunctionToScope(std::make_shared<IfOperator>());

        AddFunctionToScope(std::make_shared<AndOperator>());
        AddFunctionToScope(std::make_shared<OrOperator>());
        AddFunctionToScope(std::make_shared<NotOperator>());
        AddFunctionToScope(std::make_shared<LessOperator>());
        AddFunctionToScope(std::make_shared<LeqOperator>());
        AddFunctionToScope(std::make_shared<GreaterOperator>());
        AddFunctionToScope(std::make_shared<GeqOperator>());
        AddFunctionToScope(std::make_shared<EqualsOperator>());

        AddFunctionToScope(std::make_shared<BooleanPredicate>());
        AddFunctionToScope(std::make_shared<NumberPredicate>());
        AddFunctionToScope(std::make_shared<SymbolPredicate>());
        AddFunctionToScope(std::make_shared<PairPredicate>());
        AddFunctionToScope(std::make_shared<ListPredicate>());
        AddFunctionToScope(std::make_shared<NullPredicate>());
    }

    void AddFunctionToScope(std::shared_ptr<Function>&& function) {
        scope_->DefineVariable(function->Repr(), function);
    }
};
