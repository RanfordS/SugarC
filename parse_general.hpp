#pragma once

#include <vector>

enum TokenClass
{   TOKEN_CLASS_NONE
,   TOKEN_CLASS_COMMENT_LINE
,   TOKEN_CLASS_COMMENT_BLOCK
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

union TokenState
{
    struct
    {
        size_t currentposition;
        char suffix;
        bool hasDecimal     : 1;
        bool hasLeadingZero : 1;
        bool hasExponential : 1;
        bool hasHexMarker   : 1;
        //  if (hasHexMarker)
        //      assert (hasLeadingZero)
        //      return HEX
        //  if (hasDecimal || hasExponential)
        //      return FLOAT
        //  if (hasLeadingZero)
        //      return OCTAL
        //  return INT
    } number;
    struct
    {
    } string;
};

extern bool parseGeneral (FILE* file, std::vector<TokenGeneral> &tokenList);
