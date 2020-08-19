#include "parse_header.hpp"
#include <cstdio>



TokenClass classifyInitial (char c)
{
    if ISWHITE(c)
        return TK_NONE;
    if ISNUMBER(c)
        return TK_LITERAL_NUMBER;
    if ISLETTER(c)
        return TK_GENERIC_NOUN;
    if (c == '\'')
        return TK_LITERAL_CHAR;
    if (c == '"')
        return TK_LITERAL_STRING;
    if (c == '(')
        return TK_BRACKET_ROUND_OPEN;
    if (c == ')')
        return TK_BRACKET_ROUND_CLOSE;
    if (c == '[')
        return TK_BRACKET_SQUARE_OPEN;
    if (c == ']')
        return TK_BRACKET_SQUARE_CLOSE;
    if (c == '{')
        return TK_BRACKET_CURLY_OPEN;
    if (c == '}')
        return TK_BRACKET_CURLY_CLOSE;
    if ISASCII(c)
        return TK_OPERATOR;
    return TK_INVALID;
}

// - //

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

bool continueCommentLine (char c, Token &token)
{
    if (token.raw.back () == '\n')
    {   // trim
        token.raw.erase (token.raw.size () - 1, 1);
        token.raw.erase (0, 2);
        return false;
    }
    return true;
}

bool continueCommentBlock (char c, Token &token)
{
    size_t size = token.raw.size ();
    if (size < 2) return true;

    char secondLast = token.raw[size-2];
    char last = token.raw[size-1];
    if (secondLast == '*'
    &&  last == '/')
    {   // trim
        token.raw.erase (size - 2, 2);
        token.raw.erase (0, 2);
        return false;
    }

    return true;
}

bool continueChar (char c, Token &token)
{
    //TODO: handle `\\` sequences
    size_t size = token.raw.size ();
    char secondLast = '\\';
    char last = token.raw[size-1];
    if (size > 1)
        secondLast = token.raw[size-2];

    return secondLast == '\\' || last != '\'';
}

bool continueString (char c, Token &token)
{
    //TODO: handle `\\` sequences
    size_t size = token.raw.size ();
    char secondLast = '\\';
    char last = token.raw[size-1];
    if (size > 1)
        secondLast = token.raw[size-2];

    return secondLast == '\\' || last != '\"';
}

bool continueOperator (char c, Token &token)
{
    if (token.raw[0] == '/')
    {
        if (c == '/')
        {   token.tClass = TK_COMMENT_LINE;
            return true;
        }
        if (c == '*')
        {   token.tClass = TK_COMMENT_BLOCK;
            return true;
        }
    }

    return false;
}

bool continueNumber (char c, Token &token)
{
    (void) token;
    return ISLETTER(c) || ISNUMBER(c) || c == '-' || c == '.';
}

bool continueNoun (char c, Token &token)
{
    (void) token;
    return ISLETTER(c) || ISNUMBER(c);
}

bool continueCurrent (char c, Token &token)
{
    switch (token.tClass)
    {
        case TK_COMMENT_LINE:
            return continueCommentLine (c, token);
        case TK_COMMENT_BLOCK:
            return continueCommentBlock (c, token);
        case TK_LITERAL_CHAR:
            return continueChar (c, token);
        case TK_LITERAL_STRING:
            return continueString (c, token);
        case TK_OPERATOR:
            return continueOperator (c, token);
        case TK_LITERAL_NUMBER:
            return continueNumber (c, token);
        case TK_GENERIC_NOUN:
            return continueNoun (c, token);
        default:
            return false;
    }
}

// - //

bool parseInitial (FILE* file, std::vector<Token> &list, Options* options)
{
    size_t line = 1;
    size_t column = 0;

    Token current = {};

    while (true)
    {
        int ic = fgetc (file);
        char c = ic == EOF ? '\n' : ic;

        // line and column metrics
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

        // continue
        if (current.tClass != TK_NONE)
        {
            if (continueCurrent (c, current))
                current.raw.push_back (c);
            else
            {   list.push_back (current);
                current = {};
                current.line = line;
                current.column = column;
            }
        }

        // handle EOF
        if (ic == EOF)
        {   if (current.tClass != TK_NONE)
            {   std::printf ("Delimited block was not closed before end of file\n");
                return false;
            }
            return true;
        }

        // identify initial of next token
        if (current.tClass == TK_NONE)
            if ((current.tClass = classifyInitial (c)) != TK_NONE)
            {   current.line = line;
                current.column = column;
                current.raw.push_back (c);
            }

        if (current.tClass == TK_INVALID)
        {   std::printf ("Unrecoginised character with code 0x%X at [%lu,%lu]\n",
                (uint8_t)c, line, column);
            return false;
        }
    }
}
