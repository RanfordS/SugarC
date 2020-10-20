#include "alternate.hpp"
#include <cstdio>

static void recursor (Token &token)
{
    std::printf ("\033[%lu;%luH", token.line, token.column + 4);
    //
    // 1-bold, 3-italics, 4-underline, 5-blink, 7-inverted
    // 31-red, 32-green, 33-yellow, 34-blue, 35-purple, 36-teal

    if (token.tokenClass == TK_COMMENT_LINE
    ||  token.tokenClass == TK_COMMENT_BLOCK)
    {   std::printf ("\033[0;3;31m");
    }
    else
    if (token.tokenClass == TK_LITERAL_CHAR
    ||  token.tokenClass == TK_LITERAL_STRING)
    {   std::printf ("\033[0;32m");
    }
    else
    if (token.tokenClass == TK_OPERATOR)
    {   std::printf ("\033[0;1;34m");
    }
    else
    if (TK_ISBRACKET(token.tokenClass))
    {   std::printf ("\033[0;34m");
    }
    else
    if (TK_ISNUMBER(token.tokenClass))
    {   std::printf ("\033[0;35m");
    }
    else
    if (token.tokenClass == TK_NOUN_KEYWORD)
    {   std::printf ("\033[0;1;36m");
    }
    else
    if (token.tokenClass == TK_NOUN_TYPE)
    {   std::printf ("\033[0;1;33m");
    }
    else
    if (token.tokenClass == TK_NOUN)
    {   std::printf ("\033[0;4m");
    }
    else
    {   std::printf ("\033[0m");
    }

    std::printf ("%s", token.raw.data ());

    for (auto child : token.subtokens)
        recursor (child);
}

void highlighter (std::vector<Token> &root)
{
    std::printf ("\033[?1049h");
    for (auto token : root)
        recursor (token);
    std::getchar ();
    std::printf ("\033[?1049l");
}
