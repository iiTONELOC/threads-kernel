# Linked List Array

```md
     +-------------------------+
     |      LinkedListNode     |
     +-------------------------+
     | pPrev   |   (Pointer)   |
     | pData   |   (Pointer)   |
     | pNext   |   (Pointer)   |
     +-------------------------+
     
    +-----------------------------------+
    |            LinkedList             |
    +-----------------------------------+
    | count         |       size_t      |
    | pHead         |     (Pointer)     |
    | pTail         |     (Pointer)     |
    | OrderFunction |   Function Ptr    |
    +-----------------------------------+

    LinkListNode LinkedListNodeArray[100]

    +---------------------------------------+
    |            LinkedListArray            |
    +---------------------------------------+
    | nodeCount     |        size_t         |
    | pLinkedList   |      *LinkedList      |
    | pNodeStorage  | *LinkedListNodeArray  |
    +---------------------------------------+

    +------------------------------+
    |      LinkedListNodeArray     |
    +------------------------------+
    | +--------+--------+--------+ |
    | |  Prev  |  Data  |  Next  | |  Node 0
    | +--------+--------+--------+ |
    | +--------+--------+--------+ |
    | |  Prev  |  Data  |  Next  | |  Node 1
    | +--------+--------+--------+ |
    | +--------+--------+--------+ |
    | |  Prev  |  Data  |  Next  | |  Node 2
    | +--------+--------+--------+ |
    |             ...              |
    | +--------+--------+--------+ |
    | |  Prev  |  Data  |  Next  | |  Node 99
    | +--------+--------+--------+ |
    +------------------------------+

```

## Overview

This file contains the implementation of a doubly linked sorted list that uses an array of LinkedListNode structs for storage rather than dynamic memory allocation when a new node is needed.

The primary benefit of this approach is to minimize dynamic memory allocation (such as malloc and free) by preallocating a fixed array of nodes

## Table of Contents

- [Linked List Array](#linked-list-array)
  - [Overview](#overview)
  - [Table of Contents](#table-of-contents)
  - [Structures](#structures)
  - [Function Definitions](#function-definitions)
    - [1. `InitializeLinkedListArray`](#1-initializelinkedlistarray)
      - [Description:](#description)
      - [Parameters:](#parameters)
    - [2. `GetNextEmptyIndex`](#2-getnextemptyindex)
      - [Description:](#description-1)
      - [Parameters:](#parameters-1)
      - [Return:](#return)
    - [3. `InsertDataIntoLinkedListArray`](#3-insertdataintolinkedlistarray)
      - [Description:](#description-2)
      - [Parameters:](#parameters-2)
      - [Return:](#return-1)
    - [4. `RemoveNodeFromLinkedListArray`](#4-removenodefromlinkedlistarray)
      - [Description:](#description-3)
      - [Parameters:](#parameters-3)
      - [Return:](#return-2)
  - [Building the Tests](#building-the-tests)

## Structures

```c
typedef struct _linkedListNode
{
    void *pNext;
    void *pPrev;
    void *pData;
} LinkedListNode;

typedef struct _linkedList
{
    size_t count;
    void *pHead;
    void *pTail;
    int (*OrderFunction)(void *pNode1, void *pNode2);
} LinkedList;

typedef struct _linkedListArray
{
    size_t nodeCount;             // The number of nodes in the array
    LinkedList *pLinkedList;      // The linked list structure
    LinkedListNode *pNodeStorage; // The array of nodes
} LinkedListArray;
```

## Function Definitions

### 1. `InitializeLinkedListArray`

```c
void InitializeLinkedListArray(LinkedListArray *pListArray, LinkedList *pList,
                               LinkedListNode *pNodeStorage, size_t nodeCount,
                               int (*OrderFunction)(void *pNode1, void *pNode2));
```

#### Description:

This function initializes a linked list array by setting its pointer to the provided linked list, the count of nodes, and the array of nodes. It also initializes the linked list and sets the order function for the list. The nodes in the node storage array are initialized to `NULL`.

#### Parameters:

- `pListArray` _(LinkedListArray *)_: Pointer to the linked list array to be initialized.
- `pList` _(LinkedList *)_: Pointer to the linked list to be used.
- `pNodeStorage` _(LinkedListNode *)_: Pointer to an array of nodes to store the data.
- `nodeCount` _(size_t)_: The number of nodes available in the storage array.
- `OrderFunction` _(int (*)(void *, void *))_: A function pointer for the ordering logic of the list.

---

### 2. `GetNextEmptyIndex`

```c
int GetNextEmptyIndex(LinkedListArray *pListArray);
```

#### Description:

This function scans through the node storage array to find the index of the next empty node. It returns the index of the first empty node found.

#### Parameters:

- `pListArray` _(LinkedListArray *)_: Pointer to the linked list array to check for an empty node.

#### Return:

- Returns the index of the first empty node, or -1 if no empty nodes are found.

---

### 3. `InsertDataIntoLinkedListArray`

```c
int InsertDataIntoLinkedListArray(LinkedListArray *pListArray, void *pData);
```

#### Description:

This function inserts data into the linked list array by finding the next available empty index, setting the node's data, and inserting the node into the linked list.

#### Parameters:

- `pListArray` _(LinkedListArray *)_: Pointer to the linked list array to insert the data into.
- `pData` _(void *)_: Pointer to the data to be inserted.

#### Return:

- Returns the index of the inserted node, or -1 if there is no available space.

---

### 4. `RemoveNodeFromLinkedListArray`

```c
int RemoveNodeFromLinkedListArray(LinkedListArray *pListArray, LinkedListNode *pNode);
```

#### Description:

This function removes a node from the linked list array by finding it in the node storage array, removing it from the linked list, and resetting the node to its default values.

#### Parameters:

- `pListArray` _(LinkedListArray *)_: Pointer to the linked list array from which to remove the node.
- `pNode` _(LinkedListNode *)_: Pointer to the node to be removed.

#### Return:

- Returns the index of the removed node, or -1 if the node was not found.

## Building the Tests

A small unit test file has been included for (T)est (D)riven (D)evelopment.

The module can be built using the provided `.\make.ps1` PowerShell script will build the object files and generate the executable for the tests.

To run the build, navigate to the root of the Linked List Directory and run the make script:

```bash
.\make.ps1
```

To run the tests use the provided `.\test.ps1` script (after running the build) or call the `LinkedListTest.exe` directly:

```bash
.\out\LinkedListArrayTests.exe

# or

.\test.ps1

-----------------------------

# expected output
TestGetNextEmptyIndex Started
TestGetNextEmptyIndex Passed
TestInsertNode Started
TestInsertNode Passed
TestRemoveNode Started
TestRemoveNode Passed
````
