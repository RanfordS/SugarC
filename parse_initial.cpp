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
        }   delimited;
        struct
        {   size_t positionDecimal;
            size_t positionExponential;
            size_t positionHexMarker;
            bool hasLeadingZero;
        }   number;
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

TokenClass classifyInitial (char c)
{
    if ISWHITE(c)
        return TOKEN_CLASS_NONE;
    if ISNUMBER(c)
        return TOKEN_CLASS_NUMBER;
    if ISLETTER(c)
        return TOKEN_CLASS_NOUN;
    if (c == '\'')
        return TOKEN_CLASS_CHAR;
    if (c == '"')
        return TOKEN_CLASS_STRING;
    if ISASCII(c)
        return TOKEN_CLASS_OPERATOR;
    return TOKEN_CLASS_UNKNOWN;
}

//

/*
 * continueTokenClass functions:
 *
 * All off the following function return false when the given
 * char is not part of the current token.
 *
 * Both Comment types will crop thier content strings before returning false,
 * hence omitting their respective closing sequences.
 *
 * Number engulfs a lot of things, this should be fine.
 */

bool continueCommentLine (char c, Token* token)
{
    if (token->delimited.last == '\n')
    {   token->raw.erase (token->raw.size () - 1, 1);
        return false;
    }
    token->delimited.last = c;
    return true;
}

bool continueCommentBlock (char c, Token* token)
{
    if (token->delimited.secondLast == '*'
    &&  token->delimited.last == '/')
    {   token->raw.erase (token->raw.size () - 2, 2);
        return false;
    }

    token->delimited.secondLast = token->delimited.last;
    token->delimited.last = c;
    return true;
}

bool continueChar (char c, Token* token)
{
    if (token->delimited.last == '\''
    &&  token->delimited.secondLast != '\\')
        return false;

    token->delimited.secondLast = token->delimited.last;
    token->delimited.last = c;
}

bool continueString (char c, Token* token)
{
    if (token->delimited.last == '\"'
    &&  token->delimited.secondLast != '\\')
        return false;

    token->delimited.secondLast = token->delimited.last;
    token->delimited.last = c;
}

bool continueOperator (char c, Token* token)
{
    return classifyInitial (c) == TOKEN_CLASS_OPERATOR;
}

bool continueNumber (char c, Token* token)
{
    return ISLETTER(c) || ISNUMBER(c) || c == '-' || c == '.';
}

bool continueNoun (char c, Token* token)
{
    return ISLETTER(c) || ISNUMBER(c);
}

//

bool parseInitial (FILE* file, std::vector<Token> list, Options* options)
{
    size_t line = 1;
    size_t column = 0;

    Token current = {};

    int c;

    while (true)
    {
        c = fgetc (file);
        switch (c)
        {
            case '\n':
                column = 0;
                ++line;
                break;

            case '\t':
            {   uint8_t tab = options->tabSize;
                if (!options->tabFixedSize)
                    column = (column/tab)*tab;
                column += tab;
            }   break;

            default:
                ++column;
                break;
        }


        if (c == EOF) break;
    }
}
