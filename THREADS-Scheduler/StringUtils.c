#ifndef STRINGUTILS_H
#include "StringUtils.h"
#endif

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
