#include "alternate.hpp"

bool bracketsvalidator
(std::vector <Token> &list, std::vector <BracketOffence> &offenders)
{
    std::vector <Token> stack = {};

    for (auto token : list)
    {
        if (TK_ISBRACKET_OPEN(token.tokenClass))
        {
            stack.push_back (token);
        }
        else
        if (TK_ISBRACKET_CLOSE(token.tokenClass))
        {
            if (stack.empty ())
            {
                BracketOffence offence = {};
                offence.number = 1;
                offence.tokens[0] = token;
                offenders.push_back (offence);
            }
            else
            if (!TK_ISBRACKET_MATCHING(stack.back().tokenClass,
                                              token.tokenClass))
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



Token bracket (std::vector <Token> &list)
{
    Token root = {};
    root.tokenClass = TK_CONTEXT_FILE;

    std::vector<Token*> stack = {&root};

    for (auto token : list)
    {
        if (TK_ISBRACKET_OPEN(token.tokenClass))
        {
            Token child = {};
            child.tokenClass = TK_BRACKET_OPEN_TO_BLOCK(token.tokenClass);
            child.line = token.line;
            child.column = token.column;

            stack.back ()->subtokens.push_back (child);
            // add a pointer to the new token into the stack
            stack.push_back (&stack.back ()->subtokens.back ());
        }

        stack.back ()->subtokens.push_back (token);

        if (TK_ISBRACKET_CLOSE(token.tokenClass))
        {
            stack.pop_back ();
        }
    }

    return root;
}

