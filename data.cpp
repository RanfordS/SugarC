#include "alternate.hpp"

/* TokenClass rethink

enum TokenClass
{   TK_CLASS_MASK             = 0b00000111
,   TK_CLASS_NONE             = 0b00000000
,   TK_CLASS_OPERATOR         = 0b00000001
,   TK_CLASS_COMMENT          = 0b00000010
,   TK_CLASS_LITTERAL         = 0b00000011
,   TK_CLASS_BRACKET          = 0b00000100
,   TK_CLASS_NUMBER           = 0b00000101
,   TK_CLASS_NOUN             = 0b00000110
,   TK_CLASS_CONTEXT          = 0b00000111

,   TKP_HAS_LEFT              = 0b10000000
,   TKP_HAS_RIGHT             = 0b01000000

//  simple

,   TKP_COMMENT_BLOCK         = 0b00001000
,   TKP_LITTERAL_STRING       = 0b00001000

,   TK_NONE
    = TK_CLASS_NONE

,   TK_OPERATOR
    = TK_CLASS_OPERATOR

,   TK_COMMENT_LINE
    = TK_CLASS_COMMENT

,   TK_COMMENT_BLOCK
    = TK_CLASS_COMMENT | TKP_COMMENT_BLOCK

//  brackets

,   TKP_BRACKET_CLASS_MASK    = 0b00011000
,   TKP_BRACKET_CLASS_ROUND   = 0b00000000
,   TKP_BRACKET_CLASS_CURLY   = 0b00001000
,   TKP_BRACKET_CLASS_SQUARE  = 0b00010000
,   TKP_BRACKET_CLASS_TERNARY = 0b00010000

,   TK_BRACKET_OPEN_ROUND
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_ROUND
    | TKP_HAS_LEFT

,   TK_BRACKET_OPEN_SQUARE
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_SQUARE
    | TKP_HAS_LEFT

,   TK_BRACKET_OPEN_CURLY
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_CURLY
    | TKP_HAS_LEFT

,   TK_BRACKET_OPEN_TERNARY
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_TERNARY
    | TKP_HAS_LEFT

,   TK_BRACKET_CLOSE_ROUND
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_ROUND
    | TKP_HAS_RIGHT

,   TK_BRACKET_CLOSE_SQUARE
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_SQUARE
    | TKP_HAS_RIGHT

,   TK_BRACKET_CLOSE_CURLY
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_CURLY
    | TKP_HAS_RIGHT

,   TK_BRACKET_CLOSE_TERNARY
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_TERNARY
    | TKP_HAS_RIGHT

,   TK_BRACKET_BLOCK_ROUND
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_ROUND
    | TKP_HAS_LEFT | TKP_HAS_RIGHT

,   TK_BRACKET_BLOCK_SQUARE
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_SQUARE
    | TKP_HAS_LEFT | TKP_HAS_RIGHT

,   TK_BRACKET_BLOCK_CURLY
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_CURLY
    | TKP_HAS_LEFT | TKP_HAS_RIGHT

,   TK_BRACKET_BLOCK_TERNARY
    = TK_CLASS_BRACKET | TKP_BRACKET_CLASS_TERNARY
    | TKP_HAS_LEFT | TKP_HAS_RIGHT

//  numbers

,   TKP_NUMBER_CLASS_MASK     = 0b00011000
,   TKP_NUMBER_CLASS_INT      = 0b00000000
,   TKP_NUMBER_CLASS_ZERO     = 0b00001000
,   TKP_NUMBER_CLASS_NONB10   = 0b00010000
,   TKP_NUMBER_CLASS_FLOAT    = 0b00011000
,   TKP_NUMBER_NONB10_MASK    = 0b01100000
,   TKP_NUMBER_NONB10_BIN     = 0b00000000
,   TKP_NUMBER_NONB10_OCT     = 0b00100000
,   TKP_NUMBER_NONB10_HEX     = 0b01000000
,   TKP_NUMBER_INT_UNSIGNED   = 0b00100000
,   TKP_NUMBER_INT_SPECIFIED  = 0b01000000
,   TKP_NUMBER_FLOAT_EXPONENT = 0b00100000
,   TKP_NUMBER_FLOAT_FLOAT    = 0b01000000

,   TK_NUMBER_ZERO
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_ZERO

,   TK_NUMBER_INT_SIGNED
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_INT

,   TK_NUMBER_INT_UNSIGNED
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_INT
    | TKP_NUMBER_INT_UNSIGNED

,   TK_NUMBER_INT_SPECIFIED_SIGNED
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_INT
    | TKP_NUMBER_INT_SPECIFIED

,   TK_NUMBER_INT_SPECIFIED_UNSIGNED
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_INT
    | TKP_NUMBER_INT_SPECIFIED | TKP_NUMBER_INT_UNSIGNED

,   TK_NUMBER_BINARY
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_NONB10 | TKP_NUMBER_NONB10_BIN

,   TK_NUMBER_OCTAL
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_NONB10 | TKP_NUMBER_NONB10_OCT

,   TK_NUMBER_HEXADECIMAL
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_NONB10 | TKP_NUMBER_NONB10_HEX

,   TK_NUMBER_DOUBLE
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_FLOAT

,   TK_NUMBER_FLOAT
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_FLOAT
    | TKP_NUMBER_FLOAT_FLOAT

,   TK_NUMBER_EXPONENTIAL_DOUBLE
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_FLOAT
    | TKP_NUMBER_FLOAT_EXPONENT

,   TK_NUMBER_EXPONENTIAL_FLOAT
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_FLOAT
    | TKP_NUMBER_FLOAT_EXPONENT | TKP_NUMBER_FLOAT_FLOAT

//  nouns

,   TKP_NOUN_CLASS_MASK       = 0b00111000
,   TKP_NOUN_CLASS_NONE       = 0b00000000
,   TKP_NOUN_CLASS_KEYWORD    = 0b00001000
,   TKP_NOUN_CLASS_TYPE       = 0b00010000
,   TKP_NOUN_CLASS_VARIABLE   = 0b00011000
,   TKP_NOUN_CLASS_FUNCTION   = 0b00100000

,   TK_NOUN
    = TK_CLASS_NOUN | TKP_NOUN_CLASS_NONE

,   TK_NOUN_KEYWORD
    = TK_CLASS_NOUN | TKP_NOUN_CLASS_KEYWORD

,   TK_NOUN_TYPE
    = TK_CLASS_NOUN | TKP_NOUN_CLASS_TYPE

,   TK_NOUN_VARIABLE
    = TK_CLASS_NOUN | TKP_NOUN_CLASS_VARIABLE

,   TK_NOUN_FUNCTION
    = TK_CLASS_NOUN | TKP_NOUN_CLASS_FUNCTION

//  context
/_*
___00 - control
___01 - expression
___10 - declaration
___11 - definition

control
000 - file/root
001 - branch
010 - statement
011 -
100 -
101 -
110 -
111 -

expressions
000 - ternary expression
010 - cast expression
110 - reinterpret cast expression
100 - UNUSED
001 - call expression or index expression
011 - prefix expression
111 - infix expression
101 - suffix expression

declarations
000 - variable declaration
010 -
100 -
110 -
001 - call declaration
011 - prefix declaration
111 - infix declaration
101 - suffix declaration

definitions
000 - variable definition
010 - type definition
100 - member declaration
110 -
001 - call definition
011 - prefix definition
111 - infix definition
101 - suffix definition
*_/
,   TKP_CONTEXT_CLASS_MASK = 0b00011000
,   TKP_CONTEXT_CLASS_NONE = 0b00000000
,   TKP_CONTEXT_CLASS_EXP  = 0b00001000
,   TKP_CONTEXT_CLASS_DEC  = 0b00010000
,   TKP_CONTEXT_CLASS_DEF  = 0b00011000
,   TKP_CONTEXT_FUNCTION   = 0b00100000

,   TK_CONTEXT_FILE
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_NONE

,   TK_CONTEXT_EXPRESSION_CALL
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_EXP
    | TKP_CONTEXT_FUNCTION

,   TK_CONTEXT_EXPRESSION_PREFIX
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_EXP
    | TKP_CONTEXT_FUNCTION | TKP_HAS_LEFT

,   TK_CONTEXT_EXPRESSION_SUFFIX
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_EXP
    | TKP_CONTEXT_FUNCTION | TKP_HAS_RIGHT

,   TK_CONTEXT_EXPRESSION_INFIX
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_EXP
    | TKP_CONTEXT_FUNCTION | TKP_HAS_LEFT | TKP_HAS_RIGHT

,   TK_CONTEXT_DECLARATION_CALL
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEC
    | TKP_CONTEXT_FUNCTION

,   TK_CONTEXT_DECLARATION_PREFIX
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEC
    | TKP_CONTEXT_FUNCTION | TKP_HAS_LEFT

,   TK_CONTEXT_DECLARATION_SUFFIX
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEC
    | TKP_CONTEXT_FUNCTION | TKP_HAS_RIGHT

,   TK_CONTEXT_DECLARATION_INFIX
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEC
    | TKP_CONTEXT_FUNCTION | TKP_HAS_LEFT | TKP_HAS_RIGHT

,   TK_CONTEXT_DEFINITION_CALL
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEF
    | TKP_CONTEXT_FUNCTION

,   TK_CONTEXT_DEFINITION_PREFIX
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEF
    | TKP_CONTEXT_FUNCTION | TKP_HAS_LEFT

,   TK_CONTEXT_DEFINITION_SUFFIX
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEF
    | TKP_CONTEXT_FUNCTION | TKP_HAS_RIGHT

,   TK_CONTEXT_DEFINITION_INFIX
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_DEF
    | TKP_CONTEXT_FUNCTION | TKP_HAS_LEFT | TKP_HAS_RIGHT

,   TK_CONTEXT_EXPRESSION_TERNARY
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_EXP
    | TKP_HAS_LEFT | TKP_HAS_RIGHT
};

// 1,2 class
// 3 is function
// 4 - has_left (3=1)
// 5 - has_right (3=1)

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
{   OP_NOUN_MASK = 0b00011111 // the id of the noun group
,   OP_NOUN_NONE = 0b00011111
,   OP_DIR_RTL   = 0b00100000
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
