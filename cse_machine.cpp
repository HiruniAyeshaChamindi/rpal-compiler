#include "cse_machine.h"
#include <iostream>

CSEMachine::CSEMachine(ASTNode *root)
{
    buildControlStructure(root);
}

Value CSEMachine::run()
{
    return eval();
}

void CSEMachine::buildControlStructure(ASTNode *node)
{
    if (!node)
        return;

    if (node->label.find("<INT:") == 0 || node->label.find("<ID:") == 0 ||
        node->label == "true" || node->label == "false")
    {
        control.push({node->label, {}});
    }
    else
    {
        for (auto it = node->children.rbegin(); it != node->children.rend(); ++it)
        {
            buildControlStructure(*it);
        }
        control.push({node->label, node->children});
    }
}

Value CSEMachine::eval()
{
    while (!control.empty())
    {
        ControlItem item = control.top();
        control.pop();

        const std::string &label = item.label;

        if (label.find("<INT:") == 0)
        {
            int val = std::stoi(label.substr(5, label.size() - 6));
            stack.push(val);
        }
        else if (label.find("<ID:") == 0)
        {
            std::string id = label.substr(4, label.size() - 5);
            stack.push(lookup(id));
        }
        else if (label == "true")
        {
            stack.push(true);
        }
        else if (label == "false")
        {
            stack.push(false);
        }
        else if (label == "+" || label == "-" || label == "*" || label == "/")
        {
            Value rhs = stack.top();
            stack.pop();
            Value lhs = stack.top();
            stack.pop();
            stack.push(apply(label, {lhs, rhs}));
        }
        else if (label == "gamma")
        {
            Value arg = stack.top();
            stack.pop();
            Value func = stack.top();
            stack.pop();
            std::cout << "[gamma] Function application placeholder.\n";
        }
        else
        {
            std::cout << "Unhandled control label: " << label << std::endl;
        }
    }

    return stack.top();
}

Value CSEMachine::apply(const std::string &op, const std::vector<Value> &args)
{
    if (op == "+")
        return std::get<int>(args[0]) + std::get<int>(args[1]);
    if (op == "-")
        return std::get<int>(args[0]) - std::get<int>(args[1]);
    if (op == "*")
        return std::get<int>(args[0]) * std::get<int>(args[1]);
    if (op == "/")
        return std::get<int>(args[0]) / std::get<int>(args[1]);
    return 0;
}

Value CSEMachine::lookup(const std::string &id)
{
    for (auto it = env.rbegin(); it != env.rend(); ++it)
    {
        if (it->find(id) != it->end())
        {
            return (*it)[id];
        }
    }
    std::cerr << "Unbound identifier: " << id << std::endl;
    return 0;
}
