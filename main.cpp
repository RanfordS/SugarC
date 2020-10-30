#include "alternate.hpp"
#include <sstream>

std::string tokenNames[] =
{   "TK_NONE"
,   "TK_OPERATOR"
,   "TK_COMMENT_LINE"
,   "TK_COMMENT_BLOCK"
,   "TK_LITERAL_CHAR"
,   "TK_LITERAL_STRING"
,   "TK_BRACKET_OPEN_ROUND"
,   "TK_BRACKET_OPEN_SQUARE"
,   "TK_BRACKET_OPEN_CURLY"
,   "TK_BRACKET_CLOSE_ROUND"
,   "TK_BRACKET_CLOSE_SQUARE"
,   "TK_BRACKET_CLOSE_CURLY"
,   "TK_BRACKET_BLOCK_ROUND"
,   "TK_BRACKET_BLOCK_SQUARE"
,   "TK_BRACKET_BLOCK_CURLY"
,   "TK_BRACKET_TERNARY"
,   "TK_NUMBER"
,   "TK_NUMBER_ZERO"
,   "TK_NUMBER_INT_SIGNED"
,   "TK_NUMBER_INT_UNSIGNED"
,   "TK_NUMBER_INT_SPECIFIED_SIGNED"
,   "TK_NUMBER_INT_SPECIFIED_UNSIGNED"
,   "TK_NUMBER_BINARY"
,   "TK_NUMBER_OCTAL"
,   "TK_NUMBER_HEXIDECIMAL"
,   "TK_NUMBER_DOUBLE"
,   "TK_NUMBER_FLOAT"
,   "TK_NUMBER_EXPONENTIAL_DOUBLE"
,   "TK_NUMBER_EXPONENTIAL_FLOAT"
,   "TK_NOUN"
,   "TK_NOUN_KEYWORD"
,   "TK_NOUN_TYPE"
,   "TK_NOUN_VARIABLE"
,   "TK_NOUN_OPERATOR"
,   "TK_CONTEXT_FILE"
,   "TK_CONTEXT_EXPRESSION_PREFIX"
,   "TK_CONTEXT_EXPRESSION_INFIX"
,   "TK_CONTEXT_EXPRESSION_SUFFIX"
,   "TK_CONTEXT_EXPRESSION_TERNARY"
,   "TK_CONTEXT_EXPRESSION_INDEX"
};

void disp (Token &root, int indent)
{
    std::printf ("%*sclass: %s\n", 4*indent, "", tokenNames[root.tokenClass].data ());
    if (root.tokenClass == TK_COMMENT_LINE
    ||  root.tokenClass == TK_COMMENT_BLOCK)
    {
        return;
    }

    if (!root.raw.empty ())
    {
        std::printf ("%*sraw: `%s`\n", 4*indent, "", root.raw.data ());
    }

    for (auto &child : root.subtokens)
    {
        disp (child, indent + 1);
    }
    if (root.subtokens.size () > 0)
    {
        std::printf ("%*s<<\n", 4*indent, "");
    }
}

int main (int argc, char** argv)
{
    /*
    Options options = {};
    options.tabFixedSize = false;
    options.tabSize = 4;
    */

    char* input_name = NULL;
    for (int i = 1; i < argc; ++i)
    {
        std::printf ("arg[%i] = %s\n",  i, argv[i]);
        //TODO: Handle compiler options
        if (argv[i][0] == '-')
        {
            /*
            int iarg;
            if (sscanf (argv[i], "-tabsize=%i", &iarg) == 1)
                options.tabSize = iarg;
            else if (sscanf (argv[i], "-tabfixed") == 0)
                options.tabFixedSize = true;
            */
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
        std::vector<Token> root;
        alternateparse (input_file, root);

        std::vector<BracketOffence> bracketOffences = {};
        if (!bracketsvalidator (root, bracketOffences))
        {
            for (auto offence : bracketOffences)
            {
                if (offence.number == 1)
                {
                    Token t = offence.tokens[0];
                    std::printf ("unpaired: [%lu,%lu] %s\n",
                            t.line, t.column, t.raw.data ());
                }
                else
                {
                    Token l = offence.tokens[0];
                    Token r = offence.tokens[1];
                    std::printf ("missmatch: [%lu,%lu] %s - [%lu,%lu] %s\n",
                            l.line, l.column, l.raw.data (),
                            r.line, r.column, r.raw.data ());
                }
            }
        }

        Token newroot = bracket (root);

        bool context = contextparse (newroot);

        disp (newroot, 0);

        //highlighter (newroot.subtokens);
    }

    return 0;
}
