#include "contextualizer.hpp"
#include "tools.hpp"

/* header seeking
 *
 * includes/imports:
 * Include
 *
 * type definitions:
 * Struct: Noun {}
 * Enum: Noun {}
 * BitField: Noun {}
 *
 * function declarations:
 * Function Type: Noun ();
 * Prefix Type: Noun ();
 * Infix Type: () Noun ();
 * Suffix Type: () Noun;
 */

/* root seeking
 *
 * header seekings
 *
 * function definitions:
 * Function Type: Noun () {}
 * Prefix Type: Noun () {}
 * Infix Type: () Noun () {}
 * Suffix Type: () Noun {}
 *
 * variable declarations:
 * Type: Noun (= ConstExpression) (, Noun (= ConstExpression))*;
 */

/* struct seeking
 *
 * member declaration:
 * Type: Noun (, Noun)*;
 */

/* enum/bitfield seeking
 *
 * member declaration:
 * Noun (= ConstExpression) (, Noun (= ConstExpression))* (,)
 */

/* scope seeking
 *
 * variable declaration:
 * Type: Noun;
 * Type: Noun = Expression;
 *
 * statement:
 * Expression;
 *
 * branching:
 * Keyword () Statement
 * Keyword () {}
 *
 * [while|for] () [Statement|{}]
 * do [Statement|{}] while ();
 * switch () {}
 * if () [Statement|{}] (else [Statement|{}])
 */

// prefix:
// i-2 i-1 i+0 i+1 i+2
//  ?   ?  pfx exp  ?
//  ?   ?  cmp  ?
//     <--     -->
// infix:
// i-2 i-1 i+0 i+1 i+2
//  ?  exp ifx exp  ?
//  ?  cmp  ?
// <--     -->
// suffix:
// i-2 i-1 i+0 i+1 i+2
//  ?  exp sfx  ?   ?
//  ?  cmp  ?   ?
// <--     -->

bool contextsubparse (Token &root, size_t round, size_t i)
{
    if ((root.subtokens[i].tokenClass & TK_CLASS_MASK)
    !=  TK_CLASS_OPERATOR) return false;

    uint8_t type = inbuildRounds[round].type;
    bool has_left = type & TKP_HAS_LEFT;
    bool has_right = type & TKP_HAS_RIGHT;

    for (auto name : inbuildRounds[round].operators)
    {
        if (name == root.subtokens[i].raw) goto label_match_found;
    }
    return false;
label_match_found:

    // TODO: does not skip past comments
    /*
    if (has_left)
        if ((root.subtokens[i-1].tokenClass & TKP_CONTEXT_CLASS_MASK)
        == TKP_CONTEXT_CLASS_EXP)
            return false;
    if (has_right)
        if ((root.subtokens[i+1].tokenClass & TKP_CONTEXT_CLASS_MASK)
        == TKP_CONTEXT_CLASS_EXP)
            return false;
    */

    // check for possible non-unary variant of the operator
    if (!has_left && i > 0)
    {
        for (auto alt_round : inbuildRounds)
        {
            if ((alt_round.type & TKP_SIDE_MASK)
            ==  (TKP_HAS_LEFT | TKP_HAS_RIGHT))
            {
                for (auto name : alt_round.operators)
                {
                    if (name == root.subtokens[i].raw)
                        return false;
                }
            }
        }
    }

    Token expression = {};
    expression.line = root.subtokens[i].line;
    expression.column = root.subtokens[i].column;
    expression.tokenClass = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_EXP
                          | TKP_CONTEXT_FUNCTION | (type & TKP_SIDE_MASK);

    groupTokens (root.subtokens, i - has_left, has_left + 1 + has_right, expression);

    return true;
}

bool contextualizer (Token &root)
{
    bool success = true;

    // do the rounds
    size_t round = 0;
    while (round < inbuildRounds.size ())
    {
        uint8_t type = inbuildRounds[round].type;
        bool has_left = type & TKP_HAS_LEFT;
        bool has_right = type & TKP_HAS_RIGHT;

        // ensure there are enough tokens
        if (has_left + (size_t)1 + has_right > root.subtokens.size ())
        {
            ++round;
            continue;
        }

        if (type & TKP_PARSE_RIGHT_TO_LEFT)
        {
            size_t i = root.subtokens.size () - 1 - has_right;
            while (i - has_left < (size_t)(-2))
            {
                bool match = contextsubparse (root, round, i);
                if (match && has_left) --i;
                --i;
            }
        }
        else
        {
            size_t i = 0 + has_left;
            while (i + has_right < root.subtokens.size ())
            {
                bool match = contextsubparse (root, round, i);
                if (!match || !has_left) ++i;
            }
        }

        ++round;
    }

    // do the rounds on any child blocks
    for (auto &child : root.subtokens)
    {
        if ((child.tokenClass & TK_CLASS_MASK) == TK_CLASS_BRACKET)
            contextualizer (child);
    }

    return success;
}

bool rootlevelContextualizer (Token &root)
{
    bool success = true;
    for (auto &token : root.subtokens)
    {
        if ((token.tokenClass & (TK_CLASS_MASK | TKP_CONTEXT_CLASS_MASK | TKP_CONTEXT_FUNCTION))
        ==  (TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEF | TKP_CONTEXT_FUNCTION))
        {
            success = success && contextualizer (token.subtokens.back ());
        }
    }
    return success;
}




struct DefinitionVariable
{
    std::string name;
    Token declaration;
};

struct DefintionFunction
{
    std::string name;
    std::vector<Token> arg_types;
    Token return_type;
};

struct DefinitionPrefix
{
    std::string name;
    Token arg_type;
};

struct DefinitionInfix
{
    Token left_arg_type;
    std::string name;
    Token right_arg_type;
};

struct DefinitionSuffix
{
    Token arg_type;
    std::string name;
};



