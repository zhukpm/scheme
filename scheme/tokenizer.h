#pragma once

#include <variant>
#include <optional>
#include <istream>

#include <ctype.h>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken {
    OPEN, CLOSE
};

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

struct UnknownToken {
    char value;

    bool operator==(const UnknownToken& other) const {
        return value == other.value;
    }
};

typedef std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, UnknownToken>
        Token;

class Tokenizer {
public:
    Tokenizer(std::istream* in) : in_(in) {
        Next();
    }

    bool IsEnd() {
        return terminated_;
    }

    void Next() {
        FlushToNextToken();

        if (IsEndOfStream()) {
            terminated_ = true;
            return;
        }

        char symbol = in_->get();

        if (IsSymbolBeginning(symbol)) {
            token_ = SymbolToken{ReadSymbol(symbol)};
            return;
        }

        if (std::isdigit(symbol)) {
            token_ = ConstantToken{ReadInt(symbol)};
            return;
        }

        switch (symbol) {
            case '(':
                token_ = BracketToken::OPEN;
                return;
            case ')':
                token_ = BracketToken::CLOSE;
                return;
            case '\'':
                token_ = QuoteToken{};
                return;
            case '.':
                token_ = DotToken{};
                return;
            case '+':
                if (!IsEndOfStream() && std::isdigit(in_->peek())) {
                    token_ = ConstantToken{ReadInt(in_->get())};
                } else {
                    token_ = SymbolToken{"+"};
                }
                return;
            case '-':
                if (!IsEndOfStream() && std::isdigit(in_->peek())) {
                    token_ = ConstantToken{-ReadInt(in_->get())};
                } else {
                    token_ = SymbolToken{"-"};
                }
                return;
            case '/':
                token_ = SymbolToken{"/"};
                return;
        }

        token_ = UnknownToken{symbol};
    }

    Token GetToken() {
        return token_;
    }

private:
    std::istream* in_;
    Token token_;
    bool terminated_ = false;

    bool IsEndOfStream() {
        return in_->eof() || in_->peek() == -1;
    }

    void FlushToNextToken() {
        while (!IsEndOfStream() && !std::isgraph(in_->peek())) {
            in_->get();
        }
    }

    bool IsSymbolBeginning(char symbol) const {
        // [a-zA-Z<=>*#]
        return std::isalpha(symbol) || symbol == '<' || symbol == '>' || symbol == '=' ||
               symbol == '*' || symbol == '#';
    }

    bool IsSymbolCharacter(char symbol) const {
        return IsSymbolBeginning(symbol) || std::isdigit(symbol) || symbol == '?' ||
               symbol == '!' || symbol == '-';
    }

    std::string ReadSymbol(char first) {
        std::string symbol_string(1, first);
        while (!IsEndOfStream() && IsSymbolCharacter(in_->peek())) {
            symbol_string += in_->get();
        }
        return symbol_string;
    }

    int ReadInt(char first) {
        std::string number(1, first);
        while (!IsEndOfStream() && std::isdigit(in_->peek())) {
            number += in_->get();
        }
        return std::stoi(number);
    }
};
