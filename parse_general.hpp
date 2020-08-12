#pragma once

#include <vector>

enum TokenClass
{   TOKEN_CLASS_NONE
,   TOKEN_CLASS_COMMENT
,   TOKEN_CLASS_NUMBER
,   TOKEN_CLASS_CHAR
,   TOKEN_CLASS_STRING
,   TOKEN_CLASS_OPERATOR
,   TOKEN_CLASS_NOUN
,   TOKEN_CLASS_UNKNOWN
};

struct TokenGeneral
{
    std::vector<char> raw;
    TokenClass tClass;
    size_t line;
    size_t column;
};

extern bool parseGeneral (FILE* file, std::vector<TokenGeneral> &tokenList);
