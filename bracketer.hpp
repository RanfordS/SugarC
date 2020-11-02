#pragma once
#include "data.hpp"

struct BracketOffence
{
    uint8_t number;
    Token tokens[2];
};

extern bool bracketerValidate
(std::vector <Token> &list, std::vector <BracketOffence> &offenders);

extern Token bracketer (std::vector <Token> &list);

