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

bool contextOnToken (const OperatorRound &round, Token &block, size_t i)
{
    Token &token = block.subtokens[i];

    bool needsLeft  = round.type & TKP_HAS_LEFT;
    bool needsRight = round.type & TKP_HAS_RIGHT;

    if ((round.type & TKP_SIDE_MASK) == ORT_LTR_TERNARY)
    {
        needsLeft  = true;
        needsRight = true;
    }

    bool hasLeft = i > 0;
    bool hasRight = i < block.subtokens.size () - 1;
    if (hasLeft)  hasLeft  = tokenIsExpression (block.subtokens[i-1].tokenClass);
    if (hasRight) hasRight = tokenIsExpression (block.subtokens[i+1].tokenClass);

    // seek for alternate match
    if ((!needsLeft  && hasLeft)
    ||  (!needsRight && hasRight))
        for (auto altRound : inbuildRounds)
            if ((altRound.type & TKP_SIDE_MASK) == TKP_SIDE_MASK)
                for (auto name : altRound.operators)
                    if (name == token.raw)
                        return false;

    Token expression = {};

    if ((token.tokenClass & TK_CLASS_MASK) == TK_CLASS_OPERATOR)
    {
        bool match = false;
        for (auto name : round.operators)
        {
            if (name == token.raw)
            {
                expression.tokenClass
                    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_EXP
                    | TKP_CONTEXT_FUNCTION | (round.type & TKP_SIDE_MASK);
                match = true;
                break;
            }
        }
        if (!match)
            return false;
    }
    else
    {
        switch (round.type & TKP_PARSE_MASK)
        {
            case TKP_PARSE_SUFFIX_INDEX_CALL:
                if (token.tokenClass == TK_BRACKET_BLOCK_ROUND
                ||  token.tokenClass == TK_BRACKET_BLOCK_SQUARE)
                {
                    expression.tokenClass = TK_CONTEXT_EXPRESSION_CALL;
                }
                else
                /*if (custom suffix)
                {
                    expression.tokenClass = TK_CONTEXT_EXPRESSION_SUFFIX;
                }*/
                return false;
                break;

            case TKP_PARSE_PREFIX:
                /*if (custom prefix)
                {
                    expression.tokenClass = TK_CONTEXT_EXPRESSION_PREFIX;
                }*/
                return false;
                break;

            case TKP_PARSE_INFIX:
                /*if (custom prefix)
                {
                    expression.tokenClass = TK_CONTEXT_EXPRESSION_INFIX;
                }*/
                return false;
                break;

            case TKP_PARSE_TERNARY:
                if (token.tokenClass == TK_BRACKET_BLOCK_TERNARY)
                {
                    expression.tokenClass = TK_CONTEXT_EXPRESSION_TERNARY;
                }
                return false;
                break;

            default:
                return false;
        }
    }

    groupTokens
    (   block.subtokens
    ,   i - needsLeft
    ,   needsLeft + 1 + needsRight
    ,   expression
    );
    return true;
}

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

bool contextBlock (Token &block)
{
    bool success = true;
    for (auto &round : inbuildRounds)
    {
        bool needsLeft  = round.type & TKP_HAS_LEFT;
        bool needsRight = round.type & TKP_HAS_RIGHT;

        if ((round.type & TKP_SIDE_MASK) == ORT_LTR_TERNARY)
        {
            needsLeft  = true;
            needsRight = true;
        }

        size_t minimumCount = needsLeft + 1 + needsRight;
        if (block.subtokens.size () < minimumCount) continue;

        if (round.type & TKP_PARSE_RIGHT_TO_LEFT)
        {
            size_t i = block.subtokens.size () - 1 - needsRight;

            while (i - needsLeft < (size_t)(-2))
            {
                bool match = contextOnToken (round, block, i);
                i -= 1 + (match && needsLeft);
            }
        }
        else
        {
            size_t i = needsLeft;
            while (i + needsRight < block.subtokens.size ())
            {
                bool match = contextOnToken (round, block, i);
                i += !(match && needsLeft);
            }
        }
    }
    for (auto &child : block.subtokens)
    {
        if ((child.tokenClass & TK_CLASS_MASK) == TK_CLASS_BRACKET)
            success &= contextBlock (child);
    }
    return success;
}

bool contextualizer (Token &root)
{
    const uint8_t mask =
        TK_CLASS_MASK    | TKP_CONTEXT_CLASS_MASK | TKP_CONTEXT_FUNCTION;
    const uint8_t targ =
        TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEF  | TKP_CONTEXT_FUNCTION;

    bool success = true;
    for (Token &token : root.subtokens)
    {
        if ((token.tokenClass & mask) == targ
        ||   token.tokenClass == TK_CONTEXT_STATEMENT)
        {
            success &= contextBlock (token.subtokens.back ());
        }
    }
    return success;
}
