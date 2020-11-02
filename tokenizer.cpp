#include "tokenizer.hpp"

bool isstandardtype (std::string raw)
{
    for (auto type : inbuiltTypes)
    {
        if (raw == type) return true;
    }

    int diff;

    for (auto type : inbuiltVariableTypes)
    {
        diff = raw.compare (type);
        if (diff > 0 && raw.size () + diff == type.size ())
        {
            for (size_t i = type.size (); i < raw.size (); ++i)
                if (!RANGE('0',raw[i],'9'))
                    return false;
            return false;
        }
    }

    return false;
}

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



enum AdvanceReturns
{   AR_INVALID_CHAR
,   AR_INVALID_TOKEN
,   AR_CONTINUE
,   AR_FINISHED_CHAR2THIS
,   AR_FINISHED_CHAR2NEXT
,   AR_FINISHED_CHAR2NONE
};

AdvanceReturns tokenAdvance (Token &token, char c)
{
    if ((token.tokenClass & TK_CLASS_MASK) != TK_CLASS_COMMENT
    &&  (token.tokenClass & TK_CLASS_MASK) != TK_CLASS_LITERAL
    &&  charIsWhitespace (c)) return AR_FINISHED_CHAR2NONE;

    switch (token.tokenClass)
    {
        case TK_NONE:
        {
            if (!charIsValid (c)) return AR_INVALID_CHAR;

            if (charIsLetter (c))
                token.tokenClass = TK_NOUN;
            else
            if (c == '0')
                token.tokenClass = TK_NUMBER_ZERO;
            else
            if (RANGE('1',c,'9'))
                token.tokenClass = TK_NUMBER;
            else
            switch (c)
            {
                case '\'':
                    token.tokenClass = TK_LITERAL_CHAR;
                    break;

                case '"':
                    token.tokenClass = TK_LITERAL_STRING;
                    break;

                case '(':
                    token.tokenClass = TK_BRACKET_OPEN_ROUND;
                    return AR_FINISHED_CHAR2THIS;
                    break;

                case ')':
                    token.tokenClass = TK_BRACKET_CLOSE_ROUND;
                    return AR_FINISHED_CHAR2THIS;
                    break;

                case '[':
                    token.tokenClass = TK_BRACKET_OPEN_SQUARE;
                    return AR_FINISHED_CHAR2THIS;
                    break;

                case ']':
                    token.tokenClass = TK_BRACKET_CLOSE_SQUARE;
                    return AR_FINISHED_CHAR2THIS;
                    break;

                case '{':
                    token.tokenClass = TK_BRACKET_OPEN_CURLY;
                    return AR_FINISHED_CHAR2THIS;
                    break;

                case '}':
                    token.tokenClass = TK_BRACKET_CLOSE_CURLY;
                    return AR_FINISHED_CHAR2THIS;
                    break;

                default:
                    token.tokenClass = TK_OPERATOR;
                    break;
            }
            return AR_CONTINUE;
            break;
        }

        // operator

        case TK_OPERATOR:
        {
            std::string preview = token.raw;
            preview.push_back (c);

            if (preview == "//")
            {   token.tokenClass = TK_COMMENT_LINE;
                return AR_CONTINUE;
            }

            if (preview == "/*")
            {   token.tokenClass = TK_COMMENT_BLOCK;
                return AR_CONTINUE;
            }

            for (auto name : operators)
            {   if (name == preview) return AR_CONTINUE;
            }

            return AR_FINISHED_CHAR2NEXT;
            break;
        }

        // delimited

        case TK_COMMENT_LINE:
        {
            return c == '\n' ? AR_FINISHED_CHAR2THIS : AR_CONTINUE;
            break;
        }

        case TK_COMMENT_BLOCK:
        {
            if (token.raw.size () > 3)
            {
                const char* end = token.raw.data () + token.raw.size () - 2;
                std::string close = "*/";
                if (end == close) return AR_FINISHED_CHAR2THIS;
            }
            return AR_CONTINUE;
        }

        case TK_LITERAL_CHAR:
        {
            if (c > '~') return AR_INVALID_CHAR;

            if (c == '\'' && !stringescape (token.raw))
            {
                if (!RANGE(3,token.raw.size (),4))
                    return AR_INVALID_TOKEN;
                return AR_FINISHED_CHAR2THIS;
            }
        }

        case TK_LITERAL_STRING:
        {
            if (c == '"' && !stringescape (token.raw))
                return AR_FINISHED_CHAR2THIS;
            return AR_CONTINUE;
            break;
        }

        // number

        case TK_NUMBER_ZERO:
        {
            if (c == 'b' || c == 'B')
            {   token.tokenClass = TK_NUMBER_BINARY;
                return AR_CONTINUE;
            }
            if (c == 'x' || c == 'X')
            {   token.tokenClass = TK_NUMBER_HEXADECIMAL;
                return AR_CONTINUE;
            }
            if (RANGE('0',c,'7'))
            {   token.tokenClass = TK_NUMBER_OCTAL;
                return AR_CONTINUE;
            }
            if (c == '.')
            {   token.tokenClass = TK_NUMBER_DOUBLE;
                return AR_CONTINUE;
            }
            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        case TK_NUMBER:
        {
            if (RANGE('0',c,'9')) return AR_CONTINUE;

            if (c == '.')
            {   token.tokenClass = TK_NUMBER_DOUBLE;
                return AR_CONTINUE;
            }
            if (c == 'e' || c == 'E')
            {   token.tokenClass = TK_NUMBER_EXPONENTIAL_DOUBLE;
                return AR_CONTINUE;
            }
            if (c == 'i' || c == 'I')
            {   token.tokenClass = TK_NUMBER_INT_SIGNED;
                return AR_CONTINUE;
            }
            if (c == 'u' || c == 'U')
            {   token.tokenClass = TK_NUMBER_INT_UNSIGNED;
                return AR_CONTINUE;
            }
            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        case TK_NUMBER_INT_SIGNED:
        {
            if (RANGE('0',c,'9'))
            {   token.tokenClass = TK_NUMBER_INT_SPECIFIED_SIGNED;
                return AR_CONTINUE;
            }

            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        case TK_NUMBER_INT_UNSIGNED:
        {
            if (RANGE('0',c,'9'))
            {   token.tokenClass = TK_NUMBER_INT_SPECIFIED_UNSIGNED;
                return AR_CONTINUE;
            }

            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        case TK_NUMBER_INT_SPECIFIED_SIGNED:
        {
            if (RANGE('0',c,'9')) return AR_CONTINUE;

            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        case TK_NUMBER_INT_SPECIFIED_UNSIGNED:
        {
            if (RANGE('0',c,'9')) return AR_CONTINUE;

            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        case TK_NUMBER_BINARY:
        {
            if (c == '0' || c == '1') return AR_CONTINUE;

            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        case TK_NUMBER_OCTAL:
        {
            if (RANGE('0',c,'7')) return AR_CONTINUE;

            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        case TK_NUMBER_HEXADECIMAL:
        {
            if (RANGE('0',c,'9')
            ||  RANGE('a',c,'f')
            ||  RANGE('A',c,'F')) return AR_CONTINUE;

            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        case TK_NUMBER_DOUBLE:
        {
            if (RANGE('0',c,'9')) return AR_CONTINUE;

            if (c == 'f' || c == 'F')
            {   token.tokenClass = TK_NUMBER_FLOAT;
                return AR_FINISHED_CHAR2THIS;
            }
            if (c == 'e' || c == 'E')
            {   token.tokenClass = TK_NUMBER_EXPONENTIAL_DOUBLE;
                return AR_CONTINUE;
            }

            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        // unreachable TK_NUMBER_FLOAT

        case TK_NUMBER_EXPONENTIAL_DOUBLE:
        {
            if (RANGE('0',c,'9')) return AR_CONTINUE;

            if (c == '-' || c == '+')
            {   char p = token.raw.back ();
                if (p == 'e' || p == 'E') return AR_CONTINUE;
                return AR_FINISHED_CHAR2NEXT;
            }
            if (c == 'f' || c == 'F')
            {   token.tokenClass = TK_NUMBER_EXPONENTIAL_FLOAT;
                return AR_FINISHED_CHAR2THIS;
            }

            if (charIsOperator (c)) return AR_FINISHED_CHAR2NEXT;
            return AR_INVALID_TOKEN;
            break;
        }

        // unreachable TK_NUMBER_EXPONENTIAL_FLOAT

        // noun

        case TK_NOUN:
        {
            if (charIsLetter (c) || RANGE('0',c,'9')) return AR_CONTINUE;
            return AR_FINISHED_CHAR2NEXT;
            break;
        }

        // debug

        default:
        {
            std::printf ("PROGRAMMER ERROR IN %s\n", __FUNCTION__);
            std::printf ("Reached unhandled state: %i\n", token.tokenClass);
            return AR_INVALID_TOKEN;
            break;
        }
    }

    return AR_INVALID_TOKEN;
}



void identifykeywords (std::vector <Token> &list)
{
    for (auto &token : list)
        if (token.tokenClass == TK_NOUN)
        {
            for (auto keyword : keywords)
                if (token.raw == keyword)
                    token.tokenClass = TK_NOUN_KEYWORD;

            if (isstandardtype (token.raw))
                token.tokenClass = TK_NOUN_TYPE;
        }
}

bool tokenizer (FILE* file, std::vector <Token> &list)
{
    bool success = true;
    uint8_t tab = 4;

    Token token = {};
    size_t line = 1;
    size_t column = 0;

#define PUSH list.push_back (token)
#define PUSH_COND {if (token.tokenClass != TK_NONE) PUSH;}
#define RESET {token = {}; token.line = line; token.column = column;}

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

        if (token.tokenClass == TK_NONE) RESET;

rerun:
        switch (tokenAdvance (token, c))
        {
            case AR_INVALID_CHAR:
            case AR_INVALID_TOKEN:
                success = false;
                token.raw.push_back (c);
                PUSH; RESET;
                break;

            case AR_CONTINUE:
                token.raw.push_back (c);
                break;

            case AR_FINISHED_CHAR2THIS:
                token.raw.push_back (c);
                PUSH_COND; RESET;
                break;

            case AR_FINISHED_CHAR2NONE:
                PUSH_COND; RESET;
                break;

            case AR_FINISHED_CHAR2NEXT:
                PUSH_COND; RESET;
                goto rerun;
                break;
        }
    }

#undef RESET
#undef PUSH_COND
#undef PUSH

    if (token.tokenClass != TK_NONE) success = false;

    identifykeywords (list);

    return success;
}

