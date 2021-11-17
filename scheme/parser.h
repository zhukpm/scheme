#pragma once

#include <memory>
#include <variant>

#include <object.h>
#include <tokenizer.h>
#include <error.h>

inline void RequireNotEnd(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError{"Unexpected end of program"};
    }
}

template <class TokenClass>
inline void RequireEqual(Tokenizer* tokenizer, const TokenClass& expected,
                         std::string msg = "Syntax error") {
    Token token = tokenizer->GetToken();
    if (TokenClass* actual = std::get_if<TokenClass>(&token)) {
        if (*actual == expected) {
            return;
        }
    }
    throw SyntaxError{msg};
}

template <class TokenClass>
inline void RequireNotEqual(Tokenizer* tokenizer, const TokenClass& expected,
                            std::string msg = "Syntax error") {
    Token token = tokenizer->GetToken();
    if (TokenClass* actual = std::get_if<TokenClass>(&token)) {
        if (*actual == expected) {
            throw SyntaxError{msg};
        }
    }
}

inline std::shared_ptr<Cell> ReadList(Tokenizer* tokenizer);

inline std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        return nullptr;
    }

    Token token = tokenizer->GetToken();
    tokenizer->Next();

    if (ConstantToken* number = std::get_if<ConstantToken>(&token)) {
        return std::make_shared<Number>(number->value);
    }
    if (SymbolToken* symbol = std::get_if<SymbolToken>(&token)) {
        if (symbol->name == kSchemeTrue->Repr()) {
            return kSchemeTrue;
        } else if (symbol->name == kSchemeFalse->Repr()) {
            return kSchemeFalse;
        }
        return std::make_shared<Symbol>(symbol->name);
    }
    if (std::get_if<QuoteToken>(&token)) {
        return std::make_shared<Cell>(std::make_shared<Symbol>("quote"),
                                      std::make_shared<Cell>(Read(tokenizer)));
    }
    if (BracketToken* bracket = std::get_if<BracketToken>(&token)) {
        if (*bracket == BracketToken::OPEN) {
            return ReadList(tokenizer);
        }
    }

    throw SyntaxError{"Wrong token"};
}

inline std::shared_ptr<Cell> ReadList(Tokenizer* tokenizer) {
    // Reading first half of a list
    RequireNotEnd(tokenizer);

    Token token = tokenizer->GetToken();

    if (BracketToken* bracket = std::get_if<BracketToken>(&token)) {
        if (*bracket == BracketToken::CLOSE) {
            // This is an empty list
            tokenizer->Next();
            return nullptr;
        }
    }
    RequireNotEqual<DotToken>(tokenizer, {}, "Unexpected appearance of '.' symbol");

    auto first = Read(tokenizer);

    // Reading second half of a list
    RequireNotEnd(tokenizer);

    token = tokenizer->GetToken();

    if (BracketToken* bracket = std::get_if<BracketToken>(&token)) {
        if (*bracket == BracketToken::CLOSE) {
            tokenizer->Next();
            return std::make_shared<Cell>(first, nullptr);
        }
    }
    if (std::get_if<DotToken>(&token)) {
        tokenizer->Next();
        auto second = Read(tokenizer);

        RequireNotEnd(tokenizer);
        RequireEqual<BracketToken>(tokenizer, BracketToken::CLOSE,
                                   "')' expected after second element of a pair");

        tokenizer->Next();

        return std::make_shared<Cell>(first, second);
    }

    return std::make_shared<Cell>(first, ReadList(tokenizer));
}
