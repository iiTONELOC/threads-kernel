#include "SchedulerUtils.h"

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
 * @brief Displays a row in the process table
 *
 * @param pProcess Pointer to the process to display
 */
void PrintProcessRow(Process *pProcess)
{
    char *statusStr = NULL;
    char statusBuffer[20] = {0};

    // Grab the Status String
    if (pProcess->status < NUM_PROCESS_STATES)
    {
        statusStr = STATUS_STRINGS[pProcess->status];
    }
    else
    {
        // Use the User Defined Number
        snprintf(statusBuffer, sizeof(statusBuffer), "%d", pProcess->status);
        statusStr = statusBuffer;
    }

    // Print process information
    console_output(0, PROCESS_TABLE_ROW_FORMAT,
                   pProcess->pid,
                   pProcess->pParent == NULL ? -1 : pProcess->pParent->pid,
                   pProcess->priority,
                   statusStr,
                   pProcess->pChildren.length,
                   pProcess->cpuTime,
                   pProcess->name);
}

/**
 * @brief Destroys a doubly linked node.
 *
 * This function destroys a doubly linked node
 * if it was dynamically allocated. Otherwise, it
 * resets the node's values.
 *
 * @param pNode Pointer to the DSL_Node structure.
 * @return Nothing
 */
void DestroyDoublyLinkedNode(DSL_Node *pNode)
{
    if (pNode == NULL)
    {
        return;
    }
    else if (pNode->dynamic == 1)
    {
        free(pNode);
    }
    else
    {
        DSL_InitNode(0, pNode, NULL);
    }
}

/**
 * @brief Creates a dynamic doubly linked node.
 *
 * This function creates a dynamically allocated doubly linked node.
 *
 * @param pData Pointer to the data the node holds.
 * @return Pointer to the created DoublyLinkedNode structure.
 */
DSL_Node *CreateDoublyLinkedNode(void *pData)
{
    DSL_Node *pNode = (DSL_Node *)malloc(sizeof(DSL_Node));

    if (pNode == NULL)
    {
        return NULL;
    }

    DSL_InitNode(1, pNode, pData);
    return pNode;
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

/**
 * @brief Print the process table
 *
 * @param usingTablePtr The process table to print
 * @param size The size of the process table
 * @param currentNumProcesses The current number of processes
 */
void PrintProcessTable(Process *usingTablePtr, int size, int currentNumProcesses)
{
    int i = 0;

    // Print header
    console_output(0, PROCESS_TABLE_HEADER_FORMAT,
                   "PID",
                   "Parent",
                   "Priority",
                   "Status",
                   "# Kids",
                   "CPUtime",
                   "Name");

    // if the table is full print the last process first
    if (currentNumProcesses == MAXPROC)
    {
        PrintProcessRow(&usingTablePtr[MAXPROC - 1]);
        // adjust the size to print the rest of the table
        size--;
    }

    // Loop through process table
    for (i; i < size; i++)
    {
        Process *pProcess = &usingTablePtr[i];
        // Skip empty process slots
        if (pProcess->context == NULL)
        {
            continue;
        }

        // Print process row
        PrintProcessRow(pProcess);
    }
}

/**
 * @brief Validate the k_spawn parameters
 *
 * @param name The name of the process
 * @param entryPoint The entry point of the process
 * @param arg The arguments to pass to the process
 * @param stacksize The size of the stack
 * @param priority The priority of the process
 * @param debugFlag The debug flag
 *
 * @return int The return value of the function
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