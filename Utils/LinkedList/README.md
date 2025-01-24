# LinkedList.c

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

    +--------+--------+--------+     +--------+--------+--------+     +--------+--------+--------+
    |  Prev  |  Data  |  Next  |<--->|  Prev  |  Data  |  Next  |<--->|  Prev  |  Data  |  Next  |
    +--------+--------+--------+     +--------+--------+--------+     +--------+--------+--------+
```

## Overview

This file contains the implementation for a generic doubly linked list in C. The linked list is designed to store data in nodes, with the ability to insert and remove nodes in a sorted order based on a user-defined comparison function. The implementation supports basic linked list operations such as initialization, insertion, removal, and printing of the list.

The code is adapted from Professor Duren's skeleton and Anthony Tropeano's doubly linked list implementation from the CYBV470 - Final Exercise 3 (originally submitted on 12/8/2024).

## Table of Contents

- [LinkedList.c](#linkedlistc)
  - [Overview](#overview)
  - [Table of Contents](#table-of-contents)
  - [Structures](#structures)
  - [Function Definitions](#function-definitions)
    - [1. `InitializeList`](#1-initializelist)
      - [Description:](#description)
      - [Parameters:](#parameters)
    - [2. `InitializeNode`](#2-initializenode)
      - [Description:](#description-1)
      - [Parameters:](#parameters-1)
      - [Behavior:](#behavior)
    - [2. `InsertNode`](#2-insertnode)
      - [Description:](#description-2)
      - [Parameters:](#parameters-2)
      - [Behavior:](#behavior-1)
    - [3. `RemoveNode`](#3-removenode)
      - [Description:](#description-3)
      - [Parameters:](#parameters-3)
      - [Behavior:](#behavior-2)
    - [4. `PrintList`](#4-printlist)
      - [Description:](#description-4)
      - [Parameters:](#parameters-4)
      - [Behavior:](#behavior-3)
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
```

## Function Definitions

### 1. `InitializeList`

```c
void InitializeList(LinkedList *pList, int (*OrderFunction)(void *pNode1, void *pNode2));
```

#### Description:

This function initializes a doubly linked list by setting its count to 0, and both its head and tail pointers to `NULL`. It also sets the `OrderFunction`, which is used to determine the position of nodes when inserting them into the list.

#### Parameters:

- `pList` _(LinkedList *)_: Pointer to the linked list to be initialized.
- `OrderFunction` _(int (*)(void *, void *))_: A function pointer that defines the ordering of nodes in the list. If `NULL`, nodes will be inserted at the end of the list.

### 2. `InitializeNode`

```c
void InitializeNode(LinkedListNode *pNode)
```

#### Description:

This function initializes a single node to all NULL values

#### Parameters:

- `pList` _(LinkedListNode *)_: Pointer to the linked list node to initialize.

#### Behavior:

- Returns NULL if given a NULL pointer


### 2. `InsertNode`

```c
void InsertNode(LinkedList *pList, void *pNode);
```

#### Description:

This function inserts a node into the linked list. It places the new node in its correct position based on the provided `OrderFunction`. If the `OrderFunction` is `NULL`, the node is inserted at the end of the list.

#### Parameters:

- `pList` _(LinkedList *)_: Pointer to the linked list.
- `pNode` _(void *)_: Pointer to the node to be inserted.

#### Behavior:

- If the list is empty, the node becomes both the head and the tail.
- If the `OrderFunction` is provided, the function will compare the node with the existing nodes to insert it in the correct position.
- If no order function is provided, the node will be added to the end of the list.

### 3. `RemoveNode`

```c
void RemoveNode(LinkedList *pList, void *pNode);
```

#### Description:

This function removes a node from the linked list. It correctly updates the list's head and tail pointers when the node to be removed is either the first or last node in the list.

#### Parameters:

- `pList` _(LinkedList *)_: Pointer to the linked list.
- `pNode` _(void *)_: Pointer to the node to be removed.

#### Behavior:

- If the node is the head, the head pointer is updated to the next node.
- If the node is the tail, the tail pointer is updated to the previous node.
- If the node is somewhere in between, the previous and next pointers of the neighboring nodes are updated to remove the node.
- If the list becomes empty after the removal, both the head and tail pointers are set to `NULL`.

### 4. `PrintList`

```c
void PrintList(LinkedList *pList);
```

#### Description:

This function prints the contents of the linked list to the standard output. For each node in the list, it prints the node's address, the next node's address, the previous node's address, and the data pointer contained in the node.

#### Parameters:

- `pList` (LinkedList *): Pointer to the linked list to be printed.

#### Behavior:

- It starts at the head of the list and traverses through each node, printing the information for each one.
- The loop continues until it reaches the end of the list (when the current node is `NULL`).

## Building the Tests

A small unit test file has been included for (T)est (D)riven (D)evelopment.

The module can be built using the provided `.\make.ps1` PowerShell script will build the object files and generate the executable for the tests.

To run the build, navigate to the root of the Linked List Directory and run the make script:

```bash
.\make.ps1
```

To run the tests use the provided `.\test.ps1` script (after running the build) or call the `LinkedListTest.exe` directly:

```bash
.\out\LinkedListsTest.exe

# or

.\test.ps1

---------------------------

# expected output
TestInsertNode Started
TestInsertNode Passed
TestRemoveNode Started
TestRemoveNode Passed
```
