#include <string>
#include <vector>

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





enum RevisedTokenClass
{   RTK_NONE
//  operator
,   RTK_OPERATOR
//  delimited
,   RTK_COMMENT_LINE
,   RTK_COMMENT_BLOCK
,   RTK_LITERAL_CHAR
,   RTK_LITERAL_STRING
//  brackets
,   RTK_BRACKET_OPEN_ROUND
,   RTK_BRACKET_OPEN_SQUARE
,   RTK_BRACKET_OPEN_CURLY
,   RTK_BRACKET_CLOSE_ROUND
,   RTK_BRACKET_CLOSE_SQUARE
,   RTK_BRACKET_CLOSE_CURLY
,   RTK_BRACKET_BLOCK_ROUND
,   RTK_BRACKET_BLOCK_SQUARE
,   RTK_BRACKET_BLOCK_CURLY
//  numbers
,   RTK_NUMBER
,   RTK_NUMBER_ZERO
,   RTK_NUMBER_INT_SIGNED
,   RTK_NUMBER_INT_UNSIGNED
,   RTK_NUMBER_INT_SPECIFIED_SIGNED
,   RTK_NUMBER_INT_SPECIFIED_UNSIGNED
,   RTK_NUMBER_BINARY
,   RTK_NUMBER_OCTAL
,   RTK_NUMBER_HEXIDECIMAL
,   RTK_NUMBER_DOUBLE
,   RTK_NUMBER_FLOAT
,   RTK_NUMBER_EXPONENTIAL_DOUBLE
,   RTK_NUMBER_EXPONENTIAL_FLOAT
//  nouns
,   RTK_NOUN
,   RTK_NOUN_KEYWORD
,   RTK_NOUN_TYPE
,   RTK_NOUN_VARIABLE
//  contextual
,   RTK_CONTEXT_OPERATOR_PREFIX
,   RTK_CONTEXT_OPERATOR_INFIX
,   RTK_CONTEXT_OPERATOR_SUFFIX
,   RTK_CONTEXT_CAST
,   RTK_CONTEXT_STATEMENT
,   RTK_CONTEXT_SCOPE
};

#define RANGE(a,b,c) ((a) <= (b) && (b) <= (c))

#define RTK_ISDELIMITED(t) RANGE(RTK_COMMENT_LINE,t,RTK_LITERAL_STRING)

#define RTK_ISBRACKET(t) RANGE(RTK_BRACKET_OPEN_ROUND,t,RTK_BRACKET_BLOCK_CURLY)

#define RTK_ISNUMBER(t) RANGE(RTK_NUMBER,t,RTK_NUMBER_EXPONENTIAL_FLOAT)

#define RTK_ISNOUN(t) RANGE(RTK_NOUN,t,RTK_NOUN_VARIABLE)

#define CHAR_ISWHITESPACE(c) \
    ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')

#define CHAR_ISLETTER(c) \
    (RANGE('A',c,'Z') || RANGE('a',c,'z') || (c) == '_')

// ops + - = * / % & | ^ ! < > ~ @ ; : ?
// excludes .
#define CHAR_ISOPERATOR(c) ((c) == '+' || (c) == '-' || (c) == '=' || (c) == '*' || (c) == '/' || (c) == '%' || (c) == '&' || (c) == '|' || (c) == '^' || (c) == '!' || (c) == '<' || (c) == '>' || (c) == '~' || (c) == '@' || (c) == ';' || (c) == ':' || (c) == '?')

#define CHAR_ISVALID(c) RANGE(' ',c,'~')

const std::vector<std::string> operators
{   "++", "--"
,   "+=", "-=", "*=", "/=", "%="
,   "&=", "|=", "^="
,   "==", "!=", "<=", ">="
,   "&&", "||", "^^"
,   "<<", ">>"
,   "&&=", "||=", "^^="
,   "<<=", ">>="
};

bool stringescape (std::string text)
{
    size_t i = text.size ();
    bool state = false;

    while (i--)
    {
        if (text[i] != '\\') break;
        state = !state;
    }

    return state;
}

enum AdavanceReturns
{   AR_INVALID_CHAR
,   AR_INVALID_TOKEN
,   AR_CONTINUE
,   AR_FINISHED_CHAR2THIS
,   AR_FINISHED_CHAR2NEXT
,   AR_FINISHED_CHAR2NONE
};

struct RevisedToken
{
    RevisedTokenClass tokenClass;
    std::string raw;

