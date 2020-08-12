#include <cstdio>
#include <vector>

#include "parse_general.hpp"

std::vector<const char*> tokenClassStrings =
{   "TOKEN_CLASS_NONE"
,   "TOKEN_CLASS_COMMENT"
,   "TOKEN_CLASS_NUMBER"
,   "TOKEN_CLASS_CHAR"
,   "TOKEN_CLASS_STRING"
,   "TOKEN_CLASS_OPERATOR"
,   "TOKEN_CLASS_NOUN"
,   "TOKEN_CLASS_UNKNOWN"
};

int main (int argc, char** argv)
{
    char* input_name = NULL;
    for (int i = 1; i < argc; ++i)
    {
        //TODO: Handle compiler options
        if (argv[i][0] != '-')
            input_name = argv[i];
        else
            printf ("Unhandled argument %s\n", argv[i]);
    }
    if (!input_name)
    {
        printf ("no input file name specified\n");
        return 1;
    }

    FILE* input_file = fopen (input_name, "r");
    if (!input_file)
        printf ("could not open file: %s\n", input_name);
    else
    {
        std::vector<TokenGeneral> tokens (0);
        bool success = parseGeneral (input_file, tokens);

        if (!success)
        {   std::printf ("compile failed\n");
            return 1;
        }

        for (size_t i = 0; i < tokens.size (); ++i)
        {
            //std::printf ("M1\n");
            const char* str = tokenClassStrings[tokens[i].tClass];
            //std::printf ("M2\n");
            std::printf ("[%3lu,%2lu] %s |%s|\n",
                    tokens[i].line, tokens[i].column, str, tokens[i].raw.data ());
            //std::printf ("M3\n");
        }
    }


    fclose (input_file);

    return 0;
}
