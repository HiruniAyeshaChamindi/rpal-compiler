#ifndef CSE_MACHINE_H
#define CSE_MACHINE_H

#include "parser.h"
#include <stack>
#include <vector>
#include <map>
#include <variant>
#include <string>

// Represents a runtime value
using Value = std::variant<int, bool, std::string>;

// Environment frame: mapping from variable name to Value
using EnvFrame = std::map<std::string, Value>;

// Environment: stack of frames
using Environment = std::vector<EnvFrame>;

// Control structure element
struct ControlItem
{
    std::string label;
    std::vector<ASTNode *> args;
};

// The CSE machine runtime
class CSEMachine
{
public:
    CSEMachine(ASTNode *root);
    Value run();

private:
    std::stack<Value> stack;
    std::stack<ControlItem> control;
    Environment env;

    void buildControlStructure(ASTNode *node);
    Value eval();
    Value apply(const std::string &op, const std::vector<Value> &args);
    Value lookup(const std::string &id);
};

#endif
