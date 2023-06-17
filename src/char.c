#include "char.h"



bool charIsLetter (char c)
{
    return (('a' <= c) && (c <= 'z'))
        || (('A' <= c) && (c <= 'Z'))
        ||  ('_' == c);
}



bool charIsNumber (char c)
{
    return ('0' <= c) && (c <= 'z');
}




// EOF //
