#include "rootlevel.hpp"
#include "tools.hpp"

/* type-seeker ?
 *
 * everything before a colon operator is a type reference unless it
 *
 * starting from the end of the scope, seek to a colon at `i`
 * next token `i-1` is first
 *
 * invalidated by `label`, `case`, and `default`
 */

/* finds the following
 *
 * ~@> includes
 * include "file";
 * include "file" as <noun>;
 *
 * ~@> type definitions
 * Struct : <noun> {}
 * Enum : <noun> {}
 * Bitfield : <noun> {}
 *
 * ~@> function declarations
 * Function <noun>+ : <noun> () ;
 * Prefix <noun>+ : <noun|op> () ;
 * Infix <noun>+ : () <noun|op> () ;
 * Suffix <noun>+ : () <noun|op> ;
 *
 * ~@> function definitions
 * Function <noun>+ : <noun> () {}
 * Prefix <noun>+ : <noun|op> () {}
 * Infix <noun>+ : () <noun|op> () {}
 * Suffix <noun>+ : () <noun|op> {}
 *
 * ~@> variable declarations
 * <noun>+ : <noun> ;
 */

enum RootLevelState
{   RLS_NONE

,   RLS_INCLUDE_KEY // 1
,   RLS_INCLUDE_FILE // 2
,   RLS_INCLUDE // 3 end
,   RLS_INCLUDE_AS_KEY // 3
,   RLS_INCLUDE_AS_NAME // 4
,   RLS_INCLUDE_AS // 5 end

,   RLS_TYPEDEF_TYPE // 1
,   RLS_TYPEDEF_COLON // 2
,   RLS_TYPEDEF_NAME // 3
,   RLS_TYPEDEF // 4 end

,   RLS_FUNCTION_TYPE // 1 <->
,   RLS_FUNCTION_COLON // 2
,   RLS_FUNCTION_NAME // 3
,   RLS_FUNCTION_BRACKET // 4
,   RLS_FUNCTION_DEC // 5 end
,   RLS_FUNCTION_DEF // 5 end

,   RLS_PREFIX_TYPE // 1 <->
,   RLS_PREFIX_COLON // 2
,   RLS_PREFIX_NAME // 3
,   RLS_PREFIX_RIGHT // 4
,   RLS_PREFIX_DEC // 5 end
,   RLS_PREFIX_DEF // 5 end

,   RLS_SUFFIX_TYPE // 1 <->
,   RLS_SUFFIX_COLON // 2
,   RLS_SUFFIX_LEFT // 3
,   RLS_SUFFIX_NAME // 4
,   RLS_SUFFIX_DEC // 5 end
,   RLS_SUFFIX_DEF // 5 end

,   RLS_INFIX_TYPE // 1 <->
,   RLS_INFIX_COLON // 2
,   RLS_INFIX_LEFT // 3
,   RLS_INFIX_NAME // 4
,   RLS_INFIX_RIGHT // 5
,   RLS_INFIX_DEC // 6 end
,   RLS_INFIX_DEF // 6 end

,   RLS_VARIABLE_TYPE // 1 <->
,   RLS_VARIABLE_COLON // 2
,   RLS_VARIABLE // 3 end
};

//#define DBG_ROOTLEVEL

#ifdef DBG_ROOTLEVEL

std::string RootLevelStateName[] =
{   "RLS_NONE"

,   "RLS_INCLUDE_KEY" // 1
,   "RLS_INCLUDE_FILE" // 2
,   "RLS_INCLUDE" // 3 end
,   "RLS_INCLUDE_AS_KEY" // 3
,   "RLS_INCLUDE_AS_NAME" // 4
,   "RLS_INCLUDE_AS" // 5 end

,   "RLS_TYPEDEF_TYPE" // 1
,   "RLS_TYPEDEF_COLON" // 2
,   "RLS_TYPEDEF_NAME" // 3
,   "RLS_TYPEDEF" // 4 end

,   "RLS_FUNCTION_TYPE" // 1 <->
,   "RLS_FUNCTION_COLON" // 2
,   "RLS_FUNCTION_NAME" // 3
,   "RLS_FUNCTION_BRACKET" // 4
,   "RLS_FUNCTION_DEC" // 5 end
,   "RLS_FUNCTION_DEF" // 5 end

,   "RLS_PREFIX_TYPE" // 1 <->
,   "RLS_PREFIX_COLON" // 2
,   "RLS_PREFIX_NAME" // 3
,   "RLS_PREFIX_RIGHT" // 4
,   "RLS_PREFIX_DEC" // 5 end
,   "RLS_PREFIX_DEF" // 5 end

,   "RLS_SUFFIX_TYPE" // 1 <->
,   "RLS_SUFFIX_COLON" // 2
,   "RLS_SUFFIX_LEFT" // 3
,   "RLS_SUFFIX_NAME" // 4
,   "RLS_SUFFIX_DEC" // 5 end
,   "RLS_SUFFIX_DEF" // 5 end

,   "RLS_INFIX_TYPE" // 1 <->
,   "RLS_INFIX_COLON" // 2
,   "RLS_INFIX_LEFT" // 3
,   "RLS_INFIX_NAME" // 4
,   "RLS_INFIX_RIGHT" // 5
,   "RLS_INFIX_DEC" // 6 end
,   "RLS_INFIX_DEF" // 6 end

,   "RLS_VARIABLE_TYPE" // 1 <->
,   "RLS_VARIABLE_COLON" // 2
,   "RLS_VARIABLE" // 3 end
};