    // returns:
    //  3 - token complete without error, char discarded (usually whitespace)
    //  2 - token complete without error, char adds to this
    //  1 - token complete without error, char adds to next
    //  0 - token can accept more characters, char adds to this
    // -1 - token invalidated, error
    // -2 - invalid char, error
    AdavanceReturns advance (char c)
    {
        if (!RTK_ISDELIMITED(tokenClass)
        &&  CHAR_ISWHITESPACE(c)) return AR_FINISHED_CHAR2NONE;

        switch (tokenClass)
        {
            case RTK_NONE:
            {
                if (!CHAR_ISVALID(c)) return AR_INVALID_CHAR;

                if (CHAR_ISLETTER(c))
                    tokenClass = RTK_NOUN;
                else
                if (c == '0')
                    tokenClass = RTK_NUMBER_ZERO;
                else
                if (RANGE('1',c,'9'))
                    tokenClass = RTK_NUMBER;
                else
                switch (c)
                {
                    case '\'':
                        tokenClass = RTK_LITERAL_CHAR;
                        break;

                    case '"':
                        tokenClass = RTK_LITERAL_STRING;
                        break;

                    case '(':
                        tokenClass = RTK_BRACKET_OPEN_ROUND;
                        return AR_FINISHED_CHAR2THIS;
                        break;

                    case ')':
                        tokenClass = RTK_BRACKET_CLOSE_ROUND;
                        return AR_FINISHED_CHAR2THIS;
                        break;

                    case '[':
                        tokenClass = RTK_BRACKET_OPEN_SQUARE;
                        return AR_FINISHED_CHAR2THIS;
                        break;

                    case ']':
                        tokenClass = RTK_BRACKET_CLOSE_SQUARE;
                        return AR_FINISHED_CHAR2THIS;
                        break;

                    case '{':
                        tokenClass = RTK_BRACKET_OPEN_CURLY;
                        return AR_FINISHED_CHAR2THIS;
                        break;

                    case '}':
                        tokenClass = RTK_BRACKET_CLOSE_CURLY;
                        return AR_FINISHED_CHAR2THIS;
                        break;

                    default:
                        tokenClass = RTK_OPERATOR;
                        break;
                }
                return AR_CONTINUE;
                break;
            }

            // operator

            case RTK_OPERATOR:
            {
                std::string preview = raw;
                raw.push_back (c);

                if (raw == "//")
                {   tokenClass = RTK_COMMENT_LINE;
                    return AR_CONTINUE;
                }

                if (raw == "/*")
                {   tokenClass = RTK_COMMENT_BLOCK;
                    return AR_CONTINUE;
                }

                for (auto name : operators)
                {   if (name == preview) return AR_CONTINUE;
                }

                return AR_FINISHED_CHAR2NEXT;
                break;
            }

            // delimited

            case RTK_COMMENT_LINE:
            {
                return c == '\n' ? AR_FINISHED_CHAR2THIS : AR_CONTINUE;
                break;
            }

            case RTK_COMMENT_BLOCK:
            {
                if (raw.size () > 3)
                {
                    const char* end = raw.data () + raw.size () - 2;
                    std::string close = "*/";
                    if (end == close) return AR_FINISHED_CHAR2THIS;
                }
                return AR_CONTINUE;
            }

            case RTK_LITERAL_CHAR:
            {
                if (c > '~') return AR_INVALID_CHAR;

                if (c == '\'' && !stringescape (raw))
                {
                    if (!RANGE(3,raw.size (),4))
                        return AR_INVALID_TOKEN;
                    return AR_FINISHED_CHAR2THIS;
                }
            }

            // number

            case RTK_NUMBER_ZERO:
            {
                if (c == 'b' || c == 'B')
                {   tokenClass = RTK_NUMBER_BINARY;
                    return AR_CONTINUE;
                }
                if (c == 'x' || c == 'X')
                {   tokenClass = RTK_NUMBER_HEXIDECIMAL;
                    return AR_CONTINUE;
                }
                if (RANGE('0',c,'7'))
                {   tokenClass = RTK_NUMBER_OCTAL;
                    return AR_CONTINUE;
                }
                if (c == '.')
                {   tokenClass = RTK_NUMBER_DOUBLE;
                    return AR_CONTINUE;
                }
                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER:
            {
                if (RANGE('0',c,'9')) return AR_CONTINUE;

                if (c == '.')
                {   tokenClass = RTK_NUMBER_DOUBLE;
                    return AR_CONTINUE;
                }
                if (c == 'e' || c == 'E')
                {   tokenClass = RTK_NUMBER_EXPONENTIAL_DOUBLE;
                    return AR_CONTINUE;
                }
                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_INT_SIGNED:
            {
                if (RANGE('0',c,'9'))
                {   tokenClass = RTK_NUMBER_INT_SPECIFIED_SIGNED;
                    return AR_CONTINUE;
                }

                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_INT_UNSIGNED:
            {
                if (RANGE('0',c,'9'))
                {   tokenClass = RTK_NUMBER_INT_SPECIFIED_UNSIGNED;
                    return AR_CONTINUE;
                }

                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_INT_SPECIFIED_SIGNED:
            {
                if (RANGE('0',c,'9')) return AR_CONTINUE;

                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_INT_SPECIFIED_UNSIGNED:
            {
                if (RANGE('0',c,'9')) return AR_CONTINUE;

                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_BINARY:
            {
                if (c == '0' || c == '1') return AR_CONTINUE;

                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_OCTAL:
            {
                if (RANGE('0',c,'7')) return AR_CONTINUE;

                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_HEXIDECIMAL:
            {
                if (RANGE('0',c,'9')
                ||  RANGE('a',c,'f')
                ||  RANGE('A',c,'F')) return AR_CONTINUE;

                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_DOUBLE:
            {
                if (RANGE('0',c,'9')) return AR_CONTINUE;

                if (c == 'f' || c == 'F')
                {   tokenClass = RTK_NUMBER_FLOAT;
                    return AR_FINISHED_CHAR2THIS;
                }
                if (c == 'e' || c == 'E')
                {   tokenClass = RTK_NUMBER_EXPONENTIAL_DOUBLE;
                    return AR_CONTINUE;
                }

                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_FLOAT:
            {
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_EXPONENTIAL_DOUBLE:
            {
                if (RANGE('0',c,'9')) return AR_CONTINUE;

                if (c == '-' || c == '+')
                {   char p = raw[raw.size () - 1];
                    if (p == 'e' || p == 'E') return AR_CONTINUE;
                    return AR_FINISHED_CHAR2NEXT;
                }
                if (c == 'f' || c == 'F')
                {   tokenClass = RTK_NUMBER_EXPONENTIAL_FLOAT;
                    return AR_FINISHED_CHAR2THIS;
                }

                if (CHAR_ISOPERATOR(c)) return AR_FINISHED_CHAR2NEXT;
                return AR_INVALID_TOKEN;
                break;
            }

            case RTK_NUMBER_EXPONENTIAL_FLOAT:
            {
                return AR_INVALID_TOKEN;
                break;
            }

            // noun

            case RTK_NOUN:
            {
                if (CHAR_ISLETTER(c) || RANGE('0',c,'9')) return AR_CONTINUE;
                return AR_FINISHED_CHAR2NEXT;
                break;
            }
        }
    }
};



std::vector<std::string> keywords =
{   "switch"
,   "case"
,   "default"
,   "break"
,   "do"
,   "while"
,   "for"
,   "continue"
,   "if"
,   "else"
,   "return"
};

void identifykeywords (std::vector<RevisedToken> &list)
{
    for (auto token : list)
        if (token.tokenClass == RTK_NOUN)
            for (auto keyword : keywords)
                if (token.raw == keyword)
                    token.tokenClass = RTK_NOUN_KEYWORD;
}



bool alternateparse (FILE* file, std::vector<RevisedToken> &list)
{
    bool success = true;

    RevisedToken token = {};
    size_t line = 1;
    size_t column = 0;
    uint8_t tab = 4;

    int i = ~EOF;
    while (i != EOF)
    {
        i = fgetc (file);
        char c = i == EOF ? '\n' : i;

        switch (c)
        {
            case '\n':
                column = 0;
                ++line;
                break;

            case '\t':
                column = (column/tab)*tab;
                column += tab;
                break;

            default:
                ++column;
                break;
        }

rerun:
        switch (token.advance (c))
        {
            case AR_INVALID_CHAR:
            case AR_INVALID_TOKEN:
                success = false;
                token.raw.push_back (c);
                list.push_back (token);
                token = {};
                break;

            case AR_CONTINUE:
                token.raw.push_back (c);
                break;

            case AR_FINISHED_CHAR2THIS:
                token.raw.push_back (c);
            case AR_FINISHED_CHAR2NONE:
                list.push_back (token);
                token = {};
                break;

            case AR_FINISHED_CHAR2NEXT:
                list.push_back (token);
                token = {};
                goto rerun;
                break;
        }
    }

    return true;
}






