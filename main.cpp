#include "lexer.h"
#include "parser.h"
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: ./myrpal [-ast] filename\n";
        return 1;
    }

    bool printAst = false;
    string filename;

    if (string(argv[1]) == "-ast")
    {
        printAst = true;
        filename = argv[2];
    }
    else
    {
        filename = argv[1];
    }

    ifstream file(filename);
    if (!file)
    {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    string line, content;
    while (getline(file, line))
        content += line + '\n';
    file.close();

    vector<Token> toks = tokenize(content);
    setTokens(toks);
    ASTNode *root = parse();

    if (printAst)
        printAST(root);
    // should be changed.
    else
        cout << "Output of the above program is:" << endl;

    return 0;
}
