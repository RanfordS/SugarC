#include "data.hpp"

/*
static size_t seekdepth (Token &token)
{
    size_t i = 0;

    for (auto &child : token.subtokens)
    {
        size_t j = seekdepth (child);
        if (j > i) j = i;
    }

    return i + tokenIsExpression (token.tokenClass);
}
*/

static void recursor (Token &token)
{
    uint8_t tokenClass = token.tokenClass;
    uint8_t majorClass = tokenClass & TK_CLASS_MASK;

    int color = 0;
    int style = 0;

    size_t dispLine = 2*token.line - 1;
    size_t dispColumn = token.column;

    bool standard = true;

    switch (majorClass)
    {
        case TK_CLASS_COMMENT:
            color = 31; style = 3;
            break;

        case TK_CLASS_LITERAL:
            color = 32;
            break;

        case TK_CLASS_OPERATOR:
            color = 34; style = 1;
            break;

        case TK_CLASS_BRACKET:
            color = 34;
            break;

        case TK_CLASS_NUMBER:
            color = 35;
            break;

        case TK_CLASS_NOUN:
            switch (tokenClass)
            {
                case TK_NOUN_KEYWORD:
                    color = 36; style = 1;
                    break;

                case TK_NOUN_TYPE:
                    color = 33; style = 1;
                    break;

                case TK_NOUN:
                    color = 4;
                    break;
            }
            break;

        default:
            standard = false;
            break;
    }

    if (standard)
    {
        if (!token.raw.empty ())
        {
            std::printf ("\033[%lu;%luH", dispLine, dispColumn);

            if (style)
                std::printf ("\033[%i;%im", style, color);
            else
                std::printf ("\033[%im", color);

            //TODO: continue
            std::printf ("%s", "");
        }
    }
}
