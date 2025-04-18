#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <vector>
#include <string>

struct ASTNode
{
    std::string label;
    std::vector<ASTNode *> children;
    ASTNode(const std::string &lbl) : label(lbl) {}
};

void setTokens(const std::vector<Token> &toks);
ASTNode *parse();
void printAST(ASTNode *node, int depth = 0);

#endif
