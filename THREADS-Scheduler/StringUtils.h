#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#ifndef IMPORT_STDLIB
#define IMPORT_STDLIB
#include <stdlib.h>
#endif

/*_______________________Function Prototypes_______________________*/

/**
 * @brief Trims the right side of a string
 *
 * @param pString The string to trimS
 */
void TrimRight(char *pString);

/**
 * @brief Copy a string from source to destination
 *
 * @param pSource The source string
 * @param pDestination The destination string
 * @param size The size of the destination string
 */
void CopyString(char *pSource, char *pDestination, size_t size);
#endif
