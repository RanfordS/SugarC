#include "parse_header.hpp"

void parseStatements (Token* srcScope, Token* dstScope)
{
    Token statement;
    bool newstatement = true;
    bool maybedefinition = false;

    for (size_t i = 0; i < srcScope->subTokens.size (); ++i)
    {
        Token token = srcScope->subTokens[i];
        TokenClass tClass = token.tClass;

        if (tClass == TK_COMMENT_LINE
        ||  tClass == TK_COMMENT_BLOCK)
        {
            dstScope->subTokens.push_back (token);
        }
        else
        {
            if (newstatement)
            {
                statement = {};
                statement.tClass = TK_STATEMENT;
                statement.line = srcScope->subTokens[i].line;
                statement.column = srcScope->subTokens[i].column;
                newstatement = false;
            }

            statement.subTokens.push_back (token);

            if (tClass == TK_BRACKET_CURLY_BLOCK)
            {
                parseStatements (&srcScope->subTokens[i],
                    &statement.subTokens[statement.subTokens.size () - 1]);
            }

            if (tClass == TK_BRACKET_ROUND_BLOCK)
                maybedefinition = true;
            else
            {   if ((tClass == TK_BRACKET_CURLY_BLOCK && maybedefinition)
                ||  (tClass == TK_OPERATOR && token.raw == ";"))
                    newstatement = true;
                maybedefinition = false;
            }

            if (newstatement)
            {
                dstScope->subTokens.push_back (statement);
            }
        }
    }
}
