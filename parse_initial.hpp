#pragma once
#include <string>
#include <vector>

enum TokenClass
{   TOKEN_CLASS_NONE
,   TOKEN_CLASS_COMMENT_LINE
,   TOKEN_CLASS_COMMENT_BLOCK
,   TOKEN_CLASS_CHAR
,   TOKEN_CLASS_STRING
,   TOKEN_CLASS_OPERATOR
,   TOKEN_CLASS_NUMBER
,   TOKEN_CLASS_NOUN

,   TOKEN_CLASS_UNKNOWN
};

struct Token
{
    std::string raw;
    TokenClass tClass;
    size_t line;
    size_t column;

    union
    {   struct
        {   char last;
            char secondLast;
        }   delimitedInfo;
        struct
        {   size_t positionDecimal;
            size_t positionExponential;
            size_t positionHexMarker;
            bool hasLeadingZero;
        }   numberInfo;
        struct
        {   bool isComment : 1;
            bool isCommentBlock : 1;
        }   operatorInfo;
    };
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
#define ISASCII(c) (' ' <= (c) && (c) <= '~')

extern bool parseInitial (FILE* file, std::vector<Token>* list, Options* options);


