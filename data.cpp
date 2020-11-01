#include "data.hpp"

const std::vector <std::string> inbuiltTypes
{   "Bool"
,   "Char"
,   "Byte"
,   "Int"
,   "UInt"
,   "Size"
,   "Half"
,   "Float"
,   "Double"
,   "Void"
,   "Function"
,   "Prefix"
,   "Infix"
,   "Suffix"
,   "Enum"
,   "BitField"
,   "Struct"
,   "Union"
};

const std::vector <std::string> inbuiltVariableTypes
{   "Int"
,   "UInt"
};

const std::vector <std::string> modifiers
{   "Volatile"
,   "Restricted"
,   "Align8"
,   "Align16"
,   "Align32"
,   "Align64"
,   "Const"
,   "Atomic"
,   "Extern"
,   "Inline"
,   "Sugar"
,   "Array"
};

const std::vector <std::string> keywords
{   "switch"
,   "case"
,   "default"
,   "break"
,   "do"
,   "while"
,   "for"
,   "continue"
,   "if"
,   "else"
,   "return"
,   "goto"
,   "label"
};

const std::vector <std::string> operators
{   "++", "--"
,   "+=", "-=", "*=", "/=", "%="
,   "&=", "|=", "^="
,   "==", "!=", "<=", ">="
,   "&&", "||", "^^"
,   "<<", ">>"
,   "&&=", "||=", "^^="
,   "<<=", ">>="
,   "->"
};

const std::vector <OperatorRound> inbuildRounds
{   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {  "."     // member access
    }}
,   {   ORT_LTR_SUFFIX | TKP_PARSE_SUFFIX_INDEX_CALL,
    {   "++"    // postincrement
    ,   "--"    // postdecrement
    //  array index and function call should act like these
    //  custom suffix functions should act like these
    }}
,   {   ORT_RTL_PREFIX | TKP_PARSE_PREFIX,
    {   "!"     // logical not
    ,   "~"     // bitwise not
    ,   "&"     // address of (reference) / pointer declaration
    ,   "@"     // at address (dereference)
    ,   "++"    // preincrement
    ,   "--"    // predecrement
    //  custom prefix functions should act like these
    }}
,   {   ORT_LTR_PREFIX | TKP_PARSE_SIMPLE,
    {   "+"     // unary plus
    ,   "-"     // unary minus
    }}
/*  a custom pow-like operator would have to go here
,   {   OT_RTL_INFIX,
    {   "pow"   // power
    }}
 */
,   {   ORT_LTR_INFIX | TKP_PARSE_INFIX,
    {   "*"  	// multiply
    ,   "/"  	// divide
    ,   "%"  	// modulus
    //  custom infix functions should act like these
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "+"  	// add
    ,   "-"  	// subtract
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "<<" 	// shift up
    ,   ">>" 	// shift down
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "<"  	// less than
    ,   "<=" 	// less than or equal to
    ,   ">"  	// greater than
    ,   ">=" 	// greater than or equal to
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "=="    // equality
    ,   "!="    // inquality
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "&"     // bitwise and
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "^"     // bitwise xor
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "|"     // bitwise or
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "&&"    // logical and
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "^^"    // logical xor
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "||"    // logical or
    }}
,   {   ORT_LTR_INFIX | TKP_PARSE_SIMPLE,
    {   "->"    // for range
    }}
,   {   ORT_RTL_INFIX | TKP_PARSE_SIMPLE,
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
,   {   ORT_RTL_TERNARY | TKP_PARSE_TERNARY,
    {
    }}
/*  comma is handled as an operator in C, but may be handled elsewhere
,   {   OT_LTR_INFIX,
    {   ","     // separator
    }}
 */
};

inline bool charIsWhitespace (char c)
{
    switch (c)
    {
        case ' ': case '\t': case '\n': case '\r':
            return true;
            break;
    }
    return false;
}

inline bool charIsLetter (char c)
{
    switch (c)
    {
        case '$': case '_': case '\\':
            return true;
            break;
    }
    return RANGE('A',c,'Z') || RANGE('a',c,'z');
}

inline bool charIsOperator (char c)
{
    switch (c)
    {
        case '+': case '-': case '*': case '/': case '%':
        case '&': case '|': case '^': case '~':
        case '<': case '=': case '>': case '!':
        case '@': case '?': case ':': case ';':
            return true;
            break;
    }
    return false;
}

inline bool charIsValid (char c)
{
    return RANGE(' ',c,'~');
}

