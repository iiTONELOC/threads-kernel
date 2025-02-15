#include "Utils.h"

/*_______________________Function Definitions_______________________*/

/**
 * @brief Trims the right side of a string
 *
 * @param pString Pointer to the string to trim
 */
void TrimRight(char *pString)
{
    int i = 0;
    // while we haven't reached the end of the string
    while (pString[i] != '\0')
    {
        // traverse to the end of the string
        i++;
    }

    // end of the string was found
    i--;

    // loop backwards until we find a character that is not a space, tab, newline, or carriage return
    while (pString[i] == ' ' || pString[i] == '\t' || pString[i] == '\n' || pString[i] == '\r')
    {
        // null terminate the String
        pString[i] = '\0';
        i--;
    }
}

/**
 * @brief Copy a string from source to destination
 *
 * @param pSource Pointer to the source string
 * @param pDestination Pointer to the destination string
 * @param size The size of the destination string
 */
void CopyString(char *pSource, char *pDestination, size_t size)
{
    // size_t is an unsigned long long integer
    unsigned long long i = 0ULL;

    // if the source string is NULL, an empty string is copied to the destination
    if (pSource == NULL)
    {
        pDestination[0] = '\0';
        return;
    }

    // Trim the right side of the string so we don't copy garbage
    TrimRight(pSource);

    // Traverse the source string, copying each character to the destination string
    // for a maximum of size - 1 characters
    while (pSource[i] != '\0' && i < size - 1)
    {
        // copy the character
        pDestination[i] = pSource[i];
        i++;
    }
    // ensure the destination string is null terminated
    pDestination[i] = '\0';
}

/**
 * @brief Validate the parameters for k_spawn
 *
 * @param name The name of the process
 * @param entryPoint The entry point of the process
 * @param arg The arguments for the process
 * @param stacksize The size of the stack
 * @param priority The priority of the process
 * @param debugFlag The debug flag
 * @return int 0 if the parameters are valid, -1 if the name is NULL, -2 if the name is too long
 */
int ValidateKSpawnParams(char *name, int (*entryPoint)(void *), void *arg, int stacksize,
                         int priority, int debugFlag)
{

    /*Validate all of the parameters, starting with the name. */
    if (name == NULL)
    {
        console_output(debugFlag, "spawn(): Name value is NULL.\n");
        return -1;
    }
    if (strlen(name) >= (MAXNAME - 1))
    {
        console_output(debugFlag, "spawn(): Process name is too long.  Halting...\n");
        stop(1);
    }

    if (arg != NULL && strlen((char *)arg) >= (MAXARG - 1))
    {
        console_output(debugFlag, "spawn(): Process arg is too long.  Halting...\n");
        stop(1);
    }

    if (entryPoint == NULL)
    {
        console_output(debugFlag, "spawn(): entryPoint is NULL.\n");
        return -1;
    }

    if (stacksize < THREADS_MIN_STACK_SIZE)
    {
        console_output(debugFlag, "spawn(): Stack size is too small\n");
        return -2;
    }

    if (priority < LOWEST_PRIORITY || priority > HIGHEST_PRIORITY)
    {
        console_output(debugFlag, "spawn(): Priority out of range.\n");
        return -3;
    }

    return 0;
}
