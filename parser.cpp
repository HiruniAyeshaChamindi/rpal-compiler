#include "parser.h"
#include <iostream>

using namespace std;

static vector<Token> tokens;
static size_t currentTokenIndex = 0;

// Forward declarations
static ASTNode *parseE();
static ASTNode *parseEw();
static ASTNode *parseT();

static ASTNode *parseTa();
static ASTNode *parseTc();
static ASTNode *parseB();
static ASTNode *parseBt();
static ASTNode *parseBs();
static ASTNode *parseBp();
static ASTNode *parseA();
static ASTNode *parseAt();
static ASTNode *parseAf();
static ASTNode *parseAp();
static ASTNode *parseR();
static ASTNode *parseRn();
static ASTNode *parseD();
static ASTNode *parseDa();
static ASTNode *parseDr();
static ASTNode *parseDb();
static ASTNode *parseVb();
static ASTNode *parseVl();

// Token utilities
static Token peek()
{
    if (currentTokenIndex >= tokens.size())
        return {TokenType::END_OF_FILE, ""};
    return tokens[currentTokenIndex];
}

static Token next()
{
    return tokens[currentTokenIndex++];
}

static bool match(const string &value)
{
    if (peek().value == value)
    {
        next();
        return true;
    }
    return false;
}

static bool match(TokenType type)
{
    if (peek().type == type)
    {
        next();
        return true;
    }
    return false;
}

// Lexer integration
void setTokens(const vector<Token> &toks)
{
    tokens = toks;
    currentTokenIndex = 0;
}

// AST printing
void printAST(ASTNode *node, int depth)
{
    for (int i = 0; i < depth; i++)
        cout << ".";
    cout << node->label << endl;
    for (ASTNode *child : node->children)
        printAST(child, depth + 1);
}

// Parser entry
ASTNode *parse()
{
    return parseE();
}

// Full RPAL grammar parser implementation

static ASTNode *parseE()
{
    if (match("let"))
    {
        ASTNode *node = new ASTNode("let");
        node->children.push_back(parseD());
        if (!match("in"))
        {
            cerr << "Expected 'in'\n";
            exit(1);
        }
        node->children.push_back(parseE());
        return node;
    }
    if (match("fn"))
    {
        ASTNode *node = new ASTNode("lambda");
        do
        {
            node->children.push_back(parseVb());
        } while (peek().type == TokenType::IDENTIFIER || peek().value == "(");
        if (!match("."))
        {
            cerr << "Expected '.'\n";
            exit(1);
        }
        node->children.push_back(parseE());
        return node;
    }
    return parseEw();
}

static ASTNode *parseEw()
{
    ASTNode *t = parseT();
    if (match("where"))
    {
        ASTNode *node = new ASTNode("where");
        node->children.push_back(t);
        node->children.push_back(parseDr());
        return node;
    }
    return t;
}

static ASTNode *parseT()
{
    ASTNode *ta = parseTa();
    if (peek().value == ",")
    {
        ASTNode *node = new ASTNode("tau");
        node->children.push_back(ta);
        while (match(","))
            node->children.push_back(parseTa());
        return node;
    }
    return ta;
}

static ASTNode *parseTa()
{
    ASTNode *left = parseTc();
    while (match("aug"))
    {
        ASTNode *node = new ASTNode("aug");
        node->children.push_back(left);
        node->children.push_back(parseTc());
        left = node;
    }
    return left;
}

static ASTNode *parseTc()
{
    ASTNode *left = parseB();
    if (match("->"))
    {
        ASTNode *node = new ASTNode("->");
        node->children.push_back(left);
        node->children.push_back(parseTc());
        if (!match("|"))
        {
            cerr << "Expected '|'\n";
            exit(1);
        }
        node->children.push_back(parseTc());
        return node;
    }
    return left;
}

static ASTNode *parseB()
{
    ASTNode *left = parseBt();
    while (match("or"))
    {
        ASTNode *node = new ASTNode("or");
        node->children.push_back(left);
        node->children.push_back(parseBt());
        left = node;
    }
    return left;
}

static ASTNode *parseBt()
{
    ASTNode *left = parseBs();
    while (match("&"))
    {
        ASTNode *node = new ASTNode("&");
        node->children.push_back(left);
        node->children.push_back(parseBs());
        left = node;
    }
    return left;
}

static ASTNode *parseBs()
{
    if (match("not"))
    {
        ASTNode *node = new ASTNode("not");
        node->children.push_back(parseBp());
        return node;
    }
    return parseBp();
}

static ASTNode *parseBp()
{
    ASTNode *left = parseA();
    string ops[] = {"gr", ">", "ge", ">=", "ls", "<", "le", "<=", "eq", "ne"};
    for (const string &op : ops)
    {
        if (match(op))
        {
            ASTNode *node = new ASTNode(op);
            node->children.push_back(left);
            node->children.push_back(parseA());
            return node;
        }
    }
    return left;
}

static ASTNode *parseA()
{
    if (match("+"))
    {
        ASTNode *node = new ASTNode("+");
        node->children.push_back(parseAt());
        return node;
    }
    if (match("-"))
    {
        ASTNode *node = new ASTNode("neg");
        node->children.push_back(parseAt());
        return node;
    }
    ASTNode *left = parseAt();
    while (peek().value == "+" || peek().value == "-")
    {
        string op = next().value;
        ASTNode *node = new ASTNode(op);
        node->children.push_back(left);
        node->children.push_back(parseAt());
        left = node;
    }
    return left;
}

