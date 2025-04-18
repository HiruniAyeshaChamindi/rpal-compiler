#include "lexer.h"
#include <cctype>
#include <iostream>

using namespace std;

unordered_set<string> keywords = {
    "let", "in", "fn", "where", "rec", "aug", "or", "not", "gr", "ge", "ls",
    "le", "eq", "ne", "true", "false", "nil", "dummy", "within", "and"};

unordered_set<char> operatorSymbols = {
    '+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|',
    '$', '!', '#', '%', '^', '_', '[', ']', '{', '}', '"', '`', '?'};

unordered_set<char> punctuations = {'(', ')', ';', ','};

vector<Token> tokenize(const string &input)
{
    vector<Token> tokens;
    size_t i = 0;

    while (i < input.length())
    {
        char c = input[i];
        // ignore spaces
        if (isspace(c))
        {
            i++;
            continue;
        }
        // ignore comments
        if (c == '/' && i + 1 < input.length() && input[i + 1] == '/')
        {
            while (i < input.length() && input[i] != '\n')
                i++;
            continue;
        }
        // check whether the input is a letter A-Z or a-z
        if (isalpha(c))
        {
            string ident;
            while (i < input.length() && (isalnum(input[i]) || input[i] == '_'))
                ident += input[i++];
            if (keywords.count(ident))
                tokens.push_back({TokenType::KEYWORD, ident});
            else
                tokens.push_back({TokenType::IDENTIFIER, ident});
        }
        else if (isdigit(c))
        {
            string number;
            while (i < input.length() && isdigit(input[i]))
                number += input[i++];
            tokens.push_back({TokenType::INTEGER, number});
        }
        // Handling Strings
        else if (c == '\'')
        {
            string str;
            i++;
            while (i < input.length())
            {
                if (input[i] == '\\') // Handling Escape Characters
                {
                    str += input[i++];
                    str += input[i++];
                }
                else if (input[i] == '\'')
                {
                    i++;
                    break;
                }
                else
                {
                    str += input[i++];
                }
            }
            tokens.push_back({TokenType::STRING, str});
        }
        else if (operatorSymbols.count(c))
        {
            string op;
            while (i < input.length() && operatorSymbols.count(input[i]))
                op += input[i++];
            tokens.push_back({TokenType::OPERATOR, op});
        }
        else if (punctuations.count(c))
        {
            tokens.push_back({TokenType::PUNCTUATION, string(1, c)});
            i++;
        }
        else
        {
            cerr << "Unknown character: " << c << endl;
            i++;
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, ""});
    return tokens;
}
