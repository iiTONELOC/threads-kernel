/**
 * @file LinkedListsTest.c
 * @see LinkedList.h
 * @author Anthony Tropeano
 * @date  1/23/2025
 *
 * @brief This file contains the test functions for the LinkedList module.
 */

#ifndef LINKEDLIST_H
#include "LinkedList.h"
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
int OrderFunction(void *pNode1, void *pNode2);
void TestInsertNode();
void TestRemoveNode();
void TestLinkedList();

// __________________________ GLobal  __________________________

LinkedList testList = {0, NULL, NULL, NULL};
TestData testNumbers[5] = {{1}, {2}, {3}, {4}, {5}};
// Avoid using Malloc
LinkedListNode nodeBucket[5] = {{NULL, NULL, &testNumbers[0]},
                                {NULL, NULL, &testNumbers[1]},
                                {NULL, NULL, &testNumbers[2]},
                                {NULL, NULL, &testNumbers[3]},
                                {NULL, NULL, &testNumbers[4]}};

// __________________________ Function Definitions __________________________

int main(int argc, char *argv[])
{
    TestLinkedList();
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

    return data1->number - data2->number;
}

void TestInsertNode()
{
    printf("TestInsertNode Started\n");
    for (int i = 0; i < 5; i++)
    {
        InsertNode(&testList, &nodeBucket[i]);

        assert(testList.count == i + 1);
        assert(testList.pHead == &nodeBucket[0]);
        assert(testList.pTail == &nodeBucket[i]);
        assert(testList.OrderFunction == OrderFunction);
    }
    printf("TestInsertNode Passed\n");
}

void TestRemoveNode()
{
    printf("TestRemoveNode Started\n");
    for (int i = 0; i < 5; i++)
    {
        RemoveNode(&testList, &nodeBucket[i]);
        assert(testList.count == 4 - i);

        if (i == 4) // Last node
        {
            assert(testList.pHead == NULL);
            assert(testList.pTail == NULL);
        }
        else
        {
            assert(testList.pHead == &nodeBucket[i + 1]);
            assert(testList.pTail == &nodeBucket[4]);
        }
        assert(testList.OrderFunction == OrderFunction);
    }
    printf("TestRemoveNode Passed\n");
}

void TestLinkedList()
{
    InitializeList(&testList, OrderFunction);
    TestInsertNode();
    TestRemoveNode();
}
