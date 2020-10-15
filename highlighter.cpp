#include "parse_header.hpp"

static void subroutine (Token &root)
{
    for (size_t i = 0; i < root.subTokens.size (); ++i)
    {
        Token token = root.subTokens[i];

        if (token.tClass == TK_STATEMENT)
        {
            std::printf ("\033[%lu;%luH\033[36m>", token.line, token.column + 3);
        }

        if (!token.raw.empty ())
        {
            // move to position
            std::printf ("\033[%lu;%luH", token.line, token.column + 4);

            // 1-bold, 3-italics, 4-underline, 5-blink, 7-inverted
            // 31-red, 32-green, 33-yellow, 34-blue, 35-purple, 36-teal
            // select format
            int form = 0;
            int color = 0;
            switch (token.tClass)
            {
                case TK_LITERAL_CHAR:
                case TK_LITERAL_STRING:
                    color = 32; // green
                    break;

                case TK_COMMENT_LINE:
                case TK_COMMENT_BLOCK:
                    form = 3; // italics
                    color = 34; // blue
                    break;

                case TK_LITERAL_NUMBER:
                    color = 35; // purple
                    break;

                case TK_NOUN_TYPE:
                    form = 1; // bold
                case TK_OPERATOR:
                    color = 33; // yellow
                    break;

                case TK_NOUN_GENERIC:
                    form = 4;
                    break;

                default:
                    form = 0; // none
                    break;
            }
            if (color)
                std::printf ("\033[0;%i;%im", form, color);
            else
                std::printf ("\033[0;%im", form);

            // write token
            if (token.tClass == TK_COMMENT_LINE)
                std::printf ("//");
            if (token.tClass == TK_COMMENT_BLOCK)
                std::printf ("/*");
            std::printf ("%s", token.raw.c_str ());
        }

        if (!token.subTokens.empty ())
        {
            subroutine (token);

            if TK_ISBRACKET (token.tClass)
            {
                char c = '!';
                switch (token.tClass)
                {
                    case TK_BRACKET_ROUND_BLOCK:
                        c = ')';
                        break;
                    case TK_BRACKET_SQUARE_BLOCK:
                        c = ']';
                        break;
                    case TK_BRACKET_CURLY_BLOCK:
                        c = '}';
                        break;
                    default:
                        break;
                }
                std::printf ("\033[0m%c", c);
            }
        }
    }
}

void highlighter (Token &root)
{
    std::printf ("\033[?1049h");
    subroutine (root);
    std::getchar ();
    std::printf ("\033[?1049l");
}
