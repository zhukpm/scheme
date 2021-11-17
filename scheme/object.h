#pragma once

#include <memory>
#include <variant>
#include <deque>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;

    virtual std::string Repr() const = 0;

    virtual bool AsBoolean() const = 0;
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (std::dynamic_pointer_cast<T>(obj)) {
        return true;
    }
    return false;
}

class Number : public Object {
public:
    explicit Number(int value) : value_(value) {
    }

    int GetValue() const {
        return value_;
    }

    std::string Repr() const override {
        return std::to_string(value_);
    }

    bool AsBoolean() const override {
        return true;
    }

private:
    int value_;
};

class Symbol : public Object {
public:
    explicit Symbol(std::string name) : name_(name) {
    }

    const std::string& GetName() const {
        return name_;
    }

    std::string Repr() const override {
        return GetName();
    }

    bool AsBoolean() const override {
        return true;
    }

private:
    std::string name_;
};

class Cell : public Object {
public:
    explicit Cell(std::shared_ptr<Object> first = nullptr, std::shared_ptr<Object> second = nullptr)
            : first_(first), second_(second) {
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }

    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

    void SetFirst(std::shared_ptr<Object> object) {
        first_ = object;
    }

    void SetSecond(std::shared_ptr<Object> object) {
        second_ = object;
    }

    std::string Repr() const override {
        if (!first_) {
            return "()";
        }
        std::string repr = "(" + first_->Repr();
        if (!second_) {
            return repr + ")";
        }
        repr += " ";
        if (Is<Cell>(second_)) {
            return repr + second_->Repr().substr(1);
        }
        return repr + ". " + second_->Repr() + ")";
    }

    bool AsBoolean() const override {
        return true;
    }

    std::deque<std::shared_ptr<Object>> ToVector() const {
        if (!first_) {
            return {};
        }
        auto vec = std::deque<std::shared_ptr<Object>>{first_};
        auto second = second_;
        auto null = std::make_shared<Cell>();
        while (second) {
            if (Is<Cell>(second)) {
                auto tail = As<Cell>(second);
                if (tail->first_ == nullptr) {
                    vec.push_back(null);
                } else {
                    vec.push_back(tail->first_);
                }
                second = tail->second_;
            } else {
                vec.push_back(second);
                break;
            }
        }
        return vec;
    }

private:
    std::shared_ptr<Object> first_, second_;
};

class Boolean : public Object {
public:
    explicit Boolean(std::string symbol, bool value) : symbol_(symbol), value_(value) {
    }

    std::string Repr() const override {
        return symbol_;
    }

    bool GetValue() const {
        return value_;
    }

    bool AsBoolean() const override {
        return GetValue();
    }

private:
    const std::string symbol_;
    const bool value_;
};

const std::shared_ptr<Boolean> kSchemeTrue = std::make_shared<Boolean>("#t", true);
const std::shared_ptr<Boolean> kSchemeFalse = std::make_shared<Boolean>("#f", false);

const std::shared_ptr<Cell> kSchemeNull = std::make_shared<Cell>();
