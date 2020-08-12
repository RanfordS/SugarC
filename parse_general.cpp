#include <cstdio>
#include <string>
#include "parse_general.hpp"

// takes char and returns the corresponding TokenClass
TokenClass classifyInitial (char c)
{
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        return TOKEN_CLASS_NONE;

    if (c < ' ' || '~' < c)
        return TOKEN_CLASS_UNKNOWN;

    if ('0' <= c && c <= '9')
        return TOKEN_CLASS_NUMBER;

    if (c == '\'')
        return TOKEN_CLASS_CHAR;

    if (c == '\"')
        return TOKEN_CLASS_STRING;

    if (('A' <= c && c <= 'Z')
    ||  ('a' <= c && c <= 'z')
    ||  (c == '_'))
        return TOKEN_CLASS_NOUN;

    return TOKEN_CLASS_OPERATOR;
}


bool parseGeneral (FILE* file, std::vector<TokenGeneral> &tokenList)
{
    size_t line = 1;
    size_t column = 0;
    
    TokenGeneral token = {};

    int c;

    while (true)
    {
        c = fgetc (file);
        if (c == '\n')
        {
            column = 0;
            ++line;
        }
        else if (c == '\t')
            column = column/4*4 + 4;
        else
            ++column;

        if (c == -1) break;
        if (!c) break;

        TokenClass tClassThis;

        switch (token.tClass)
        {
            default:
                //TODO: implement separate rules for continuing tokens
            case TOKEN_CLASS_NONE:
                tClassThis = classifyInitial (c);
                if (tClassThis == TOKEN_CLASS_UNKNOWN)
                {
                    return false;
                }
                break;
        }

        if (tClassThis != token.tClass && token.tClass != TOKEN_CLASS_NONE)
        {
            token.raw.push_back ('\0');
            tokenList.push_back (token);
            token = {};
        }
        if (tClassThis != TOKEN_CLASS_NONE)
        {
            if (token.tClass == TOKEN_CLASS_NONE)
            {
                token.line = line;
                token.column = column;
            }
            token.raw.push_back (c);
        }

        /*
        if (tClassThis != TOKEN_CLASS_NONE)
        {
            token.raw.push_back (c);
        }
        // non-empty token to pass
        else if (token.tClass != TOKEN_CLASS_NONE)
        {
            std::printf ("pushing token type %i\n", token.tClass);
            std::printf ("token raw |%s|\n", token.raw.data ());
        }
        */
        token.tClass = tClassThis;
    }

    return true;
}
