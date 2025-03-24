#pragma once

#ifdef DOUBLESEALIB_EXPORTS
#define DOUBLE_SEA_LIB_API __declspec(dllexport)
#else
#define DOUBLE_SEA_LIB_API __declspec(dllimport)
#endif // DOUBLESEALIB_EXPORTS

#ifndef DOUBLE_SEA_LIST_H
#define DOUBLE_SEA_LIST_H
#include <stddef.h>

// __________________________ Typedefs and Structures __________________________
/**
 * @brief OrderFunction is a function pointer type that is used to compare two nodes.
 *
 * @param pNode1 The first node to compare.
 * @param pNode2 The second node to compare.
 *
 * @return int Returns 1 if the first node is greater than the second node,
 * 		   -1 if the first node is less than the second node, and 0 if they are equal.
 */
typedef int (*OrderFunction)(void *pNode1, void *pNode2);

/**
 * @brief DSL_Node is a structure that represents a node in a doubly linked list.
 *
 * @param pData A void pointer to the data that the node holds.
 * @param pNext A void pointer to the next node in the list.
 * @param pPrev A void pointer to the previous node in the list.
 * @param dynamic A flag that indicates if the node is dynamic.
 */
typedef struct DSL_Node
{
	void *pData;
	void *pNext;
	void *pPrev;
	int dynamic;
} DSL_Node;

/**
 * @brief DSL_List is a structure that represents a list.
 *
 * @param pHead A void pointer to the head of the list.
 * @param pTail A void pointer to the tail of the list.
 * @param dynamic A flag that indicates if the list is dynamic.
 * @param length The number of nodes in the list.
 * @param offset The offset to the data in the node.
 * @param orderFunction A function pointer to the function that compares two nodes.
 */
typedef struct DSL_List
{
	void *pHead;
	void *pTail;
	int dynamic;
	size_t length;
	size_t offset;
	OrderFunction orderFunction;
} DSL_List;

/**
 * @brief DSL_InitStaticStorageListArgs is a structure that holds the arguments for the
 * 		  DSL_InitStaticStorageList function.
 *
 * @param data A void pointer to the nodes that the list will hold.
 * @param offset The offset to the pNext pointer in the node.
 * @param maxItems The max number of elements to add to the list.
 * @param pList A pointer to the list that will be initialized.
 * @param structSize The size of the structure that the list will hold.
 * @param indexOffset The offset to the indexOffset in the structure.
 * @param orderFunction A function pointer to the function that compares two nodes.
 */
typedef struct DSL_InitStaticStorageListArgs
{
	void *data;
	size_t offset;
	size_t maxItems;
	DSL_List *pList;
	size_t structSize;
	size_t indexOffset;
	OrderFunction orderFunction;
} DSL_InitStaticStorageListArgs;

typedef DSL_List DSL_DynamicList; // Alias for the dynamic list
typedef DSL_Node DSL_DynamicNode; // Alias for the dynamic node

// __________________________ Macros __________________________

#define OFFSETOF_DSL_NODE offsetof(DSL_Node, pNext) // Offset to the pNext field in the DSL_Node structure

// __________________________ Function Prototypes __________________________

/**
 * @brief Gets the pointer to the next node in a doubly linked list.
 *
 * This function gets the pointer to the next node in a doubly linked list.
 *
 * @param pNode Pointer to the current node.
 * @param offset Offset to the pNext field in the data structure.
 * @return Pointer to the next node.
 */
DOUBLE_SEA_LIB_API void **_GetNextPointer(void *pNode, size_t offset);

/**
 * @brief Gets the pointer to the previous node in a doubly linked list.
 *
 * This function gets the pointer to the previous node in a doubly linked list.
 *
 * @param pNode Pointer to the current node.
 * @param offset Offset to the pPrev field in the data structure.
 * @return Pointer to the previous node.
 */
DOUBLE_SEA_LIB_API void **_GetPrevPointer(void *pNode, size_t offset);

/**
 * @brief Gets the pointer to the data in a doubly linked list node.
 *
 * This function gets the pointer to the data in a doubly linked list node.
 *
 * @param pNode Pointer to the current node.
 * @param offset Offset to the pNext field in the data structure.
 * @return Pointer to the data.
 */
DOUBLE_SEA_LIB_API void **_GetDataPointer(void *pNode, size_t offset);

