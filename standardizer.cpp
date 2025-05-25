#include "standardizer.h"
#include <iostream>

using namespace std;

void standardize(ASTNode *node)
{
    for (ASTNode *child : node->children)
    {
        standardize(child); // post-order traversal
    }

    // Apply transformations based on node label
    if (node->label == "let")
    {
        // let D in E => gamma (lambda D E) D
        ASTNode *gammaNode = new ASTNode("gamma");
        ASTNode *lambdaNode = new ASTNode("lambda");

        lambdaNode->children.push_back(node->children[0]); // D
        lambdaNode->children.push_back(node->children[1]); // E

        gammaNode->children.push_back(lambdaNode);
        gammaNode->children.push_back(node->children[0]);

        *node = *gammaNode;
    }
    else if (node->label == "where")
    {
        // E where D => gamma (lambda D E) D
        ASTNode *gammaNode = new ASTNode("gamma");
        ASTNode *lambdaNode = new ASTNode("lambda");

        lambdaNode->children.push_back(node->children[1]); // D
        lambdaNode->children.push_back(node->children[0]); // E

        gammaNode->children.push_back(lambdaNode);
        gammaNode->children.push_back(node->children[1]);

        *node = *gammaNode;
    }
    else if (node->label == "function_form")
    {
        // ID params... E => = ID lambda (params...) E
        string idLabel = node->children[0]->label;
        ASTNode *idNode = new ASTNode(idLabel);

        ASTNode *lambdaNode = new ASTNode("lambda");
        for (size_t i = 1; i < node->children.size() - 1; ++i)
        {
            lambdaNode->children.push_back(node->children[i]);
        }
        lambdaNode->children.push_back(node->children.back());

        ASTNode *equalNode = new ASTNode("=");
        equalNode->children.push_back(idNode);
        equalNode->children.push_back(lambdaNode);

        *node = *equalNode;
    }
    else if (node->label == "and")
    {
        // and D1 D2 ... Dn => = (x1,...,xn) (tau E1,...,En)
        ASTNode *eqNode = new ASTNode("=");
        ASTNode *idList = new ASTNode(",");
        ASTNode *tauList = new ASTNode("tau");

        for (ASTNode *def : node->children)
        {
            if (def->label == "=")
            {
                idList->children.push_back(def->children[0]);
                tauList->children.push_back(def->children[1]);
            }
        }

        eqNode->children.push_back(idList);
        eqNode->children.push_back(tauList);

        *node = *eqNode;
    }
    else if (node->label == "within")
    {
        // D1 within D2 => = x (gamma (lambda x2 E1) E2)
        ASTNode *eqNode = new ASTNode("=");
        ASTNode *gammaNode = new ASTNode("gamma");
        ASTNode *lambdaNode = new ASTNode("lambda");

        ASTNode *x2 = node->children[1]->children[0];
        ASTNode *E1 = node->children[0]->children[1];
        ASTNode *E2 = node->children[1]->children[1];

        lambdaNode->children.push_back(x2);
        lambdaNode->children.push_back(E1);
        gammaNode->children.push_back(lambdaNode);
        gammaNode->children.push_back(E2);

        eqNode->children.push_back(node->children[0]->children[0]);
        eqNode->children.push_back(gammaNode);

        *node = *eqNode;
    }
    else if (node->label == "rec")
    {
        // rec x = E => = x (Y* lambda x E)
        ASTNode *eqNode = new ASTNode("=");
        ASTNode *gammaNode = new ASTNode("gamma");
        ASTNode *Ystar = new ASTNode("Y*");
        ASTNode *lambdaNode = new ASTNode("lambda");

        ASTNode *x = node->children[0]->children[0];
        ASTNode *E = node->children[0]->children[1];

        lambdaNode->children.push_back(x);
        lambdaNode->children.push_back(E);
        gammaNode->children.push_back(Ystar);
        gammaNode->children.push_back(lambdaNode);

        eqNode->children.push_back(x);
        eqNode->children.push_back(gammaNode);

        *node = *eqNode;
    }
}
