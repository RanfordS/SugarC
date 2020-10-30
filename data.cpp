#include "alternate.hpp"

/* TokenClass rethink

enum TokenClass
{   TK_CLASS_MASK = 0b00000111
,   TK_CLASS_NONE = 0b00000000
,   TK_CLASS_OPER = 0b00000001
,   TK_CLASS_COMT = 0b00000010
,   TK_CLASS_LIT  = 0b00000011
,   TK_CLASS_BRAC = 0b00000100
,   TK_CLASS_NUM  = 0b00000101
,   TK_CLASS_NOUN = 0b00000110
,   TK_CLASS_CTX  = 0b00000111
};

enum CommentProperties
{   CP_BLOCK = 0b00001000
};

enum LiteralProperties
{   LP_STRING = 0b00001000
};

enum BracketProperties
{   BP_CLASS_MASK   = 0b00000011
,   BP_CLASS_ROUND  = 0b00000000
,   BP_CLASS_CURLY  = 0b00000001
,   BP_CLASS_SQUARE = 0b00000010
,   BP_OPEN         = 0b00000100
,   BP_CLOSE        = 0b00001000
};

enum NumberProperties
{   NP_CLASS_MASK    = 0b00011000
,   NP_CLASS_NUMBER  = 0b00000000
,   NP_CLASS_ZERO    = 0b00001000
,   NP_CLASS_FLOAT   = 0b00010000
,   NP_CLASS_NONB10  = 0b00011000
,   NP_INT_UNSIGNED  = 0b00100000
,   NP_INT_SPECIFIED = 0b01000000
,   NP_FLOAT_EXP     = 0b00100000
,   NP_FLOAT_FLOAT   = 0b01000000
,   NP_NONB10_MASK   = 0b01100000
,   NP_NONB10_BIN    = 0b00000000
,   NP_NONB10_OCT    = 0b00100000
,   NP_NONB10_HEX    = 0b01000000
};

enum NounProperties
{   WP_CLASS_MASK     = 0b00111000
,   WP_CLASS_NONE     = 0b00000000
,   WP_CLASS_KEYWORD  = 0b00001000
,   WP_CLASS_TYPE     = 0b00010000
,   WP_CLASS_OPERATOR = 0b00011000
,   WP_CLASS_VARIABLE = 0b00100000
};

// 1,2 class
// 3 is function
// 4 - has_left (3=1)
// 5 - has_right (3=1)

ternary
index
cast
reinter
variable declaration
type definition
member declaration
statement
branch

00000 - file
00001
00010
00011
00100
00101 - function expression (call)
00110 - function declaration
00111 - function definition
01000
01001 - cast expression
01010
01011
01100
01101 - suffix expression
01110 - suffix declaration
01111 - suffix definition
10000
10001
10010
10011
10100
10101 - prefix expression
10110 - prefix declaration
10111 - prefix definition
11000
11001 - ternary expression
11010
11011
11100
11101 - infix expression
11110 - infix declaration
11111 - infix definition

enum ContextProperties
{   XP_EXPRESSION          = 0b00001000
,   XP_EXPRESSION_HASLEFT  = 0b00010000
,   XP_EXPRESSION_HASRIGHT = 0b00100000
,   XP_
};

enum TokenClass
{   TK_NUMBER                = TK_CLASS_NUM
,   TK_NUMBER_ZERO           = TK_CLASS_NUM | NP_CLASS_ZERO
,   TK_NUMBER_INT            = TK_CLASS_NUM | NP_CLASS_NUMBER
,   TK_NUMBER_UINT           = TK_CLASS_NUM | NP_CLASS_NUMBER | NP_INT_UNSIGNED
,   TK_NUMBER_INT_SPECIFIED  = TK_CLASS_NUM | NP_CLASS_NUMBER | NP_INT_SPECIFIED
,   TK_NUMBER_UINT_SPECIFIED = TK_CLASS_NUM | NP_CLASS_NUMBER | NP_INT_UNSIGNED | NP_INT_SPECIFIED
,   TK_NUMBER_BINARY         = TK_CLASS_NUM | NP_CLASS_NONB10 | NP_NONB10_BIN
,   TK_NUMBER_OCTAL          = TK_CLASS_NUM | NP_CLASS_NONB10 | NP_NONB10_OCT
,   TK_NUMBER_HEXIDECIMAL    = TK_CLASS_NUM | NP_CLASS_NONB10 | NP_NONB10_HEX
,   TK_NUMBER_DOUBLE         = TK_CLASS_NUM | NP_CLASS_FLOAT
,   TK_NUMBER_FLOAT          = TK_CLASS_NUM | NP_CLASS_FLOAT | NP_FLOAT_FLOAT
,   TK_NUMBER_EXP_DOUBLE     = TK_CLASS_NUM | NP_CLASS_FLOAT | NP_FLOAT_EXP
,   TK_NUMBER_EXP_FLOAT      = TK_CLASS_NUM | NP_CLASS_FLOAT | NP_FLOAT_FLOAT | NP_FLOAT_EXP
};
*/

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

