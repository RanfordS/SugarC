#include "alternate.hpp"

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

enum OperatorType
{   OT_LTR_SUFFIX
,   OT_LTR_PREFIX
,   OT_LTR_INFIX
,   OT_LTR_TERNARY
,   OT_RTL_SUFFIX
,   OT_RTL_PREFIX
,   OT_RTL_INFIX
,   OT_RTL_TERNARY
};

#define OT_ISLTR(t) RANGE(OT_LTR_SUFFIX,t,OT_LTR_TERNARY)
#define OT_ISRTL(t) RANGE(OT_RTL_SUFFIX,t,OT_RTL_TERNARY)
#define OT_HASLEFT(t) \
    ((t) == OT_LTR_SUFFIX || (t) == OT_LTR_INFIX || \
     (t) == OT_RTL_SUFFIX || (t) == OT_RTL_INFIX)
#define OT_HASRIGHT(t) \
    ((t) == OT_LTR_PREFIX || (t) == OT_LTR_INFIX || \
     (t) == OT_RTL_PREFIX || (t) == OT_RTL_INFIX)

struct OperatorRound
{   OperatorType type;
    std::vector <std::string> operators;
};

const std::vector <OperatorRound> inbuildRounds
{   {   OT_LTR_INFIX,
    {  "."     // member access
    }}
,   {   OT_LTR_SUFFIX,
    {   "++"    // postincrement
    ,   "--"    // postdecrement
    //  array index and function call should act like these
    //  custom suffix functions should act like these
    }}
,   {   OT_RTL_PREFIX,
    {   "!"     // logical not
    ,   "~"     // bitwise not
    ,   "&"     // address of (reference) / pointer declaration
    ,   "@"     // at address (dereference)
    ,   "++"    // preincrement
    ,   "--"    // predecrement
    //  custom prefix functions should act like these
    }}
,   {   OT_LTR_PREFIX,
    {   "+"     // unary plus
    ,   "-"     // unary minus
    }}
/*  a custom pow-like operator would have to go here
,   {   OT_RTL_INFIX,
    {   "pow"   // power
    }}
 */
,   {   OT_LTR_INFIX,
    {   "*"  	// multiply
    ,   "/"  	// divide
    ,   "%"  	// modulus
    //  custom infix functions should act like these
    }}
,   {   OT_LTR_INFIX,
    {   "+"  	// add
    ,   "-"  	// subtract
    }}
,   {   OT_LTR_INFIX,
    {   "<<" 	// shift up
    ,   ">>" 	// shift down
    }}
,   {   OT_LTR_INFIX,
    {   "<"  	// less than
    ,   "<=" 	// less than or equal to
    ,   ">"  	// greater than
    ,   ">=" 	// greater than or equal to
    }}
,   {   OT_LTR_INFIX,
    {   "=="    // equality
    ,   "!="    // inquality
    }}
,   {   OT_LTR_INFIX,
    {   "&"     // bitwise and
    }}
,   {   OT_LTR_INFIX,
    {   "^"     // bitwise xor
    }}
,   {   OT_LTR_INFIX,
    {   "|"     // bitwise or
    }}
,   {   OT_LTR_INFIX,
    {   "&&"    // logical and
    }}
,   {   OT_LTR_INFIX,
    {   "^^"    // logical xor
    }}
,   {   OT_LTR_INFIX,
    {   "||"    // logical or
    }}
,   {   OT_LTR_INFIX,
    {   "->"    // for range
    }}
/*  ternary operator goes here, but doesn't match the typical pattern
,   {   OT_RTL_TERNARY
    {   "?:"    // ternary selector
    }}
 */
,   {   OT_RTL_INFIX,
    {   "="     // assign
    ,   "+="    // add assign
    ,   "-="    // subtract assign
    ,   "*="    // multiply assign
    ,   "/="    // divide assign
    ,   "%="    // modulus assign
    ,   "<<="   // shift up assign
    ,   ">>="   // shift down assign
    ,   "&="    // bitwise and assign
    ,   "^="    // bitwise xor assign
    ,   "|="    // bitwise or assign
    ,   "&&="   // logical and assign
    ,   "^^="   // logical xor assign
    ,   "||="   // logical or assign
    }}
/*  comma is handled as an operator in C, but may be handled elsewhere
,   {   OT_LTR_INFIX,
    {   ","     // separator
    }}
 */
};


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
    if (root.subtokens[i].tokenClass != TK_OPERATOR) return false;

    OperatorType type = inbuildRounds[round].type;
    bool has_left = OT_HASLEFT(type);
    bool has_right = OT_HASRIGHT(type);

    for (auto name : inbuildRounds[round].operators)
    {
        if (name == root.subtokens[i].raw) goto label_match_found;
    }
    return false;
