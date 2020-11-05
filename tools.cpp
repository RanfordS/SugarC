#include "tools.hpp"

void groupTokens (std::vector <Token> &list, size_t idx, size_t num, Token &parent)
{
    for (size_t i = 0; i < num; ++i)
    {
        parent.subtokens.push_back (list[idx+i]);
    }

    list.erase (list.begin () + idx, list.begin () + idx + num);
    list.emplace (list.begin () + idx, parent);
}
