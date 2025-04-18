#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_set>

enum class TokenType
{
    IDENTIFIER,
    INTEGER,
    STRING,
    OPERATOR,
    PUNCTUATION,
    KEYWORD,
    END_OF_FILE
};

struct Token
{
    TokenType type;
    std::string value;
};

std::vector<Token> tokenize(const std::string &input);

#endif