label_match_found:

    // TODO: does not skip past comments
    if (has_left)
        if (!TK_ISEXPRESSION(root.subtokens[i-1].tokenClass))
            return false;
    if (has_right)
        if (!TK_ISEXPRESSION(root.subtokens[i+1].tokenClass))
            return false;

    // check for possible non-unary variant of the operator
    if (!has_left && i > 0)
    {
        for (auto alt_round : inbuildRounds)
        {
            if (alt_round.type == OT_LTR_INFIX
            ||  alt_round.type == OT_RTL_INFIX)
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
    switch (type)
    {
        case OT_LTR_PREFIX:
        case OT_RTL_PREFIX:
            expression.tokenClass = TK_CONTEXT_EXPRESSION_PREFIX;
            expression.subtokens.push_back (root.subtokens[i]);
            expression.subtokens.push_back (root.subtokens[i+1]);
            break;

        case OT_LTR_INFIX:
        case OT_RTL_INFIX:
            expression.tokenClass = TK_CONTEXT_EXPRESSION_INFIX;
            expression.subtokens.push_back (root.subtokens[i-1]);
            expression.subtokens.push_back (root.subtokens[i]);
            expression.subtokens.push_back (root.subtokens[i+1]);
            break;

        case OT_LTR_SUFFIX:
        case OT_RTL_SUFFIX:
            expression.tokenClass = TK_CONTEXT_EXPRESSION_SUFFIX;
            expression.subtokens.push_back (root.subtokens[i-1]);
            expression.subtokens.push_back (root.subtokens[i]);
            break;

        default:
            return false;
            break;
    }

    root.subtokens.erase
        (root.subtokens.begin () + i - has_left,
         root.subtokens.begin () + i + 1 + has_right);
    root.subtokens.emplace (root.subtokens.begin () + i - has_left, expression);

    return true;
}

bool contextparse (Token &root)
{
    bool success = true;

    // do the rounds
    size_t round = 0;
    while (round < inbuildRounds.size ())
    {
        OperatorType type = inbuildRounds[round].type;
        bool has_left = OT_HASLEFT(type);
        bool has_right = OT_HASRIGHT(type);

        // ensure there are enough tokens
        if (has_left + (size_t)1 + has_right > root.subtokens.size ())
        {
            ++round;
            continue;
        }

        if (OT_ISLTR(type))
        {
            size_t i = 0 + has_left;
            while (i + has_right < root.subtokens.size ())
            {
                bool match = contextsubparse (root, round, i);
                if (!match || !has_left) ++i;
            }
        }
        else
        {
            size_t i = root.subtokens.size () - 1 - has_right;
            while (i - has_left < (size_t)(-2))
            {
                bool match = contextsubparse (root, round, i);
                if (match && has_left) --i;
                --i;
            }
        }

        ++round;
    }

    // do the rounds on any child blocks
    for (auto &child : root.subtokens)
    {
        if (TK_ISBRACKET_BLOCK(child.tokenClass)) contextparse (child);
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



