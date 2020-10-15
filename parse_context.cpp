#include "parse_header.hpp"
#include <map>

struct DeclarationInfo
{
    TokenClass tClass;
};

typedef std::map<std::string,DeclarationInfo> DeclarationMap;

struct DeclarationList
{
    DeclarationMap data;

    DeclarationInfo declarationInScope (std::string name)
    {
        DeclarationMap::iterator iter = data.find (name);
        if (iter != data.end ())
        {
            return iter->second;
        }

        DeclarationInfo none = {};
        return none;
    }
};

struct DeclarationStack
{
    std::vector<DeclarationList> stack;

    DeclarationInfo declarationInScope (std::string name)
    {
        DeclarationInfo info;
        size_t i = stack.size ();
        while (i--)
        {
            info = stack[i].declarationInScope (name);
            if (info.tClass) break;
        }
        return info;
    }

    DeclarationInfo declarationInLocalScope (std::string name)
    {
        size_t i = stack.size () - 1;
        return stack[i].declarationInScope (name);
    }

    void pushDeclaration (std::string name, DeclarationInfo info)
    {
        stack[stack.size () - 1].data[name] = info;
    }
};

void parseContextInternal (Token* scope, std::vector<DeclarationList> list)
{
    { DeclarationList top = {}; list.push_back (top); }
}

void parseContext (Token* scope)
{
    for (size_t i = 0; i < scope->subTokens.size (); ++i)
    {
        Token* token = &scope->subTokens[i];
        TokenClass tClass = token->tClass;

        if (tClass == TK_NOUN_GENERIC)
        {
            if (token->raw == "Int"
            ||  token->raw == "Float"
            ||  token->raw == "Size"
            ||  token->raw == "Char")
            {
                token->tClass = TK_NOUN_TYPE;
            }
        }
        parseContext (token);
    }
}
