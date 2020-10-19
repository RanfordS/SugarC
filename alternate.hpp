#include <string>
#include <vector>

#define RANGE(a,b,c) ((a) <= (b) && (b) <= (c))

#define CHAR_ISWHITESPACE(c) \
    ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')

#define CHAR_ISLETTER(c) \
    (RANGE('A',c,'Z') || RANGE('a',c,'z') || (c) == '_')

// ops + - = * / % & | ^ ! < > ~ @ ; : ?
// excludes .
#define CHAR_ISOPERATOR(c) ((c) == '+' || (c) == '-' || (c) == '=' || (c) == '*' || (c) == '/' || (c) == '%' || (c) == '&' || (c) == '|' || (c) == '^' || (c) == '!' || (c) == '<' || (c) == '>' || (c) == '~' || (c) == '@' || (c) == ';' || (c) == ':' || (c) == '?')

#define CHAR_ISVALID(c) RANGE(' ',c,'~')

extern const std::vector<std::string> inbuiltTypes;;
extern const std::vector<std::string> inbuiltVariableTypes;

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
//  contextual
,   TK_CONTEXT_OPERATOR_PREFIX
,   TK_CONTEXT_OPERATOR_INFIX
,   TK_CONTEXT_OPERATOR_SUFFIX
,   TK_CONTEXT_CAST
,   TK_CONTEXT_STATEMENT
,   TK_CONTEXT_SCOPE
};

#define TK_ISDELIMITED(t) RANGE(TK_COMMENT_LINE,t,TK_LITERAL_STRING)
#define TK_ISBRACKET(t) RANGE(TK_BRACKET_OPEN_ROUND,t,TK_BRACKET_BLOCK_CURLY)
#define TK_ISNUMBER(t) RANGE(TK_NUMBER,t,TK_NUMBER_EXPONENTIAL_FLOAT)
#define TK_ISNOUN(t) RANGE(TK_NOUN,t,TK_NOUN_VARIABLE)



extern const std::vector<std::string> operators;

bool stringescape (std::string text);

struct Token
{
    TokenClass tokenClass;
    std::string raw;
    size_t line;
    size_t column;
    std::vector<Token> subtokens;
};

extern std::vector<std::string> keywords;
extern void identifykeywords (std::vector<Token> &list);



extern bool alternateparse (FILE* file, std::vector<Token> &list);