/**
 * @brief DSL_Pop removes the first node from the list and returns it
 * @param pFromList - A pointer to the list from which the node will be removed
 * @return void* A pointer to the removed node
 */
DOUBLE_SEA_LIB_API void *DSL_Pop(DSL_List *pFromList);

/**
 * @brief DSL_DestroyNode frees the memory of a node
 *
 * Destroys a linked list node and frees the memory that it occupies if it is dynamic.
 * Otherwise, the node is reset to default values but it's memory is not freed.
 *
 * @param pNode - A pointer to the node that will be destroyed
 */
DOUBLE_SEA_LIB_API void DSL_DestroyNode(DSL_Node *pNode);

/**
 * @brief DSL_Push adds a node to the end of the list
 *
 * Adds a node to the end of the list.
 *
 * @param pNode - A pointer to the node that will be added
 * @param pIntoList - A pointer to the list that the node will be added to
 */
DOUBLE_SEA_LIB_API void DSL_Push(void *pNode, DSL_List *pIntoList);

/**
 * @brief DSL_InitStaticStorageList initializes a list with data
 *
 * Initializes a list using a static array of nodes. The function initializes the
 * list, adds the nodes to it, and sets each node's index into the table for easy access.
 * This requires the stuct being added to have an indexOffset field, and for its offset in
 * bytes to be passed to the `pArgs->indexOffset` field.
 *
 * @param pArgs - A pointer to the arguments that will be used to initialize the list
 */
DOUBLE_SEA_LIB_API void DSL_InitStaticStorageListWData(DSL_InitStaticStorageListArgs *pArgs);

/**
 * @brief DSL_RemoveNode removes a node from a list
 *
 * Removes a node from a list.
 *
 * @param pNode - A pointer to the node that will be removed
 * @param pFromList - A pointer to the list from which the node will be removed
 */
DOUBLE_SEA_LIB_API void DSL_RemoveNode(void *pNode, DSL_List *pFromList);

/**
 * @brief DSL_InsertNode inserts a node into a list
 *
 * Inserts a node into a list.
 *
 * @param pNode - A pointer to the node that will be inserted
 * @param pIntoList - A pointer to the list that the node will be inserted into
 */
DOUBLE_SEA_LIB_API void DSL_InsertNode(void *pNode, DSL_List *pIntoList);

/**
 * @brief DSL_DestroyList destroys a list
 *
 * Destroys a list and frees the memory the struct occupies if it is dynamic.
 * Otherwise, the list is reset to default values but it's memory is not freed.
 * Destroying a list, will also destroy all of it's nodes if the cleanNodes flag is set.
 *
 * @param pList - A pointer to the list that will be destroyed
 * @param cleanNodes - A flag that indicates if the nodes of the list will be destroyed
 */
DOUBLE_SEA_LIB_API void DSL_DestroyList(DSL_List *pList, int cleanNodes);

/**
 * @brief DSL_FindNode finds a node in a list
 *
 * Finds a node in a list.
 *
 * @param pList - A pointer to the list that the node will be searched in
 * @param pWithData - A pointer to the data that the node holds
 * @return void** A pointer to the found node
 */
DOUBLE_SEA_LIB_API void **DSL_FindNode(DSL_List *pList, void *pWithData);

/**
 * @brief DSL_InitNode initializes a dynamic node
 *
 * Initializes a dynamic node with the given data.
 *
 * @param isDynamic - A flag that indicates if the node is dynamic
 * @param pNode - A pointer to the node that will be initialized
 * @param pWithData - A pointer to the data that the node will hold
 */
DOUBLE_SEA_LIB_API void DSL_InitNode(int isDynamic, DSL_Node *pNode, void *pWithData);

/**
 * @brief DSL_InitList initializes a list
 *
 * Initializes a list with the given offset and order function.
 *
 * @param isDynamic - A flag that indicates if the list is dynamic
 * @param offset - The offset to the pNext pointers in the nodes
 * @param pList - A pointer to the list that will be initialized
 * @param pOrderFunction - A function pointer to the function that compares two nodes
 */
DOUBLE_SEA_LIB_API void DSL_InitList(int isDynamic, size_t offset, DSL_List *pList, OrderFunction pOrderFunction);

#endif // DOUBLE_SEA_LIST_H
