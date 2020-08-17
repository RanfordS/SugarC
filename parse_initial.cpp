#include "parse_initial.hpp"
#include <cstdio>



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

bool continueCommentLine (char c, Token* token)
{
    if (token->delimitedInfo.last == '\n')
    {   token->raw.erase (token->raw.size () - 1, 1);
        return false;
    }
    token->delimitedInfo.last = c;
    return true;
}

bool continueCommentBlock (char c, Token* token)
{
    if (token->delimitedInfo.secondLast == '*'
    &&  token->delimitedInfo.last == '/')
    {   token->raw.erase (token->raw.size () - 2, 2);
        return false;
    }

    token->delimitedInfo.secondLast = token->delimitedInfo.last;
    token->delimitedInfo.last = c;
    return true;
}

bool continueChar (char c, Token* token)
{
    if (token->delimitedInfo.last == '\''
    &&  token->delimitedInfo.secondLast != '\\')
        return false;

    token->delimitedInfo.secondLast = token->delimitedInfo.last;
    token->delimitedInfo.last = c;
    return true;
}

bool continueString (char c, Token* token)
{
    if (token->delimitedInfo.last == '\"'
    &&  token->delimitedInfo.secondLast != '\\')
        return false;

    token->delimitedInfo.secondLast = token->delimitedInfo.last;
    token->delimitedInfo.last = c;
    return true;
}

bool continueOperator (char c, Token* token)
{
    if (token->raw[token->raw.size() - 1] == '/')
        switch (c)
        {   case '*': token->operatorInfo.isCommentBlock = true;
            case '/': token->operatorInfo.isComment = true;
            default: break;
        }
    return classifyInitial (c) == TOKEN_CLASS_OPERATOR;
}

bool continueNumber (char c, Token* token)
{
    (void) token;
    return ISLETTER(c) || ISNUMBER(c) || c == '-' || c == '.';
}

bool continueNoun (char c, Token* token)
{
    (void) token;
    return ISLETTER(c) || ISNUMBER(c);
}

bool continueCurrent (char c, Token* token)
{
    switch (token->tClass)
    {
        case TOKEN_CLASS_COMMENT_LINE:
            return continueCommentLine (c, token);
        case TOKEN_CLASS_COMMENT_BLOCK:
            return continueCommentBlock (c, token);
        case TOKEN_CLASS_CHAR:
            return continueChar (c, token);
        case TOKEN_CLASS_STRING:
            return continueString (c, token);
        case TOKEN_CLASS_OPERATOR:
            return continueOperator (c, token);
        case TOKEN_CLASS_NUMBER:
            return continueNumber (c, token);
        case TOKEN_CLASS_NOUN:
            return continueNoun (c, token);
        default:
            return true;
    }
}

// - //

bool parseInitial (FILE* file, std::vector<Token>* list, Options* options)
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
        if (current.tClass != TOKEN_CLASS_NONE)
        {
            if (continueCurrent (c, &current))
            {
                // handle comments
                if (current.tClass == TOKEN_CLASS_OPERATOR
                &&  current.operatorInfo.isComment)
                {   size_t size = current.raw.size ();
                    bool block = current.operatorInfo.isCommentBlock;
                    if (size > 1)
                    {   current.raw.erase (size - 1, 1);
                        current.operatorInfo = {};
                        list->push_back (current);
                    }
                    current = {};
                    current.line = line;
                    current.column = column - 1;

                    current.tClass = block ? TOKEN_CLASS_COMMENT_BLOCK
                                           : TOKEN_CLASS_COMMENT_LINE;
                }
                // standard behaviour
                else
                    current.raw.push_back (c);
            }
            else
            {   list->push_back (current);
                current = {};
                current.line = line;
                current.column = column;
            }
        }

        // handle EOF
        if (ic == EOF)
        {   if (current.tClass != TOKEN_CLASS_NONE)
            {   std::printf ("Delimited block was not closed before end of file\n");
                return false;
            }
            return true;
        }

        // identify initial of next token
        if (current.tClass == TOKEN_CLASS_NONE)
            if ((current.tClass = classifyInitial (c)) != TOKEN_CLASS_NONE)
            {   current.line = line;
                current.column = column;
                current.raw.push_back (c);
            }
    }
}
