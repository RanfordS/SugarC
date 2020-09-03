#include "parse_header.hpp"

bool checkBrackets (std::vector<Token> &list)
{
    // stack of indicies
    std::vector<size_t> bracketStack = {};

    for (size_t i = 0; i < list.size (); ++i)
    {
        TokenClass current = list[i].tClass;

        if TK_ISBRACKET_OPEN(current)
        {
            bracketStack.push_back (i);
        }
        else if TK_ISBRACKET_CLOSE(current)
        {
            if (!bracketStack.size ())
            {
                std::printf ("l%lu,c%lu: `%s` has no matching bracket\n",
                        list[i].line, list[i].column, list[i].raw.data ());
                return false;
            }

            size_t j = bracketStack[bracketStack.size () - 1];
            TokenClass matching = list[j].tClass;
            if TK_ISBRACKET_MATCH(matching, current)
                bracketStack.pop_back ();
            else
            {   std::printf ("l%lu,c%lu: `%s` does not match `%s` (l%lu,c%lu)\n",
                        list[i].line, list[i].column, list[i].raw.data (),
                        list[j].raw.data (), list[j].line, list[j].column);
                return false;
            }
        }
    }

    if (!bracketStack.empty ())
    {
        size_t count = bracketStack.size ();
        std::printf ("unmatched bracket count: %lu\n", count);
        for (size_t i = 0; i < count; ++i)
        {
            size_t j = bracketStack[i];
            std::printf ("l%lu,c%lu: `%s`\n",
                    list[j].line, list[j].column, list[j].raw.data ());
        }
    }

    return true;
}

// assumes initial has been subjected to checkBrackets
void parseBrackets (std::vector<Token> &initial, Token* fileScope)
{
    std::vector<Token*> bracketStack = {fileScope};
    Token* currentScope = fileScope;

    for (size_t i = 0; i < initial.size (); ++i)
    {
        TokenClass tClass = initial[i].tClass;

        if TK_ISBRACKET_CLOSE(tClass)
        {
            bracketStack.pop_back ();
            currentScope = bracketStack[bracketStack.size () - 1];
        }
        else
        {
            currentScope->subTokens.push_back (initial[i]);

            if TK_ISBRACKET_OPEN(tClass)
            {
                Token* newScope =
                &currentScope->subTokens[currentScope->subTokens.size () - 1];
                bracketStack.push_back (newScope);
                newScope->tClass = TK_BRACKET_OPEN_TO_BLOCK(tClass);
                currentScope = newScope;
            }
        }
    }
}


