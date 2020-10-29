#include <string>
#include <vector>
#include <unordered_map>

#define RANGE(a,b,c) ((a) <= (b) && (b) <= (c))

#define CHAR_ISWHITESPACE(c) \
    ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')

#define CHAR_ISLETTER(c) \
    (RANGE('A',c,'Z') || RANGE('a',c,'z') || (c) == '$' || (c) == '_' || (c) == '\\')

// ops + - = * / % & | ^ ! < > ~ @ ; : ?
// excludes .
#define CHAR_ISOPERATOR(c) ((c) == '+' || (c) == '-' || (c) == '=' || (c) == '*' || (c) == '/' || (c) == '%' || (c) == '&' || (c) == '|' || (c) == '^' || (c) == '!' || (c) == '<' || (c) == '>' || (c) == '~' || (c) == '@' || (c) == ';' || (c) == ':' || (c) == '?')

#define CHAR_ISVALID(c) RANGE(' ',c,'~')

extern const std::vector <std::string> inbuiltTypes;
extern const std::vector <std::string> inbuiltVariableTypes;

extern bool isstandardtype (std::string raw);

enum TokenClass
{   TK_NONE
//  operator
,   TK_OPERATOR
//  delimited
,   TK_COMMENT_LINE
,   TK_COMMENT_BLOCK
,   TK_LITERAL_CHAR
,   TK_LITERAL_STRING
//  brackets
,   TK_BRACKET_OPEN_ROUND
,   TK_BRACKET_OPEN_SQUARE
,   TK_BRACKET_OPEN_CURLY
,   TK_BRACKET_CLOSE_ROUND
,   TK_BRACKET_CLOSE_SQUARE
,   TK_BRACKET_CLOSE_CURLY
,   TK_BRACKET_BLOCK_ROUND
,   TK_BRACKET_BLOCK_SQUARE
,   TK_BRACKET_BLOCK_CURLY
//  numbers
,   TK_NUMBER
,   TK_NUMBER_ZERO
,   TK_NUMBER_INT_SIGNED
,   TK_NUMBER_INT_UNSIGNED
,   TK_NUMBER_INT_SPECIFIED_SIGNED
,   TK_NUMBER_INT_SPECIFIED_UNSIGNED
,   TK_NUMBER_BINARY
,   TK_NUMBER_OCTAL
,   TK_NUMBER_HEXIDECIMAL
,   TK_NUMBER_DOUBLE
,   TK_NUMBER_FLOAT
,   TK_NUMBER_EXPONENTIAL_DOUBLE
,   TK_NUMBER_EXPONENTIAL_FLOAT
//  nouns
,   TK_NOUN
,   TK_NOUN_KEYWORD
,   TK_NOUN_TYPE
,   TK_NOUN_VARIABLE
,   TK_NOUN_OPERATOR
//  contextual
,   TK_CONTEXT_FILE
    //- (declaration | definition | include)*
,   TK_CONTEXT_EXPRESSION_PREFIX
    //- operator, expression
,   TK_CONTEXT_EXPRESSION_INFIX
    //- expression, operator, expression
,   TK_CONTEXT_EXPRESSION_SUFFIX
    //- expression, operator
,   TK_CONTEXT_EXPRESSION_TERNARY
    //- expression, operator, expression, operator, expression
,   TK_CONTEXT_EXPRESSION_INDEX
    //- expression, square block
,   TK_CONTEXT_EXPRESSION_CALL
    //- expression, round block
,   TK_CONTEXT_EXPRESSION_CAST
    //- round open, type, colon, expression, round close
,   TK_CONTEXT_EXPRESSION_CAST_REINTERP
    //- round open, exclaimation, type, colon, expression, round close
,   TK_CONTEXT_DECLARATION_VARIABLE
    //- type, colon, (noun | expression)+
,   TK_CONTEXT_DECLARATION_FUNCTION
    //- Function, type, colon, noun, round block, semicolon
,   TK_CONTEXT_DECLARATION_PREFIX
    //- Prefix, type, colon, noun, round block, semicolon
,   TK_CONTEXT_DECLARATION_INFIX
    //- Infix, type, colon, round block, noun, round block, semicolon
,   TK_CONTEXT_DECLARATION_SUFFIX
    //- Suffix, type, colon, round block, noun, semicolon
,   TK_CONTEXT_DEFINITION_TYPE
    //- type, colon, name, curly block
,   TK_CONTEXT_DEFINITION_FUNCTION
    //- Function, type, colon, noun, round block, curly block
,   TK_CONTEXT_DEFINITION_PREFIX
    //- Prefix, type, colon, noun, round block, curly block
,   TK_CONTEXT_DEFINITION_INFIX
    //- Infix, type, colon, round block, noun, round block, curly block
,   TK_CONTEXT_DEFINITION_SUFFIX
    //- Suffix, type, colon, round block, noun, curly block
,   TK_CONTEXT_DECLARATION_MEMBER
    //- noun | expression
,   TK_CONTEXT_STATEMENT
    //- expression, semi-colon
,   TK_CONTEXT_BRANCHING
    //- keyword, round block, curly block | statement
};

#define TK_ISDELIMITED(t) RANGE(TK_COMMENT_LINE,t,TK_LITERAL_STRING)
#define TK_ISBRACKET(t) RANGE(TK_BRACKET_OPEN_ROUND,t,TK_BRACKET_BLOCK_CURLY)
#define TK_ISBRACKET_OPEN(t) \
    RANGE(TK_BRACKET_OPEN_ROUND,t,TK_BRACKET_OPEN_CURLY)
#define TK_ISBRACKET_CLOSE(t) \
    RANGE(TK_BRACKET_CLOSE_ROUND,t,TK_BRACKET_CLOSE_CURLY)
#define TK_ISBRACKET_BLOCK(t) \
    RANGE(TK_BRACKET_BLOCK_ROUND,t,TK_BRACKET_BLOCK_CURLY)
#define TK_ISBRACKET_MATCHING(a,b) \
    ((a) + (TK_BRACKET_CLOSE_ROUND - TK_BRACKET_OPEN_ROUND) == (b))
#define TK_BRACKET_OPEN_TO_BLOCK(a) \
    ((TokenClass)((a) + (TK_BRACKET_BLOCK_ROUND - TK_BRACKET_OPEN_ROUND)))
#define TK_ISNUMBER(t) RANGE(TK_NUMBER,t,TK_NUMBER_EXPONENTIAL_FLOAT)
#define TK_ISNOUN(t) RANGE(TK_NOUN,t,TK_NOUN_VARIABLE)

#define TK_ISEXPRESSION(t) \
    (RANGE(TK_CONTEXT_EXPRESSION_PREFIX,t,TK_CONTEXT_EXPRESSION_CAST_REINTERP) \
     || TK_ISNUMBER(t) || TK_ISNOUN(t) || TK_ISBRACKET(t))

extern const std::vector <std::string> operators;

bool stringescape (std::string text);

struct Token
{
    TokenClass tokenClass;
    std::string raw;
    size_t line;
    size_t column;
    std::vector<Token> subtokens;
};

extern const std::vector <std::string> keywords;
extern void identifykeywords (std::vector <Token> &list);



extern bool alternateparse (FILE* file, std::vector <Token> &list);



struct BracketOffence
{
    uint8_t number;
    Token tokens[2];
};

extern bool bracketsvalidator
(std::vector <Token> &list, std::vector <BracketOffence> &offenders);

extern Token bracket (std::vector <Token> &list);

extern bool contextparse (Token &root);



extern void highlighter (std::vector <Token> &root);


