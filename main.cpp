#include <sstream>

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
        highlighter (newroot);
    }

    return 0;
}
