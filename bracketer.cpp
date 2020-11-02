#include "bracketer.hpp"
#include <cstdio>

bool bracketerValidate
(std::vector <Token> &list, std::vector <BracketOffence> &offenders)
{
    std::vector <Token> stack = {};

    for (auto &token : list)
    {
        if ((token.tokenClass & TK_CLASS_MASK) == TK_CLASS_BRACKET
        &&  (token.tokenClass & TKP_HAS_LEFT))
        {
            stack.push_back (token);
        }
        else
        if ((token.tokenClass & TK_CLASS_BRACKET)
        &&  (token.tokenClass & TKP_HAS_RIGHT))
        {
            if (stack.empty ())
            {
                BracketOffence offence = {};
                offence.number = 1;
                offence.tokens[0] = token;
                offenders.push_back (offence);
            }
            else
            if ((stack.back().tokenClass ^ TKP_BRACKET_CLASS_MASK)
            ==  token.tokenClass)
            {
                BracketOffence offence;
                offence.number = 2;
                offence.tokens[0] = stack.back ();
                offence.tokens[1] = token;
                offenders.push_back (offence);

                stack.pop_back ();
            }
            else
            {
                stack.pop_back ();
            }
        }
        else
        if ((token.tokenClass & TK_CLASS_MASK) == TK_CLASS_OPERATOR)
        {
            if (token.raw == "?")
            {
                token.tokenClass = TK_BRACKET_OPEN_TERNARY;
                stack.push_back (token);
            }
            else
            if (token.raw == ":" && !stack.empty ())
            {
                if (stack.back().tokenClass == TK_BRACKET_OPEN_TERNARY)
                {
                    token.tokenClass = TK_BRACKET_CLOSE_TERNARY;
                    stack.pop_back ();
                }
            }
        }
    }

    // any remaining stack entries must be offenders
    for (auto token : stack)
    {
        BracketOffence offence = {};
        offence.number = 1;
        offence.tokens[0] = token;
        offenders.push_back (offence);
    }

    return offenders.empty ();
}



Token bracketer (std::vector <Token> &list)
{
    Token root = {};
    root.tokenClass = TK_CONTEXT_FILE;

    std::vector<Token*> stack = {&root};

    for (auto token : list)
    {
        if ((token.tokenClass & TK_CLASS_MASK) == TK_CLASS_BRACKET
        &&  (token.tokenClass & TKP_HAS_LEFT))
        {
            Token child = {};
            child.tokenClass = token.tokenClass | TKP_HAS_RIGHT;
            child.line = token.line;
            child.column = token.column;

            stack.back ()->subtokens.push_back (child);
            // add a pointer to the new token into the stack
            stack.push_back (&stack.back ()->subtokens.back ());
        }

        stack.back ()->subtokens.push_back (token);

        if ((token.tokenClass & TK_CLASS_MASK) == TK_CLASS_BRACKET
        &&  (token.tokenClass & TKP_HAS_RIGHT))
        {
            stack.pop_back ();
        }
    }

    return root;
}

