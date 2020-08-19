#pragma once
#include <cstdio>
#include <string>
#include <vector>

enum TokenClass
{   TK_NONE = 0
,   TK_BRACKET_ROUND_OPEN
,   TK_BRACKET_ROUND_CLOSE
,   TK_BRACKET_SQUARE_OPEN
,   TK_BRACKET_SQUARE_CLOSE
,   TK_BRACKET_CURLY_OPEN
,   TK_BRACKET_CURLY_CLOSE
,   TK_COMMENT_LINE
,   TK_COMMENT_BLOCK
,   TK_LITERAL_CHAR
,   TK_LITERAL_STRING
,   TK_LITERAL_NUMBER
,   TK_GENERIC_NOUN
,   TK_OPERATOR
,   TK_STATEMENT

,   TK_INVALID
};

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

