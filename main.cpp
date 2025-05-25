#include "lexer.h"
#include "parser.h"
#include "standardizer.h"
#include "cse_machine.h"
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
    {
        printAST(root);
    }
    else
    {
        standardize(root);
        if (!printAst)
        {
            standardize(root);
            cout << "Output of the above program is:\n";

            CSEMachine cse(root);
            Value result = cse.run();

            // Print result based on its type
            if (std::holds_alternative<int>(result))
            {
                cout << std::get<int>(result) << endl;
            }
            else if (std::holds_alternative<bool>(result))
            {
                cout << (std::get<bool>(result) ? "true" : "false") << endl;
            }
            else if (std::holds_alternative<std::string>(result))
            {
                cout << std::get<std::string>(result) << endl;
            }
        }
    }
    else

        // Placeholder: here you’ll run the CSE machine on root
        printAST(root); // temporary: show standardized tree
}

return 0;
}
