#include "tools.hpp"

void groupTokens (std::vector <Token> &list, size_t idx, size_t num, Token &parent)
{
    //std::printf ("groupTokens (idx = %lu, num = %lu)\n", idx, num);
    for (size_t i = 0; i < num; ++i)
    {
        //std::printf (" + [%lu,%lu]\n", list[idx+i].line, list[idx+i].column);
        parent.subtokens.push_back (list[idx+i]);
    }

    list.erase (list.begin () + idx, list.begin () + idx + num);
    list.emplace (list.begin () + idx, parent);
}
