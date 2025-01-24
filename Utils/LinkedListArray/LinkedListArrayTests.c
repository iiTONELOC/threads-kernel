/**
 * @file LinkedListArrayTests.c
 * @see LinkedListArray.h
 * @see LinkedListArray.c
 * @author Anthony Tropeano
 * @date  1/23/2025
 *
 * @brief This file contains the test functions for the LinkedListArray module.
 */

#ifndef LINKEDLISTARRAY_H
#include "LinkedListArray.h"
#endif

#ifndef IMPORT_ASSERT
#define IMPORT_ASSERT
#include <assert.h>
#endif

// __________________________ Structures __________________________

typedef struct _testData
{
    int number;
} TestData;

// __________________________ Function Prototypes __________________________

void TestInsertNode();
void TestRemoveNode();
void TestLinkedListArray();
int OrderFunction(void *pNode1, void *pNode2);
int GetNextEmptyIndex(LinkedListArray *pListArray);

// __________________________ GLobal  __________________________
LinkedListNode nodeBucket[5];
LinkedList testList = {0, NULL, NULL, NULL};
TestData testNumbers[5] = {{1}, {2}, {3}, {4}, {5}};
LinkedListArray listArray = {5, &testList, &nodeBucket[0]};

// __________________________ Function Definitions __________________________

int main(int argc, char *argv[])
{
    TestLinkedListArray();
    return 0;
}

/**
 * @brief Order function for the test data.
 *
 * @param pNode1 The first node to compare.
 * @param pNode2 The second node to compare.
 *
 * @return The difference between the two numbers.
 */
int OrderFunction(void *pNode1, void *pNode2)
{
    TestData *data1 = (TestData *)((LinkedListNode *)pNode1)->pData;
    TestData *data2 = (TestData *)((LinkedListNode *)pNode2)->pData;

    // descending order, the linked list test runs ascending order
    // so here we cover both bases as this function is passed to the
    // linked list's initialization function
    return data2->number - data1->number;
}

int TestGetNextEmptyIndex()
{
    InitializeLinkedListArray(&listArray, &testList, &nodeBucket[0], 5, OrderFunction);
    int nextEmptyIndex = -1;
    LinkedListNode *pCurrent = NULL;

    printf("TestGetNextEmptyIndex Started\n");
    // get the next empty index in the array
    nextEmptyIndex = GetNextEmptyIndex(&listArray);

    // assert it is zero
    assert(nextEmptyIndex == 0);

    // point a node to the first node in the array and set the data
    pCurrent = &listArray.pNodeStorage[nextEmptyIndex];

    // set the data of the node to some test data
    pCurrent->pData = &testNumbers[0];

    // insert the node into the linked list
    InsertNode(listArray.pLinkedList, pCurrent);

    // get the next empty index in the array
    nextEmptyIndex = GetNextEmptyIndex(&listArray);

    // assert it is one
    assert(nextEmptyIndex == 1);

    printf("TestGetNextEmptyIndex Passed\n");

    return 0;
}

void TestInsertNode()
{
    // initialize the linked list array
    InitializeLinkedListArray(&listArray, &testList, nodeBucket, 5, OrderFunction);

    printf("TestInsertNode Started\n");
    // insert the test data into the linked list array
    for (int i = 0; i < 5; i++)
    {
        InsertDataIntoLinkedListArray(&listArray, &testNumbers[i]);
        // assert the count of the linked list is correct
        assert(listArray.pLinkedList->count == i + 1);
    }

    printf("TestInsertNode Passed\n");
}

void TestRemoveNode()
{
    printf("TestRemoveNode Started\n");
    LinkedListNode *pCurrent;
    int startingCount = testList.count;
    // remove the test data from the linked list array
    for (int i = 0; i < 5; i++)
    {
        pCurrent = &listArray.pNodeStorage[i];
        // remove the node from the inked list array
        RemoveNodeFromLinkedListArray(&listArray, pCurrent);
        // assert the count of the linked list is correct
        assert(listArray.pLinkedList->count == startingCount - (i + 1));
    }
    printf("TestRemoveNode Passed\n");
}

void TestLinkedListArray()
{
    TestGetNextEmptyIndex();
    TestInsertNode();
    TestRemoveNode();
}
