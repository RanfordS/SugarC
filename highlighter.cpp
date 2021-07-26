#include "highlighter.hpp"
#include <cstdio>

static void recursor (Token &token)
{
    std::printf ("\033[%u;%uH", token.line, token.column + 4);
    uint8_t majorClass = token.tokenClass & TK_CLASS_MASK;

    // 1-bold, 3-italics, 4-underline, 5-blink, 7-inverted
    // 31-red, 32-green, 33-yellow, 34-blue, 35-purple, 36-teal, 37-white

    if (majorClass == TK_CLASS_COMMENT)
    {   if (token.tokenClass & TKP_COMMENT_DOCUMENT)
        {   std::printf ("\033[0;3;37m");
        }
        else
        {   std::printf ("\033[0;3;31m");
        }
    }
    else
    if (majorClass == TK_CLASS_LITERAL)
    {   std::printf ("\033[0;32m");
    }
    else
    if (majorClass == TK_CLASS_OPERATOR)
    {   std::printf ("\033[0;1;34m");
    }
    else
    if (majorClass == TK_CLASS_BRACKET)
    {   std::printf ("\033[0;34m");
    }
    else
    if (majorClass == TK_CLASS_NUMBER)
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

    /*if (majorClass == TK_CLASS_BRACKET
    &&  (token.tokenClass & TKP_SIDE_MASK) == (TKP_HAS_LEFT | TKP_HAS_RIGHT))
    {   std::printf ("\033[D\033[0;5;34m>");
    }
    else*/
    {   std::printf ("%s", token.raw.data ());
    }

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
