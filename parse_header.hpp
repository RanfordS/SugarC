#pragma once
#include <cstdio>
#include <string>
#include <vector>

enum TokenClass
{   TK_NONE = 0
,   TK_BRACKET_ROUND_OPEN
,   TK_BRACKET_SQUARE_OPEN
,   TK_BRACKET_CURLY_OPEN
,   TK_BRACKET_ROUND_CLOSE
,   TK_BRACKET_SQUARE_CLOSE
,   TK_BRACKET_CURLY_CLOSE
,   TK_BRACKET_ROUND_BLOCK
,   TK_BRACKET_SQUARE_BLOCK
,   TK_BRACKET_CURLY_BLOCK
,   TK_COMMENT_LINE
,   TK_COMMENT_BLOCK
,   TK_LITERAL_CHAR
,   TK_LITERAL_STRING
,   TK_LITERAL_NUMBER
,   TK_GENERIC_NOUN
,   TK_NOUN_FUNCTION
,   TK_NOUN_VARIABLE
,   TK_NOUN_TYPE
,   TK_OPERATOR
,   TK_STATEMENT

,   TK_INVALID
,   TK_COUNT
};

#define TK_ISBRACKET(t) (TK_BRACKET_ROUND_OPEN <= (t) && (t) <= TK_BRACKET_CURLY_BLOCK)
#define TK_ISBRACKET_OPEN(t) (TK_BRACKET_ROUND_OPEN <= (t) && (t) <= TK_BRACKET_CURLY_OPEN)
#define TK_ISBRACKET_CLOSE(t) (TK_BRACKET_ROUND_CLOSE <= (t) && (t) <= TK_BRACKET_CURLY_CLOSE)
#define TK_ISBRACKET_MATCH(a,b) ((a) + 3 == (b))
#define TK_BRACKET_OPEN_TO_BLOCK(a) ((TokenClass)((a) + 6))

struct Token
{
    TokenClass tClass;
    std::string raw;
    std::vector<Token> subTokens;
    size_t line;
    size_t column;
};

struct Options
{
    uint8_t tabSize;
    bool tabFixedSize;
};

#define ISNUMBER(c) ('0' <= (c) && (c) <= '9')
#define ISLETTER(c) (('A' <= (c) && (c) <= 'Z') || ('a' <= (c) && (c) <= 'z') || (c) == '_')
#define ISWHITE(c) ((c) == '\t' || (c) == '\n' || (c) == '\r' || (c) == ' ')
// Excludes control characters
#define ISBRACKET(c) ((c) == '(' || (c) == ')' || (c) == '[' || (c) == ']' || (c) == '{' || (c) == '}')
#define ISASCII(c) (' ' <= (c) && (c) <= '~')
extern bool parseInitial (FILE* file, std::vector<Token> &list, Options* options);

extern bool checkBrackets (std::vector<Token> &list);
extern void parseBrackets (std::vector<Token> &initial, Token* fileScope);

extern void parseStatements (Token* brackets, Token* statements);

extern void parseContext (Token* scope);

extern void highlighter (Token &root);
