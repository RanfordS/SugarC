#include "parse_header.hpp"
#include <cstdio>



TokenClass classifyInitial (char c)
{
    if ISWHITE(c)
        return TK_NONE;
    if ISNUMBER(c)
        return TK_LITERAL_NUMBER;
    if ISLETTER(c)
        return TK_NOUN_GENERIC;
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
    (void) c;
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
    (void) c;
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
    (void) c;
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
    (void) c;
    //TODO: handle `\\` sequences
    size_t size = token.raw.size ();
    char secondLast = '\\';
    char last = token.raw[size-1];
    if (size > 1)
        secondLast = token.raw[size-2];

    return secondLast == '\\' || last != '\"';
}

#define OPERATORS2_COUNT 19
std::string operators2[OPERATORS2_COUNT] =
{   "++", "--"
,   "+=", "-=", "*=", "/=", "%="
,   "&=", "|=", "^="
,   "==", "!=", "<=", ">="
,   "&&", "||", "^^"
,   "<<", ">>"
};

#define OPERATORS3_COUNT 5
std::string operators3[OPERATORS3_COUNT] =
{   "&&=", "||=", "^^="
,   "<<=", ">>="
};

bool continueOperator (char c, Token &token)
{
    std::string newraw = token.raw;
    newraw.push_back (c);

    if (newraw.size () == 2)
    {
        if (newraw == "//")
        {   token.tClass = TK_COMMENT_LINE;
            return true;
        }
        if (newraw == "/*")
        {   token.tClass = TK_COMMENT_BLOCK;
            return true;
        }

        for (size_t i = 0; i < OPERATORS2_COUNT; ++i)
            if (newraw == operators2[i])
                return true;
    }
    else if (newraw.size () == 3)
        for (size_t i = 0; i < OPERATORS3_COUNT; ++i)
            if (newraw == operators3[i])
                return true;

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
        case TK_NOUN_GENERIC:
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



/* number parsing
 * plain ints: 0 1 99
 * binary: 0b0 0B01101010
 * octal: 001 0777lu
 * hex: 0x0 0XBEEF
 * plain float: 0.0 1.0f
 * exponential: 0e0 10.2E-3f
 *                        ^ must be int
 */

enum NumTokenClass
{   NTK_LEADING_ZERO
,   NTK_NUMBER
,   NTK_INT_SIGNED
,   NTK_INT_UNSIGNED
,   NTK_INT_SPECIFIED_SIGNED
,   NTK_INT_SPECIFIED_UNSIGNED
,   NTK_BINARY
,   NTK_OCTAL
,   NTK_HEXIDECIMAL
,   NTK_DOUBLE
,   NTK_FLOAT
,   NTK_EXPONENTIAL_DOUBLE
,   NTK_EXPONENTIAL_FLOAT
};

struct NumToken
{
    NumTokenClass tClass;
    std::string raw;
};

void intparser (NumToken* token, char c)
{
    if (token->raw.empty ())
    {
        token->tClass = c == '0' ? NTK_LEADING_ZERO : NTK_NUMBER;
    }
    else
    {
        switch (token->tClass)
        {
            case NTK_LEADING_ZERO:
                if (c == 'b' || c == 'B')
                    token->tClass = NTK_BINARY;
                if (c == 'x' || c == 'X')
                    token->tClass = NTK_HEXIDECIMAL;
                if ('0' <= c && c <= '7')
                    token->tClass = NTK_OCTAL;
                if (c == '.')
                    token->tClass = NTK_DOUBLE;
                /* no match -> error */
                break;

            case NTK_NUMBER:
                if (c == '.')
                    token->tClass = NTK_DOUBLE;
                if ('0' <= c && c <= '9')
                    token->tClass = NTK_NUMBER; // no change
                if (c == 'e' || c == 'E')
                    token->tClass = NTK_EXPONENTIAL_DOUBLE;
                /* no match -> error */
                break;

            case NTK_INT_SIGNED:
                if ('0' <= c && c <= '9')
                    token->tClass = NTK_INT_SPECIFIED_SIGNED;
                /* no match -> error */
                break;

            case NTK_INT_UNSIGNED:
                if ('0' <= c && c <= '9')
                    token->tClass = NTK_INT_SPECIFIED_UNSIGNED;
                /* no match -> error */
                break;

            case NTK_INT_SPECIFIED_SIGNED:
                if ('0' <= c && c <= '9')
                    token->tClass = NTK_INT_SPECIFIED_SIGNED; // no change
                /* no match -> error */
                break;

            case NTK_INT_SPECIFIED_UNSIGNED:
                if ('0' <= c && c <= '9')
                    token->tClass = NTK_INT_SPECIFIED_UNSIGNED; // no change
                /* no match -> error */
                break;

            case NTK_BINARY:
                if (c == '0' || c == '1')
                    token->tClass = NTK_BINARY; // no change
                /* no match -> error */
                break;

            case NTK_OCTAL:
                if ('0' <= c && c <= '7')
                    token->tClass = NTK_OCTAL; // no change
                /* no match -> error */
                break;

            case NTK_HEXIDECIMAL:
                if (('0' <= c && c <= '7') || ('0' <= c && c <= '7'))
                    token->tClass = NTK_HEXIDECIMAL; // no change
                /* no match -> error */
                break;

            case NTK_DOUBLE:
                if (c == 'f' || c == 'F')
                    token->tClass = NTK_FLOAT;
                if (c == 'e' || c == 'E')
                    token->tClass = NTK_EXPONENTIAL_DOUBLE;
                if ('0' <= c && c <= '9')
                    token->tClass = NTK_DOUBLE; // no change
                /* no match -> error */
                break;

            case NTK_FLOAT:
                // number should be finished by this point
                /* invalid state -> error */
                break;

            case NTK_EXPONENTIAL_DOUBLE:
                if (c == '-' || c == '+')
                {   char p = token->raw[token->raw.size () - 1];
                    if (p == 'e' || p == 'E')
                        token->tClass = NTK_EXPONENTIAL_DOUBLE; // no change
                    /* no match -> error */
                }
                if ('0' <= c && c <= '9')
                    token->tClass = NTK_EXPONENTIAL_DOUBLE; // no change
                /* no match -> error */
                break;

            case NTK_EXPONENTIAL_FLOAT:
                // number should be finished by this point
                /* invalid state -> error */
                break;
        }
    }

    token->raw.push_back (c);
}



bool isstandardtype (std::string raw)
{
    if (raw == "Bool"
    ||  raw == "Char"
    ||  raw == "Byte"
    ||  raw == "Int"
    ||  raw == "UInt"
    ||  raw == "Size"
    ||  raw == "Half"
    ||  raw == "Float"
    ||  raw == "Double"
    ||  raw == "Void"
    ||  raw == "Function"
    ||  raw == "Enum"
    ||  raw == "BitField")
        return true;

    int diff;

    diff = raw.compare ("Int");
    if (diff > 0 && raw.size () == 3 - diff)
    {
        for (size_t i = 3; i < raw.size (); ++i)
            if (raw[i] < '0' || '9' < raw[i])
                return false;
        return true;
    }

    diff = raw.compare ("UInt");
    if (diff > 0 && raw.size () == 4 - diff)
    {
        for (size_t i = 4; i < raw.size (); ++i)
            if (raw[i] < '0' || '9' < raw[i])
                return false;
        return true;
    }
}























