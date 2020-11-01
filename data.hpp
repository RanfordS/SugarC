#include <vector>
#include <string>
#define RANGE(a,b,c) ((a) <= (b) && (b) <= (c))

// TokenClass rethink

enum TokenClass
{   TK_CLASS_MASK                      = 0b00000111
,   TK_CLASS_NONE                      = 0b00000000
,   TK_CLASS_OPERATOR                  = 0b00000001
,   TK_CLASS_COMMENT                   = 0b00000010
,   TK_CLASS_LITERAL                   = 0b00000011
,   TK_CLASS_BRACKET                   = 0b00000100
,   TK_CLASS_NUMBER                    = 0b00000101
,   TK_CLASS_NOUN                      = 0b00000110
,   TK_CLASS_CONTEXT                   = 0b00000111

,   TKP_SIDE_MASK                      = 0b11000000
,   TKP_HAS_LEFT                       = 0b10000000
,   TKP_HAS_RIGHT                      = 0b01000000
,   TKP_PARSE_RIGHT_TO_LEFT            = 0b00100000
,   TKP_PARSE_MASK                     = 0b00011111
,   TKP_PARSE_SUFFIX_INDEX_CALL        = 0b00000000
,   TKP_PARSE_PREFIX                   = 0b00000001
,   TKP_PARSE_INFIX                    = 0b00000010
,   TKP_PARSE_TERNARY                  = 0b00000011
,   TKP_PARSE_SIMPLE                   = 0b00011111

//  simple

,   TKP_COMMENT_BLOCK                  = 0b00001000
,   TKP_LITTERAL_STRING                = 0b00001000

,   TK_NONE
    = TK_CLASS_NONE

,   TK_OPERATOR
    = TK_CLASS_OPERATOR

,   TK_COMMENT_LINE
    = TK_CLASS_COMMENT

,   TK_COMMENT_BLOCK
    = TK_CLASS_COMMENT | TKP_COMMENT_BLOCK

,   TK_LITERAL_CHAR
    = TK_CLASS_LITERAL

,   TK_LITERAL_STRING
    = TK_CLASS_LITERAL | TKP_LITTERAL_STRING

//  brackets

,   TKP_BRACKET_CLASS_MASK             = 0b00011000
,   TKP_BRACKET_CLASS_ROUND            = 0b00000000
,   TKP_BRACKET_CLASS_CURLY            = 0b00001000
,   TKP_BRACKET_CLASS_SQUARE           = 0b00010000
,   TKP_BRACKET_CLASS_TERNARY          = 0b00010000

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

,   TKP_NUMBER_CLASS_MASK              = 0b00111000
,   TKP_NUMBER_CLASS_UNCERTAIN         = 0b00000000
,   TKP_NUMBER_CLASS_INT               = 0b00100000
,   TKP_NUMBER_CLASS_ZERO              = 0b00001000
,   TKP_NUMBER_CLASS_FLOAT             = 0b00010000
,   TKP_NUMBER_CLASS_NONB10            = 0b00011000
,   TKP_NUMBER_NONB10_MASK             = 0b11000000
,   TKP_NUMBER_NONB10_BIN              = 0b00000000
,   TKP_NUMBER_NONB10_OCT              = 0b01000000
,   TKP_NUMBER_NONB10_HEX              = 0b10000000
,   TKP_NUMBER_INT_UNSIGNED            = 0b01000000
,   TKP_NUMBER_INT_SPECIFIED           = 0b10000000
,   TKP_NUMBER_FLOAT_EXPONENT          = 0b01000000
,   TKP_NUMBER_FLOAT_FLOAT             = 0b10000000

,   TK_NUMBER
    = TK_CLASS_NUMBER | TKP_NUMBER_CLASS_UNCERTAIN

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

,   TKP_NOUN_CLASS_MASK                = 0b11111000
,   TKP_NOUN_CLASS_NONE                = 0b00000000
,   TKP_NOUN_CLASS_KEYWORD             = 0b00001000
,   TKP_NOUN_CLASS_TYPE                = 0b00010000
,   TKP_NOUN_CLASS_VARIABLE            = 0b00011000
,   TKP_NOUN_CLASS_FUNCTION            = 0b00100000

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
/*
___00 - control
___01 - expression
___10 - declaration
___11 - definition

control
000 - file/root
001 - branch
010 - statement
011
100
101
110
111

expressions
000 - ternary expression
010 - cast expression
110 - reinterpret cast expression
100
001 - call expression or index expression
011 - prefix expression
111 - infix expression
101 - suffix expression

declarations
000 - variable declaration
010
100
110
001 - call declaration
011 - prefix declaration
111 - infix declaration
101 - suffix declaration

definitions
000 - variable definition
010 - type definition
100 - member declaration
110
001 - call definition
011 - prefix definition
111 - infix definition
101 - suffix definition
*/
,   TKP_CONTEXT_CLASS_MASK             = 0b00011000
,   TKP_CONTEXT_CLASS_CTRL             = 0b00000000
,   TKP_CONTEXT_CLASS_EXP              = 0b00001000
,   TKP_CONTEXT_CLASS_DEC              = 0b00010000
,   TKP_CONTEXT_CLASS_DEF              = 0b00011000
,   TKP_CONTEXT_SUBCLASS_MASK          = 0b11100000
,   TKP_CONTEXT_FUNCTION               = 0b00100000
,   TKP_CONTEXT_CONTROL_FILE           = 0b00000000
,   TKP_CONTEXT_CONTROL_BRANCH         = 0b00100000
,   TKP_CONTEXT_CONTROL_STATEMENT      = 0b01000000
,   TKP_CONTEXT_EXPRESSION_TERNARY     = 0b00000000
,   TKP_CONTEXT_EXPRESSION_CAST        = 0b00100000
,   TKP_CONTEXT_EXPRESSION_REINTERP    = 0b01000000
,   TKP_CONTEXT_DECLARATION_VARIABLE   = 0b00000000
,   TKP_CONTEXT_DEFINITION_VARIABLE    = 0b00000000
,   TKP_CONTEXT_DEFINITION_TYPE        = 0b01000000
,   TKP_CONTEXT_DEFINITION_MEMBER      = 0b10000000

,   TK_CONTEXT_FILE
    = TK_CLASS_CONTEXT | TKP_CONTEXT_CLASS_CTRL

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

struct Token
{
    uint8_t tokenClass;
    std::string raw;
    size_t line;
    size_t column;
    std::vector<Token> subtokens;
};

extern const std::vector <std::string> inbuiltTypes;
extern const std::vector <std::string> inbuiltVariableTypes;
extern const std::vector <std::string> modifiers;
extern const std::vector <std::string> keywords;
extern const std::vector <std::string> operators;

enum OperatorRoundType
{   ORT_LTR_TERNARY = 0
,   ORT_LTR_SUFFIX  = TKP_HAS_LEFT
,   ORT_LTR_PREFIX  = TKP_HAS_RIGHT
,   ORT_LTR_INFIX   = TKP_HAS_LEFT | TKP_HAS_RIGHT
,   ORT_RTL_TERNARY = TKP_PARSE_RIGHT_TO_LEFT | ORT_LTR_TERNARY
,   ORT_RTL_SUFFIX  = TKP_PARSE_RIGHT_TO_LEFT | ORT_LTR_SUFFIX
,   ORT_RTL_PREFIX  = TKP_PARSE_RIGHT_TO_LEFT | ORT_LTR_PREFIX
,   ORT_RTL_INFIX   = TKP_PARSE_RIGHT_TO_LEFT | ORT_LTR_INFIX
};

struct OperatorRound
{   uint8_t type;
    std::vector <std::string> operators;
};

extern const std::vector <OperatorRound> inbuildRounds;
extern bool charIsWhitespace (char c);
extern bool charIsLetter (char c);
extern bool charIsOperator (char c);
extern bool charIsValid (char c);

