#include <sstream>
#include "parse_header.hpp"

std::string tokenClassNames[TK_COUNT] =
{   [TK_NONE]                   = "TK_NONE"
,   [TK_BRACKET_ROUND_OPEN]     = "TK_BRACKET_ROUND_OPEN"
,   [TK_BRACKET_SQUARE_OPEN]    = "TK_BRACKET_SQUARE_OPEN"
,   [TK_BRACKET_CURLY_OPEN]     = "TK_BRACKET_CURLY_OPEN"
,   [TK_BRACKET_ROUND_CLOSE]    = "TK_BRACKET_ROUND_CLOSE"
,   [TK_BRACKET_SQUARE_CLOSE]   = "TK_BRACKET_SQUARE_CLOSE"
,   [TK_BRACKET_CURLY_CLOSE]    = "TK_BRACKET_CURLY_CLOSE"
,   [TK_BRACKET_ROUND_BLOCK]    = "TK_BRACKET_ROUND_BLOCK"
,   [TK_BRACKET_SQUARE_BLOCK]   = "TK_BRACKET_SQUARE_BLOCK"
,   [TK_BRACKET_CURLY_BLOCK]    = "TK_BRACKET_CURLY_BLOCK"
,   [TK_COMMENT_LINE]           = "TK_COMMENT_LINE"
,   [TK_COMMENT_BLOCK]          = "TK_COMMENT_BLOCK"
,   [TK_LITERAL_CHAR]           = "TK_LITERAL_CHAR"
,   [TK_LITERAL_STRING]         = "TK_LITERAL_STRING"
,   [TK_LITERAL_NUMBER]         = "TK_LITERAL_NUMBER"
,   [TK_GENERIC_NOUN]           = "TK_GENERIC_NOUN"
,   [TK_OPERATOR]               = "TK_OPERATOR"
,   [TK_STATEMENT]              = "TK_STATEMENT"

,   [TK_INVALID]                = "TK_INVALID"
};

void displayTokens (Token* root, int indent)
{
    for (size_t i = 0; i < root->subTokens.size (); ++i)
    {
        Token* t = &root->subTokens[i];
        for (int j = 0; j < indent; ++j)
            std::printf ("    ");
        std::printf ("%-24s - `%s`\n",
                tokenClassNames[t->tClass].data (),
                t->raw.data ());
        if TK_ISBRACKET(t->tClass)
        {
            displayTokens (t, indent + 1);
        }
    }
}

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
        bool success = parseInitial (input_file, list, &options);

        if (!success)
            std::printf ("parseInitial failed\n");
        else
            std::printf ("parseInitial complete\n");

        std::printf ("checking brackets\n");
        std::printf (checkBrackets (list) ? "success\n" : "fail\n");

        Token root = {};
        parseBrackets (list, &root);

        std::printf ("\ntoken tree\n");
        displayTokens (&root, 1);
    }

    fclose (input_file);

    return 0;
}
