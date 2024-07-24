#define N_IMPLEMENTS nArguments
#define N_KERNEL
//-----------------------------------------------------------------------------
/**
   nArguments.cc
*/

#include <string.h>
#include "kernel/narguments.h"
#include "kernel/ndebug.h"
#include "kernel/ntypes.h"

//-----------------------------------------------------------------------------
/**
 */
nArguments::nArguments(char* args)
{
    n_assert(args != 0);

    arguments = 0;
    count = 0;

    if (strlen(args) > 0)
    {
        // Make a clone of `args'.
        char* argsClone = new char[strlen(args) + 1];
        n_assert(argsClone != 0);
        strcpy(argsClone, args);

        int i = 0;
        // Count arguments.
        char* token = strtok(argsClone, " ");
        while (token != 0)
        {
            i++;
            token = strtok(0, " ");
        }
        count = i;
        delete argsClone;

        // Create argument array.
        if (count > 0)
        {
            arguments = new char*[count];
            
            i = 0;
            token = strtok(args, " ");
            while (token != 0)
            {
                arguments[i] = new char[strlen(token) + 1];
                n_assert(arguments[i] != 0);
                strcpy(arguments[i], token);
                i++;
                token = strtok(0, " ");
            }
            n_assert(i == count);
        }
    }
}

//-----------------------------------------------------------------------------
/**
   Note: First argument (argv[0]) will be ignored since it is the program name
   itself.
 */
nArguments::nArguments(const char* args[], int num)
{
    n_assert(args != 0);
    n_assert(num >= 0);

    arguments = 0;
    count = 0;

    if (num > 1)
    {
        count = num - 1;
        arguments = new char*[count];
        n_assert(arguments != 0);
        for (int i = 1; i < num; i++)
        {
            n_assert(args[i] != 0);
            int length = strlen(args[i]);
            arguments[i - 1] = new char[length + 1];
            n_assert(arguments[i - 1] != 0);

            // Copy; remove spaces.
            int index = 0;
            for (int j = 0; j < length; j++)
            {
                if (args[i][j] != ' ')
                {
                    arguments[i - 1][index++] = args[i][j];
                }
            }
            arguments[i - 1][index] = '\0';
        }
    }
}

//-----------------------------------------------------------------------------
/**
 */
nArguments::~nArguments()
{
    for (int i = 0; i < count; i++)
    {
        n_assert(arguments[i] != 0);
        delete [] arguments[i];
    }

    if (arguments != 0) delete [] arguments;
}

//-----------------------------------------------------------------------------
/**
 */
int
nArguments::ArgumentCount() const
{
    return count;
}

//-----------------------------------------------------------------------------
/**
 */
const char* nArguments::Argument(int i) const
{
    n_assert(0 < i && i <= ArgumentCount ());

    n_assert(arguments != 0);
    n_assert(arguments[i - 1] != 0);
    return arguments[i - 1];
}
