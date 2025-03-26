#include "_Semaphore.h"

/* -------------------------- Globals ------------------------------------- */

static int nextsem_id = 0; /* next semaphore id to use */

/* ------------------------- Definitions ----------------------------------- */

/**
 * @brief Initializes the semaphore table and its empty list.
 *
 * @param pSemTable - Pointer to the semaphore table to initialize.
 * @param pFreeList - Pointer to the free list to initialize.
 */
void InitializeSemTable(SemData *pSemTable, DSL_List *pFreeList)
{

    /* Build the args for Init Static Storage w Data*/
    DSL_InitStaticStorageListArgs semTableArgs = {
        .data = pSemTable,
        .pList = pFreeList,
        .maxItems = MAX_SEMS,
        .orderFunction = NULL,
        .offset = OFFSETOF_SEM_DATA,
        .structSize = sizeof(SemData),
        .indexOffset = OFFSETOF_SEM_DATA_INDEX,
    };

    /* Init the LL with good default values */
    DSL_InitList(0, OFFSETOF_SEM_DATA, pFreeList, NULL);

    /* Add the data to the list (note: probably should rename the DSL function)*/
    DSL_InitStaticStorageListWData(&semTableArgs);
}