
#include <THREADSLib.h>
#include "UserProcess.h"

/* -------------------------------------- Definitions ----------------------------------- */

/**
 * @brief Resets the user process to its initial state.
 *
 * @param pUserProc - Pointer to the user process to reset.
 * @note this does not reset the tableIndex
 */
void ResetUserProcess(UserProcess *pUserProc)
{
    pUserProc->pid = 0;
    pUserProc->status = 0;
    pUserProc->priority = 0;
    pUserProc->privateMboxId = 0;
    pUserProc->startFunc = NULL;
    pUserProc->pParent = NULL;
    pUserProc->pNext = NULL;
    pUserProc->pPrev = NULL;
    pUserProc->pNextChild = NULL;
    pUserProc->pPrevChild = NULL;
    /* destroy list - do not delete any list nodes from memory*/
    DSL_DestroyList(&pUserProc->children, 0);
    /* Re-init the LL */
    DSL_InitList(0, OFFSETOF_USER_PROC_CHILD_NODES, &pUserProc->children, NULL);
}
