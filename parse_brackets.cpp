#include "parse_header.hpp"

enum BracketedTokenClass
{   BTCLASS_BRACKET_ROUND
,   BTCLASS_BRACKET_SQUARE
,   BTCLASS_BRACKET_CURLY
,   BTCLASS_COMMENT_LINE
,   BTCLASS_COMMENT_BLOCK
,   BTCLASS_LITERAL_NUMBER
,   BTCLASS_LITERAL_STRING
,   BTCLASS_OPERATOR
,   BTCLASS_NOUN
,   BTCLASS_STATEMENT
};

struct BracketedToken
{
    BracketedTokenClass btClass;
    std::vector<BracketedToken> subTokens;
    std::string raw;
    size_t line;
    size_t column;
};

#define ISOPENBRACKET(c) ((c) == '(' || (c) == '[' || (c) == '{')
#define ISCLOSEBRACKET(c) ((c) == ')' || (c) == ']' || (c) == '}')



bool parseBrackets (std::vector<Token> &initial, BracketedToken* fileScope)
{
    std::vector<BracketedToken*> bracketStack = {fileScope};
    BracketedToken* currentScope = fileScope;

    for (size_t i = 0; i < initial.size (); ++i)
    {
        if (initial[i].tClass == TOKEN_CLASS_BRACKET)
        {
            char bracket = initial[i].raw[0];
        }
        else
        {
            currentScope->subTokens.push_back (initial[i]);
        }
    }
}
