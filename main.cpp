#include "data.hpp"
#include "tokenizer.hpp"
#include "bracketer.hpp"
#include "rootlevel.hpp"
#include "contextualizer.hpp"
#include "highlighter.hpp"
#include <sstream>

void disp (Token &root, int indent)
{
    std::printf ("%*sclass: %s", 4*indent, "", getTokenName (root.tokenClass).data ());
    if ((root.tokenClass & TK_CLASS_MASK) != TK_CLASS_COMMENT
    &&  !root.raw.empty ())
    {
        std::printf (" `%s`", root.raw.data ());
    }
    std::printf ("\n");

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
        tokenizer (input_file, root);

        std::vector<BracketOffence> bracketOffences = {};
        if (!bracketerValidate (root, bracketOffences))
        {
            for (auto offence : bracketOffences)
            {
                std::printf ("[!] Bracket mismatch\n");
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
            return 1;
        }

        Token newroot = bracketer (root);
        Token root3 = {};
        if (!rootlevel (newroot, root3))
        {
            std::printf ("[!] Root level analysis failed\n");
            return 1;
        }

        //contextualizer (root3);
        rootlevelContextualizer (root3);

        disp (root3, 0);

        highlighter (root3.subtokens);
    }

    return 0;
}
