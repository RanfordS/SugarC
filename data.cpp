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
,   "include"
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

bool charIsWhitespace (char c)
{
    switch (c)
    {
        case ' ': case '\t': case '\n': case '\r':
            return true;
            break;
    }
    return false;
}

bool charIsLetter (char c)
{
    switch (c)
    {
        case '$': case '_': case '\\':
            return true;
            break;
    }
    return RANGE('A',c,'Z') || RANGE('a',c,'z');
}

bool charIsOperator (char c)
{
    switch (c)
    {
        case '+': case '-': case '*': case '/': case '%':
        case '&': case '|': case '^': case '~':
        case '<': case '=': case '>': case '!':
        case '@': case '?': case ':': case ';': case ',':
        case '(': case ')': case '[': case ']': case '{': case '}':
            return true;
            break;
    }
    return false;
}

bool charIsValid (char c)
{
    return RANGE(' ',c,'~');
}

// could not get a map or an array to create hold this structure
std::string getTokenName (uint8_t tokenClass)
{   switch (tokenClass)
    {
case TK_NONE:                               return "None";
case TK_OPERATOR:                           return "Operator";
case TK_COMMENT_LINE:                       return "Comment (line)";
case TK_COMMENT_BLOCK:                      return "Comment (block)";
case TK_LITERAL_CHAR:                       return "Char";
case TK_LITERAL_STRING:                     return "String";
case TK_BRACKET_OPEN_ROUND:                 return "Bracket (open round)";
case TK_BRACKET_OPEN_SQUARE:                return "Bracket (open square)";
case TK_BRACKET_OPEN_CURLY:                 return "Bracket (open curly)";
case TK_BRACKET_OPEN_TERNARY:               return "Bracket (open ternary)";
case TK_BRACKET_CLOSE_ROUND:                return "Bracket (close round)";
case TK_BRACKET_CLOSE_SQUARE:               return "Bracket (close square)";
case TK_BRACKET_CLOSE_CURLY:                return "Bracket (close curly)";
case TK_BRACKET_CLOSE_TERNARY:              return "Bracket (close ternary)";
case TK_BRACKET_BLOCK_ROUND:                return "Bracket (block round)";
case TK_BRACKET_BLOCK_SQUARE:               return "Bracket (block square)";
case TK_BRACKET_BLOCK_CURLY:                return "Bracket (block curly)";
case TK_BRACKET_BLOCK_TERNARY:              return "Bracket (block ternary)";
case TK_NUMBER:                             return "Number (uncertain)";
case TK_NUMBER_ZERO:                        return "Number (zero)";
case TK_NUMBER_INT_SIGNED:                  return "Number (int)";
case TK_NUMBER_INT_UNSIGNED:                return "Number (uint)";
case TK_NUMBER_INT_SPECIFIED_SIGNED:        return "Number (intN)";
case TK_NUMBER_INT_SPECIFIED_UNSIGNED:      return "Number (uintN)";
case TK_NUMBER_BINARY:                      return "Number (bin)";
case TK_NUMBER_OCTAL:                       return "Number (oct)";
case TK_NUMBER_HEXADECIMAL:                 return "Number (hex)";
case TK_NUMBER_DOUBLE:                      return "Number (double)";
case TK_NUMBER_FLOAT:                       return "Number (float)";
case TK_NUMBER_EXPONENTIAL_DOUBLE:          return "Number (exp double)";
case TK_NUMBER_EXPONENTIAL_FLOAT:           return "Number (exp float)";
case TK_NOUN:                               return "Noun (generic)";
case TK_NOUN_KEYWORD:                       return "Noun (keyword)";
case TK_NOUN_VARIABLE:                      return "Noun (variable)";
case TK_NOUN_TYPE:                          return "Noun (type)";
case TK_NOUN_FUNCTION:                      return "Noun (function)";
case TK_CONTEXT_FILE:                       return "Context (file)";
case TK_CONTEXT_INCLUDE:                    return "Context (include)";
case TK_CONTEXT_BRANCH:                     return "Context (branch)";
case TK_CONTEXT_STATEMENT:                  return "Context (statement)";
case TK_CONTEXT_EXPRESSION_CALL:            return "Expression (call)";
case TK_CONTEXT_EXPRESSION_PREFIX:          return "Expression (prefix)";
case TK_CONTEXT_EXPRESSION_SUFFIX:          return "Expression (suffix)";
case TK_CONTEXT_EXPRESSION_INFIX:           return "Expression (infix)";
case TK_CONTEXT_DECLARATION_CALL:           return "Declaration (call)";
case TK_CONTEXT_DECLARATION_PREFIX:         return "Declaration (prefix)";
case TK_CONTEXT_DECLARATION_SUFFIX:         return "Declaration (suffix)";
case TK_CONTEXT_DECLARATION_INFIX:          return "Declaration (infix)";
case TK_CONTEXT_DEFINITION_CALL:            return "Definition (call)";
case TK_CONTEXT_DEFINITION_PREFIX:          return "Definition (prefix)";
case TK_CONTEXT_DEFINITION_SUFFIX:          return "Definition (suffix)";
case TK_CONTEXT_DEFINITION_INFIX:           return "Definition (infix)";
case TK_CONTEXT_EXPRESSION_TERNARY:         return "Expression (ternary)";
case TK_CONTEXT_EXPRESSION_CAST:            return "Expression (cast)";
case TK_CONTEXT_EXPRESSION_CAST_REINTERP:   return "Expression (!cast)";
case TK_CONTEXT_DEFINITION_TYPE:            return "Definition (type)";
case TK_CONTEXT_DECLARATION_TYPE:           return "Declaration (type)";
default:                                    return "UNKNOWN";
    }
}

bool tokenIsExpression (uint8_t tokenClass)
{
    switch (tokenClass & TK_CLASS_MASK)
    {
        case TK_CLASS_CONTEXT:
            return (tokenClass & TKP_CONTEXT_CLASS_MASK)
                == TKP_CONTEXT_CLASS_EXP;

        case TK_CLASS_BRACKET:
            switch (tokenClass & TKP_BRACKET_CLASS_MASK)
            {
                case TKP_BRACKET_CLASS_ROUND:
                    return true;

                default:
                    return false;
            }

        case TK_CLASS_NUMBER:
        case TK_CLASS_LITERAL:
            return true;

        case TK_CLASS_NOUN:
            return true;

        default:
            return false;
    }
}