const std::vector <std::string> operators
{   "++", "--"
,   "+=", "-=", "*=", "/=", "%="
,   "&=", "|=", "^="
,   "==", "!=", "<=", ">="
,   "&&", "||", "^^"
,   "<<", ">>"
,   "?:"
,   "&&=", "||=", "^^="
,   "<<=", ">>="
,   "->"
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
};

// 0b???0 LTR
// 0b???1 RTL
// 0b?1?? has_left
// 0b??1? has_right
// 0b?00? is_ternary

// ltr tern   = 0b0000
// rtl tern   = 0b0001
// ltr prefix = 0b0010
// rtl prefix = 0b0011
// ltr suffix = 0b0100
// rtl suffix = 0b0101
// ltr infix  = 0b0110
// rtl infix  = 0b0111

enum OperatorProperties
{   OP_DIR_RTL   = 0b00000001
,   OP_HAS_LEFT  = 0b01000000 // matches renewed token
,   OP_HAS_RIGHT = 0b10000000 // matches renewed token
};

enum OperatorType
{   OT_LTR_TERNARY = 0          | 0           | 0
,   OT_LTR_PREFIX  = 0          | 0           | OP_HAS_RIGHT
,   OT_LTR_SUFFIX  = 0          | OP_HAS_LEFT | 0
,   OT_LTR_INFIX   = 0          | OP_HAS_LEFT | OP_HAS_RIGHT
,   OT_RTL_TERNARY = OP_DIR_RTL | 0           | 0
,   OT_RTL_PREFIX  = OP_DIR_RTL | 0           | OP_HAS_RIGHT
,   OT_RTL_SUFFIX  = OP_DIR_RTL | OP_HAS_LEFT | 0
,   OT_RTL_INFIX   = OP_DIR_RTL | OP_HAS_LEFT | OP_HAS_RIGHT
};

struct OperatorRound
{   uint8_t type;
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
/*  ternary operator goes here, but doesn't match the typical pattern
,   {   OT_RTL_TERNARY
    {   "?:"    // ternary selector
    }}
 */
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
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return true;
            break;
    }
    return false;
}

inline bool charIsLetter (char c)
{
    switch (c)
    {
        case '$':
        case '_':
        case '\\':
            return true;
            break;
    }
    return RANGE('A',c,'Z') || RANGE('a',c,'z');
}

inline bool charIsOperator (char c)
{
    switch (c)
    {
        case '+':
        case '-':
        case '=':
        case '*':
        case '/':
        case '%':
        case '&':
        case '|':
        case '^':
        case '!':
        case '<':
        case '>':
        case '~':
        case '@':
        case ';':
        case ':':
        case '?':
            return true;
            break;
    }
    return false;
}

inline bool charIsValid (char c)
{
    return RANGE(' ',c,'~');
}
