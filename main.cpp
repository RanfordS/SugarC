#include <cstdio>
#include <sstream>
#include "parse_initial.hpp"

std::string tokenClassNames[] =
{   [TOKEN_CLASS_NONE]          = "NONE"
,   [TOKEN_CLASS_COMMENT_LINE]  = "COMMENT_LINE"
,   [TOKEN_CLASS_COMMENT_BLOCK] = "COMMENT_BLOCK"
,   [TOKEN_CLASS_CHAR]          = "CHAR"
,   [TOKEN_CLASS_STRING]        = "STRING"
,   [TOKEN_CLASS_OPERATOR]      = "OPERATOR"
,   [TOKEN_CLASS_NUMBER]        = "NUMBER"
,   [TOKEN_CLASS_NOUN]          = "NOUN"
};

int main (int argc, char** argv)
{
    Options options = {};
    options.tabFixedSize = false;
    options.tabSize = 4;

    char* input_name = NULL;
    for (int i = 1; i < argc; ++i)
    {
        std::printf ("arg[%i] = %s\n",  i, argv[i]);
        //TODO: Handle compiler options
        if (argv[i][0] == '-')
        {
            int iarg;
            if (sscanf (argv[i], "-tabsize=%i", &iarg) == 1)
                options.tabSize = iarg;
            else if (sscanf (argv[i], "-tabfixed") == 0)
                options.tabFixedSize = true;
        }
        else
            input_name = argv[i];
    }
    if (!input_name)
    {
        printf ("no input file name specified\n");
        return 1;
    }

    FILE* input_file = fopen (input_name, "r");
    if (!input_file)
        std::printf ("could not open file: %s\n", input_name);
    else
    {
        std::vector<Token> list;
        bool success = parseInitial (input_file, &list, &options);

        if (!success)
            std::printf ("parseInitial failed\n");
        else
            std::printf ("parseInitial complete\n");

        for (size_t i = 0; i < list.size (); ++i)
        {
            Token t = list[i];
            std::printf ("[%3lu,%2lu] %13s - `%s`\n",
                    t.line, t.column, tokenClassNames[t.tClass].c_str (), t.raw.data ());
        }
    }

    fclose (input_file);

    return 0;
}
