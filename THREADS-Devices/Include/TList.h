#pragma once


#ifdef THREADS_BUILD
#define LIB_SPEC __declspec(dllexport) 
#else
#define LIB_SPEC
#endif

typedef struct
{
    void* pNext;
    void* pPrev;
} TListNode;

typedef struct
{
    void* pHead;
    void* pTail;
    int count;
    int offset;  // offset of TListNode within the structure
    int (*OrderFunction)(void* pNode1, void* pNode2);
} TList;


/* ************************************************************************
   TListInitialize

   Purpose -      Initializes a TList type.
   Parameters -   nextPrevOffset - offset from beginning of structure to
                                   the next and previous pointers with
                                   the structure that makes up the nodes.
                  orderFunction - used for sorting the list.  
   Returns -      None
   Comments -     The order function return value determines the position
                  in the list that the structure is placed.  The list 
                  is maintained in asceding order based on this return value.
   ----------------------------------------------------------------------- */
LIB_SPEC void TListInitialize(TList* pTList, int nextPrevOffset,
    int (*orderFunction)(void* pNode1, void* pNode2));

/* ************************************************************************
   TListAddNode

   Purpose -      Adds a node to the end of the list.
   Parameters -   TList *pTList        -  pointer to the list
                  void *pStructToAdd -  pointer to the structure to add

   Returns -      none
   ----------------------------------------------------------------------- */
LIB_SPEC void TListAddNode(TList* pTList, void* pStructToAdd);

/* ************************************************************************
   TListAddNodeInOrder

   Purpose -      Adds a node to the list based on the order function
   Parameters -   TList *pTList        -  pointer to the list
                  void *pStructToAdd -  pointer to the structure to add
   Returns -      none
   ----------------------------------------------------------------------- */
LIB_SPEC void TListAddNodeInOrder(TList* pTList, void* pStructToAdd);

/* ************************************************************************
   TListRemoveNode

   Purpose -      Removes the specified node from the list.
   Parameters -   TList *pTList           -  pointer to the list
                  void *pStructToRemove -  pointer to the structure to remove
   Returns -      None
   ----------------------------------------------------------------------- */
LIB_SPEC void TListRemoveNode(TList* pTList, void* pStructToRemove);

/* ************************************************************************
   TListPopNode

   Purpose -      Removes the first node from the list and returns a pointer
                  to it.
   Parameters -   TList *pTList         -  pointer to the list
   Returns -      The function returns a pointer to the removed node.
   ----------------------------------------------------------------------- */
LIB_SPEC void* TListPopNode(TList* pTList);

/* ************************************************************************
   TListGetNextNode

   Purpose -      Gets the next node releative to current node.  Returns
                  the first node if pCurrentStucture is NULL
   Parameters -   TList *pTList             -  pointer to the list
                  void *pCurrentStucture  -  pointer to current struct
   Returns -      The function returns a pointer to the next structure.
   ----------------------------------------------------------------------- */
LIB_SPEC void* TListGetNextNode(TList* pTList, void* pCurrentStucture);

/* ************************************************************************
   TListGetPreviousNode

   Purpose -      Gets the previous node releative to current node.  Returns
                  the first node if pCurrentStucture is NULL
   Parameters -   TList *pTList             -  pointer to the list
                  void *pCurrentStucture  -  pointer to current struct
   Returns -      The function returns a pointer to the previous structure.
   ----------------------------------------------------------------------- */
LIB_SPEC void* TListGetPreviousNode(TList* pTList, void* pCurrentStucture);

/* ************************************************************************
   TListGetClosestNode

   Purpose -      Gets the closest node releative to current node.  Returns
                  the first node if pCurrentStucture is NULL
   Parameters -   TList *pTList             -  pointer to the list
                  void *pCurrentStucture  -  pointer to current struct
   Returns -      The function returns a pointer to the next structure.
   ----------------------------------------------------------------------- */
LIB_SPEC void* TListGetClosestNode(TList* pTList, void* pCurrentStucture);