static ASTNode *parseAt()
{
    ASTNode *left = parseAf();
    while (peek().value == "*" || peek().value == "/")
    {
        string op = next().value;
        ASTNode *node = new ASTNode(op);
        node->children.push_back(left);
        node->children.push_back(parseAf());
        left = node;
    }
    return left;
}

static ASTNode *parseAf()
{
    ASTNode *left = parseAp();
    if (match("**"))
    {
        ASTNode *node = new ASTNode("**");
        node->children.push_back(left);
        node->children.push_back(parseAf());
        return node;
    }
    return left;
}

static ASTNode *parseAp()
{
    ASTNode *left = parseR();
    while (match("@"))
    {
        if (peek().type == TokenType::IDENTIFIER)
        {
            Token id = next();
            ASTNode *node = new ASTNode("@");
            node->children.push_back(left);
            node->children.push_back(new ASTNode("<ID:" + id.value + ">"));
            node->children.push_back(parseR());
            left = node;
        }
        else
        {
            break;
        }
    }
    return left;
}

static ASTNode *parseR()
{
    ASTNode *left = parseRn();
    while (true)
    {
        ASTNode *nextRn = parseRn();
        if (!nextRn)
            break;
        ASTNode *node = new ASTNode("gamma");
        node->children.push_back(left);
        node->children.push_back(nextRn);
        left = node;
    }
    return left;
}

static ASTNode *parseRn()
{
    Token tok = peek();
    if (tok.type == TokenType::IDENTIFIER || tok.type == TokenType::INTEGER || tok.type == TokenType::STRING ||
        tok.value == "true" || tok.value == "false" || tok.value == "nil" || tok.value == "dummy")
    {
        next();
        string type = (tok.type == TokenType::IDENTIFIER) ? "ID" : (tok.type == TokenType::INTEGER) ? "INT"
                                                               : (tok.type == TokenType::STRING)    ? "STR"
                                                                                                    : "KW";
        return new ASTNode("<" + type + ":" + tok.value + ">");
    }
    if (match("("))
    {
        ASTNode *node = parseE();
        if (!match(")"))
        {
            cerr << "Expected ')'\n";
            exit(1);
        }
        return node;
    }
    return nullptr;
}

static ASTNode *parseD()
{
    ASTNode *da = parseDa();
    if (match("within"))
    {
        ASTNode *node = new ASTNode("within");
        node->children.push_back(da);
        node->children.push_back(parseD());
        return node;
    }
    return da;
}

static ASTNode *parseDa()
{
    ASTNode *dr = parseDr();
    if (match("and"))
    {
        ASTNode *node = new ASTNode("and");
        node->children.push_back(dr);
        while (true)
        {
            node->children.push_back(parseDr());
            if (!match("and"))
                break;
        }
        return node;
    }
    return dr;
}

static ASTNode *parseDr()
{
    if (match("rec"))
    {
        ASTNode *node = new ASTNode("rec");
        node->children.push_back(parseDb());
        return node;
    }
    return parseDb();
}

static ASTNode *parseDb()
{
    if (match("("))
    {
        ASTNode *node = parseD();
        if (!match(")"))
        {
            cerr << "Expected ')'\n";
            exit(1);
        }
        return node;
    }
    if (peek().type == TokenType::IDENTIFIER)
    {
        string id = next().value;
        if (peek().type == TokenType::IDENTIFIER || peek().value == "(")
        {
            ASTNode *node = new ASTNode("function_form");
            node->children.push_back(new ASTNode("<ID:" + id + ">"));
            while (peek().type == TokenType::IDENTIFIER || peek().value == "(")
                node->children.push_back(parseVb());
            if (!match("="))
            {
                cerr << "Expected '='\n";
                exit(1);
            }
            node->children.push_back(parseE());
            return node;
        }
        else if (match("="))
        {
            ASTNode *node = new ASTNode("=");
            node->children.push_back(new ASTNode("<ID:" + id + ">"));
            node->children.push_back(parseE());
            return node;
        }
    }
    cerr << "Unexpected token in Db\n";
    exit(1);
}

static ASTNode *parseVb()
{
    if (peek().type == TokenType::IDENTIFIER)
    {
        return new ASTNode("<ID:" + next().value + ">");
    }
    else if (match("("))
    {
        if (match(")"))
        {
            return new ASTNode("()");
        }
        ASTNode *node = parseVl();
        if (!match(")"))
        {
            cerr << "Expected ')'\n";
            exit(1);
        }
        return node;
    }
    cerr << "Invalid Vb\n";
    exit(1);
}

static ASTNode *parseVl()
{
    ASTNode *node = new ASTNode(",");
    if (peek().type != TokenType::IDENTIFIER)
    {
        cerr << "Expected identifier in Vl\n";
        exit(1);
    }
    node->children.push_back(new ASTNode("<ID:" + next().value + ">"));
    while (match(","))
    {
        if (peek().type != TokenType::IDENTIFIER)
        {
            cerr << "Expected identifier after ',' in Vl\n";
            exit(1);
        }
        node->children.push_back(new ASTNode("<ID:" + next().value + ">"));
    }
    return node;
}