#endif

bool rootlevel (Token &root, Token &newroot)
{
    bool success = true;

    uint8_t state = RLS_NONE;

    newroot = {};
    newroot.tokenClass = TK_CONTEXT_FILE;
    Token context = {};

    for (auto token : root.subtokens)
    {
        if ((token.tokenClass & TK_CLASS_MASK) == TK_CLASS_COMMENT)
        {
            newroot.subtokens.push_back (token);
            continue;
        }

#ifdef DBG_ROOTLEVEL
        std::printf ("Current state: %s\n", RootLevelStateName[state].data ());
        std::printf ("Input token: %s\n", getTokenName (token.tokenClass).data ());
        if (!token.raw.empty()) std::printf ("Raw: %s\n", token.raw.data ());
#endif

        context.subtokens.push_back (token);

        switch (state)
        {
            case RLS_NONE:
            {
                if (token.tokenClass == TK_NOUN_KEYWORD
                &&  token.raw == "include")
                {
                    state = RLS_INCLUDE_KEY;
                }
                else
                if (token.tokenClass == TK_NOUN_TYPE)
                {
                    if (token.raw == "Struct"
                    ||  token.raw == "Enum"
                    ||  token.raw == "Bitfield")
                    {
                        state = RLS_TYPEDEF_TYPE;
                    }
                    else
                    if (token.raw == "Function")
                    {
                        state = RLS_FUNCTION_TYPE;
                    }
                    else
                    if (token.raw == "Prefix")
                    {
                        state = RLS_PREFIX_TYPE;
                    }
                    else
                    if (token.raw == "Suffix")
                    {
                        state = RLS_SUFFIX_TYPE;
                    }
                    else
                    if (token.raw == "Infix")
                    {
                        state = RLS_INFIX_TYPE;
                    }
                    else
                    {
                        state = RLS_VARIABLE_TYPE;
                    }
                }
                else
                {
                    state = RLS_VARIABLE_TYPE;
                }
                break;
            }

            case RLS_INCLUDE_KEY:
            {
                if (token.tokenClass != TK_LITERAL_STRING)
                {
                    return false;
                }
                state = RLS_INCLUDE_FILE;
                break;
            }

            case RLS_INCLUDE_FILE:
            {
                if (token.tokenClass != TK_CLASS_OPERATOR)
                {
                    return false;
                }

                if (token.raw == ";")
                {
                    state = RLS_INCLUDE;
                }
                else
                if (token.raw == "->")
                {
                    state = RLS_INCLUDE_AS_KEY;
                }
                else
                {
                    return false;
                }
                break;
            }

            // require single noun
            case RLS_INCLUDE_AS_KEY:
            case RLS_TYPEDEF_COLON:
            case RLS_FUNCTION_COLON:
            {
                if (token.tokenClass != TK_NOUN)
                {
                    return false;
                }
                ++state;
                break;
            }

            // requires single noun or operator
            case RLS_PREFIX_COLON:
            case RLS_SUFFIX_LEFT:
            case RLS_INFIX_LEFT:
            {
                if (token.tokenClass != TK_NOUN
                &&  token.tokenClass != TK_OPERATOR)
                {
                    return false;
                }
                ++state;
                break;
            }

            // semicolon next
            case RLS_INCLUDE_AS_NAME:
            {
                if (token.tokenClass != TK_CLASS_OPERATOR || token.raw != ";")
                {
                    return false;
                }
                ++state;
                break;
            }

            // colon next
            case RLS_TYPEDEF_TYPE:
            {
                if (token.tokenClass != TK_CLASS_OPERATOR || token.raw != ":")
                {
                    return false;
                }
                ++state;
                break;
            }

            // any til colon
            case RLS_FUNCTION_TYPE:
            case RLS_PREFIX_TYPE:
            case RLS_INFIX_TYPE:
            case RLS_SUFFIX_TYPE:
            case RLS_VARIABLE_TYPE:
            {
                if (token.tokenClass == TK_CLASS_OPERATOR && token.raw == ":")
                {
                    // collect tokens into composite type
                    Token type = {};
                    type.tokenClass = TK_CONTEXT_DECLARATION_TYPE;
                    groupTokens
                    (   context.subtokens
                    ,   0
                    ,   context.subtokens.size () - 1
                    ,   type
                    );
                    type.line = type.subtokens[0].line;
                    type.column = type.subtokens[0].column;

                    ++state;
                }
                break;
            }

            // var till semicolon
            case RLS_VARIABLE_COLON:
            {
                if (token.tokenClass == TK_CLASS_OPERATOR && token.raw == ";")
                {
                    ++state;
                }
                break;
            }

            // curly bracket next
            case RLS_TYPEDEF_NAME:
            {
                if (token.tokenClass != TK_BRACKET_BLOCK_CURLY)
                {
                    return false;
                }
                ++state;
                break;
            }

            // round bracket next
            case RLS_FUNCTION_NAME:
            {
                if (token.tokenClass != TK_BRACKET_BLOCK_ROUND)
                {
                    return false;
                }
                ++state;
                break;
            }

            // round bracket or noun
            case RLS_PREFIX_NAME:
            case RLS_INFIX_COLON:
            case RLS_INFIX_NAME:
            case RLS_SUFFIX_COLON:
            {
                if (token.tokenClass != TK_BRACKET_BLOCK_ROUND
                &&  token.tokenClass != TK_NOUN)
                {
                    return false;
                }
                ++state;
                break;
            }

/* finds the following
 *
 * ~@> includes
 * include "file";
 * include "file" as <noun>;
 *
 * ~@> type definitions
 * Struct : <noun> {}
 * Enum : <noun> {}
 * Bitfield : <noun> {}
 *
 * ~@> function declarations
 * Function <noun>+ : <noun> () ;
 * Prefix <noun>+ : <noun|op> () ;
 * Infix <noun>+ : () <noun|op> () ;
 * Suffix <noun>+ : () <noun|op> ;
 *
 * ~@> function definitions
 * Function <noun>+ : <noun> () {}
 * Prefix <noun>+ : <noun|op> () {}
 * Infix <noun>+ : () <noun|op> () {}
 * Suffix <noun>+ : () <noun|op> {}
 *
 * ~@> variable declarations
 * <noun>+ : <noun> ;
 */

            // def or dec
            case RLS_FUNCTION_BRACKET:
            case RLS_PREFIX_RIGHT:
            case RLS_INFIX_RIGHT:
            case RLS_SUFFIX_NAME:
            {
                if (token.tokenClass == TK_OPERATOR && token.raw == ";")
                {
                    state += 1;
                }
                else
                if (token.tokenClass == TK_BRACKET_BLOCK_CURLY)
                {
                    state += 2;
                }
                else
                {
                    return false;
                }
                break;
            }

            default:
                // INVALID
                break;
        }

        bool complete = true;

        switch (state)
        {
            // invalid states
            case RLS_INCLUDE:
            case RLS_INCLUDE_AS:
                context.tokenClass = TK_CONTEXT_INCLUDE;
                break;

            case RLS_TYPEDEF:
                context.tokenClass = TK_CONTEXT_DEFINITION_TYPE;
                break;

            case RLS_FUNCTION_DEC:
                context.tokenClass = TK_CONTEXT_DECLARATION_CALL;
                break;

            case RLS_FUNCTION_DEF:
                context.tokenClass = TK_CONTEXT_DEFINITION_CALL;
                break;

            case RLS_PREFIX_DEC:
                context.tokenClass = TK_CONTEXT_DECLARATION_PREFIX;
                break;

            case RLS_PREFIX_DEF:
                context.tokenClass = TK_CONTEXT_DEFINITION_PREFIX;
                break;

            case RLS_SUFFIX_DEC:
                context.tokenClass = TK_CONTEXT_DECLARATION_SUFFIX;
                break;

            case RLS_SUFFIX_DEF:
                context.tokenClass = TK_CONTEXT_DEFINITION_SUFFIX;
                break;

            case RLS_INFIX_DEC:
                context.tokenClass = TK_CONTEXT_DECLARATION_INFIX;
                break;

            case RLS_INFIX_DEF:
                context.tokenClass = TK_CONTEXT_DEFINITION_INFIX;
                break;

            case RLS_VARIABLE:
                context.tokenClass = TK_CONTEXT_STATEMENT;
                break;

            default:
                complete = false;
                break;
        }

        if (complete)
        {
#ifdef DBG_ROOTLEVEL
            std::printf ("Push\n");
#endif
            newroot.subtokens.push_back (context);
            context = {};
            state = RLS_NONE;
        }

#ifdef DBG_ROOTLEVEL
        std::printf ("\n");
#endif
    }

    return success && context.subtokens.empty ();
}


















