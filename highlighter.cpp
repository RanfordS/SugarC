#include "parse_header.hpp"

void subroutine (Token &root)
{
    for (size_t i = 0; i < root.subTokens.size (); ++i)
    {
        Token token = root.subTokens[i];

        if (token.tClass == TK_STATEMENT)
        {
            std::printf ("\033[%lu;%luH\033[36m>", token.line, token.column + 2);
        }

        if (!token.raw.empty ())
        {
            // move to position
            std::printf ("\033[%lu;%luH", token.line, token.column + 4);

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

                case TK_OPERATOR:
                    color = 33; // yellow
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
            std::printf ("%s", token.raw.c_str ());
        }

        if (!token.subTokens.empty ())
        {
            subroutine (token);
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
