#pragma once

#include <deque>

#include <error.h>
#include <object.h>
#include <scope.h>

class Function : public Object {
protected:
    explicit Function(std::string name) : name_(name) {
    }

public:
    virtual std::string Repr() const override {
        return name_;
    }

    bool AsBoolean() const override {
        return true;
    }

    virtual bool ApprovesNumberOfArguments(
            const std::deque<std::shared_ptr<Object>>& arguments) = 0;

    virtual std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                          std::shared_ptr<Scope> scope) = 0;

private:
    std::string name_;
};

class SpecialFormFunction : public Function {
protected:
    SpecialFormFunction(std::string name) : Function(name) {
    }
};

std::shared_ptr<Object> Eval(std::shared_ptr<Object> expression, std::shared_ptr<Scope> scope);

class CustomLambdaFunction : public Function {
public:
    CustomLambdaFunction(std::shared_ptr<Scope> scope, std::deque<std::string>&& arg_names,
                         std::deque<std::shared_ptr<Object>>&& expressions)
            : Function("lambda-function"),
              lambda_birth_scope_(scope),
              arg_names_(std::move(arg_names)),
              expressions_(std::move(expressions)) {
    }

    bool ApprovesNumberOfArguments(const std::deque<std::shared_ptr<Object>>& arguments) override {
        return arguments.size() == arg_names_.size();
    }

    std::shared_ptr<Object> Apply(std::deque<std::shared_ptr<Object>>& arguments,
                                  std::shared_ptr<Scope> scope) override {
        std::shared_ptr<Scope> execution_scope =
                std::shared_ptr<Scope>(new Scope(lambda_birth_scope_));
        PopulateExecutionScope(arguments, execution_scope, scope);
        auto it = expressions_.begin();
        auto last = expressions_.end() - 1;
        while (it < last) {
            Eval(*it, execution_scope);
            ++it;
        }
        auto result = Eval(*it, execution_scope);
        return result;
    }

private:
    std::shared_ptr<Scope> lambda_birth_scope_;
    std::deque<std::string> arg_names_;
    std::deque<std::shared_ptr<Object>> expressions_;

    void PopulateExecutionScope(const std::deque<std::shared_ptr<Object>>& arguments,
                                std::shared_ptr<Scope> exec_scope,
                                std::shared_ptr<Scope> caller_scope) {
        for (size_t i = 0; i < arg_names_.size(); ++i) {
            exec_scope->DefineVariable(arg_names_[i], Eval(arguments[i], caller_scope));
        }
    }
};
