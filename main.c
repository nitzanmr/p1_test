#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"
#include "pattern_matching.h"

#define TEST_HAS_FAILED 1
#define TEST_HAS_SUCCEEDED 0

int InitTest()
{
    dbllist_t* list = (dbllist_t*)malloc(sizeof(dbllist_t));

    if (!list)
    {
        perror("error: ");
        return TEST_HAS_FAILED;
    }

    dbllist_init(list);
    dbllist_destroy(list, DBLLIST_LEAVE_DATA);
    free(list);
    list = NULL;

    return 0;
}


int InsertTest()
{
    dbllist_t* list = (dbllist_t*)malloc(sizeof(dbllist_t));
    int expected[] = {
        9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    };

    if (!list)
    {
        perror("error: ");
        return TEST_HAS_FAILED;
    }

    dbllist_init(list);
    size_t numOfPrepends = 10;
    
    for (size_t i = 0; i < numOfPrepends; i++)
    {   
        // prepend adds an element at the start of the list
        // therefore, the index is from <numOfPrepends> to 0.
        dbllist_prepend(list,(void*)&expected[numOfPrepends - i - 1]);

        for (size_t j = 0; j < numOfPrepends; j++)
        {
            // append adds an element at the end of the list
            // therefore the index starts from <numOfPrepends>
            dbllist_append(list, (void*)&expected[numOfPrepends * (i + 1) + j]);
        }
    }
    
    int isInPlace       = TEST_HAS_SUCCEEDED;
    dbllist_node_t* ptr = dbllist_head(list);

    for (size_t i = 0; i < sizeof(expected) / sizeof(int); i++)
    {
        int data = *((int*)dbllist_data(ptr));
        
        if (data != expected[i])
        {
            isInPlace = TEST_HAS_FAILED;
            break;
        }

        ptr = dbllist_next(ptr);
    }

    dbllist_destroy(list, DBLLIST_LEAVE_DATA);
    free(list);
    list = NULL;

    return isInPlace;
}


int LeaveDestroyTest()
{
    dbllist_t* list = (dbllist_t*)malloc(sizeof(dbllist_t));

    if (!list)
    {
        perror("error: ");
        return TEST_HAS_FAILED;
    }

    int* dataArray[5];
    
    for (size_t i = 0; i < 5; i++)
    {
        dataArray[i] = (int*)malloc(sizeof(int));

        if (!dataArray[i])
        {
            dbllist_destroy(list, DBLLIST_FREE_DATA);
            free(list);
            list = NULL;

            perror("error: ");
            return TEST_HAS_FAILED;
        }

        dataArray[i][0] = (int)i;
        dbllist_append(list, (void*)dataArray[i]);
    }

    dbllist_destroy(list, DBLLIST_LEAVE_DATA);
    list = NULL;

    int isData = TEST_HAS_SUCCEEDED;

    for (size_t i = 0; i < 5; i++)
    {
        if (!dataArray[i])
        {
            isData = TEST_HAS_FAILED;
            break;
        }
        
        if (dataArray[i][0] != i)
        {
            isData = TEST_HAS_FAILED;
            break;
        }
    }

    for (size_t i = 0; i < 5; i++)
    {
        free(dataArray[i]);
        dataArray[i] = NULL;
    }

    return isData;
}


int FreeDestroyTest()
{
    dbllist_t* list = (dbllist_t*)malloc(sizeof(dbllist_t));

    if (!list)
    {
        perror("error: ");
        return TEST_HAS_FAILED;
    }

    int* dataArray[5];
    
    for (size_t i = 0; i < 5; i++)
    {
        dataArray[i] = (int*)malloc(sizeof(int));

        if (!dataArray[i])
        {
            free(list);
            list = NULL;
            perror("error: ");
            return TEST_HAS_FAILED;
        }

        dataArray[i][0] = (int)i + 1;
        dbllist_append(list, (void*)dataArray[i]);
    }

    dbllist_destroy(list, DBLLIST_FREE_DATA);
    list = NULL;

    int isData = TEST_HAS_SUCCEEDED;

    for (size_t i = 0; i < 5; i++)
    {
        if (dataArray[i] != NULL)
        {
            if (dataArray[i][0] == i + 1)
            {
                isData = TEST_HAS_FAILED;
                break;
            }
        }

    }

    return isData;
}


int RemoveTest()
{
    dbllist_t* listEven = (dbllist_t*)malloc(sizeof(dbllist_t));
    dbllist_t* listOdd  = (dbllist_t*)malloc(sizeof(dbllist_t));

    int evenListData[] = { 3, 2, 1, 0 };
    int oddListData[]  = { 0, 1, 2, 3, 4 };

    if (!listEven || !listOdd)
    {
        perror("error: ");
        return TEST_HAS_FAILED;
    }

    // even size list
    for (size_t i = 0; i < sizeof(evenListData) / sizeof(int); i++)
    {
        dbllist_append(listEven, (void*)&evenListData[i]);
    }

    int expectedEven[]              = {3, 2, 0};

    dbllist_node_t* removeMe    = dbllist_prev(dbllist_tail(listEven));
    dbllist_remove(listEven, removeMe, DBLLIST_LEAVE_DATA);
    dbllist_node_t* ptr         = dbllist_head(listEven);

    for (size_t i = 0; i < 3; i++)
    {
        int data = *((int*)dbllist_data(ptr));

        if (data != expectedEven[i])
        {
            dbllist_destroy(listEven, DBLLIST_LEAVE_DATA);
            dbllist_destroy(listOdd, DBLLIST_LEAVE_DATA);

            free(listEven);
            free(listOdd);

            listEven = NULL;
            listOdd = NULL;

            return TEST_HAS_FAILED;
        }
        
        ptr = dbllist_next(ptr);
    }

    int expectedEven_1[]  = {2, 0};


    removeMe    = dbllist_head(listEven);
    dbllist_remove(listEven, removeMe, DBLLIST_LEAVE_DATA);
    ptr         = dbllist_head(listEven);

    for (size_t i = 0; i < 2; i++)
    {
        int data = *((int*)dbllist_data(ptr));

        if (data != expectedEven_1[i])
        {
            dbllist_destroy(listEven, DBLLIST_LEAVE_DATA);
            dbllist_destroy(listOdd, DBLLIST_LEAVE_DATA);

            free(listEven);
            free(listOdd);

            listEven = NULL;
            listOdd = NULL;
            
            return TEST_HAS_FAILED;
        }
        
        ptr = dbllist_next(ptr);
    }

    // let's keep the check within a loop so we can play with the size later on.
    int expectedEven_2[]  = { 2 };

    removeMe    = dbllist_tail(listEven);
    dbllist_remove(listEven, removeMe, DBLLIST_LEAVE_DATA);
    ptr         = dbllist_head(listEven);

    for (size_t i = 0; i < 1; i++)
    {
        int data = *((int*)dbllist_data(ptr));

        if (data != expectedEven_2[i])
        {
            dbllist_destroy(listEven, DBLLIST_LEAVE_DATA);
            dbllist_destroy(listOdd, DBLLIST_LEAVE_DATA);

            free(listEven);
            free(listOdd);

            listEven = NULL;
            listOdd = NULL;
            
            return TEST_HAS_FAILED;
        }
        
        ptr = dbllist_next(ptr);
    }

    // odd size list
    for (size_t i = 0; i < 5; i++)
    {
        dbllist_append(listOdd, (void*)&oddListData[i]);
    }
    
    int expectedOdd[]           = {0, 1, 2, 4};

    removeMe    = dbllist_prev(dbllist_tail(listOdd));
    dbllist_remove(listOdd, removeMe, DBLLIST_LEAVE_DATA);
    ptr         = dbllist_head(listOdd);

    for (size_t i = 0; i < 4; i++)
    {
        int data = *((int*)dbllist_data(ptr));

        if (data != expectedOdd[i])
        {
            dbllist_destroy(listEven, DBLLIST_LEAVE_DATA);
            dbllist_destroy(listOdd, DBLLIST_LEAVE_DATA);

            free(listEven);
            free(listOdd);

            listEven = NULL;
            listOdd = NULL;
            
            return TEST_HAS_FAILED;
        }
        
        ptr = dbllist_next(ptr);
    }

    int expectedOdd_1[]         = {1, 2, 4};

    removeMe    = dbllist_head(listOdd);
    dbllist_remove(listOdd, removeMe, DBLLIST_LEAVE_DATA);
    ptr         = dbllist_head(listOdd);

    for (size_t i = 0; i < 3; i++)
    {
        int data = *((int*)dbllist_data(ptr));

        if (data != expectedOdd_1[i])
        {
            dbllist_destroy(listEven, DBLLIST_LEAVE_DATA);
            dbllist_destroy(listOdd, DBLLIST_LEAVE_DATA);

            free(listEven);
            free(listOdd);

            listEven = NULL;
            listOdd = NULL;
            
            return TEST_HAS_FAILED;
        }
        
        ptr = dbllist_next(ptr);
    }

    int expectedOdd_2[]         = {1, 2};

    removeMe    = dbllist_tail(listOdd);
    dbllist_remove(listOdd, removeMe, DBLLIST_LEAVE_DATA);
    ptr         = dbllist_head(listOdd);

    for (size_t i = 0; i < 2; i++)
    {
        int data = *((int*)dbllist_data(ptr));
        
        if (data != expectedOdd_2[i])
        {
            dbllist_destroy(listEven, DBLLIST_LEAVE_DATA);
            dbllist_destroy(listOdd, DBLLIST_LEAVE_DATA);

            free(listEven);
            free(listOdd);

            listEven = NULL;
            listOdd = NULL;
            
            return TEST_HAS_FAILED;
        }
        
        ptr = dbllist_next(ptr);
    }

    dbllist_destroy(listEven, DBLLIST_LEAVE_DATA);
    dbllist_destroy(listOdd, DBLLIST_LEAVE_DATA);

    free(listEven);
    free(listOdd);

    listEven = NULL;
    listOdd = NULL;

    return TEST_HAS_SUCCEEDED;
}


int ForwardTransitionTest()
{
    char* patterns[4]   = { "f0ur", "f0rward", "7trans1tions", "f0rk" };
    size_t pSize        = 4;

    pm_t* pm = (pm_t*)malloc(sizeof(pm_t));

    if (!pm)
    {
        perror("error: ");
        return TEST_HAS_FAILED;
    }

    pm_init(pm);

    for (size_t i = 0; i < pSize; i++)
        pm_addstring(pm, (unsigned char*)patterns[i], strlen(patterns[i]));

    for (size_t i = 0; i < pSize; i++)
    {
        unsigned char   symbol      = (unsigned char)patterns[i][0];
        pm_state_t*     current     = pm_goto_get(pm->zerostate, symbol);

        if (!current)
        {
            pm_destroy(pm);
            free(pm);
            pm = NULL;

            return TEST_HAS_FAILED;
        }

        for (size_t j = 1; patterns[i][j] != '\0'; j++)
        {
            symbol      = (unsigned char)patterns[i][j];
            current     = pm_goto_get(current, symbol);

            if (!current)
            {
                pm_destroy(pm);
                free(pm);
                pm = NULL;

                return TEST_HAS_FAILED;
            }
        }
    }

    pm_destroy(pm);
    free(pm);
    pm = NULL;

    return TEST_HAS_SUCCEEDED;
}


int FailureTransitionTest()
{
    char*   patterns[5]         = { "fa1lur3", "trans1t1ons", "t3st", "1s", "3asy" };
    int     expectedFailIds[27] = { 0, 0, 22, 0, 0, 0, 24, 0, 0, 0, 0, 0, 22, 8, 22, 0, 0, 0, 24, 0, 8, 0, 0, 0, 0, 0, 0 };
    size_t  pSize               = 5;

    pm_t* pm = (pm_t*)malloc(sizeof(pm_t));

    if (!pm)
    {
        perror("error: ");
        return TEST_HAS_FAILED;
    }

    pm_init(pm);

    for (size_t i = 0; i < pSize; i++)
        pm_addstring(pm, (unsigned char*)patterns[i], strlen(patterns[i]));

    pm_makeFSM(pm);

    for (size_t i = 0; i < pSize; i++)
    {
        pm_state_t* current = pm->zerostate;

        for (size_t j = 0; patterns[i][j] != '\0'; j++)
        {   
            current = pm_goto_get(current, (unsigned char)patterns[i][j]);

            if (!current)
            {
                pm_destroy(pm);
                free(pm);
                pm = NULL;

                return TEST_HAS_FAILED;
            }

            if (current->fail->id != expectedFailIds[current->id - 1])
            {
                pm_destroy(pm);
                free(pm);
                pm = NULL;

                return TEST_HAS_FAILED;
            }
        }
    }

    pm_destroy(pm);
    free(pm);
    pm = NULL;

    return TEST_HAS_SUCCEEDED;
}


int PatternScanTest()
{
    char    text[]                      = "AABAACAADAABAABA";
    char*   patterns[]                  = { "AABA", "BAA", "DAABAA", "BA", "DA" };
    int     expectedPositions[10][2]    = { {0, 3}, {12, 15}, {9, 12}, {11, 13}, {2, 4}, {11, 12}, {14, 15}, {2, 3}, {8, 13}, {8, 9} };
    size_t pSize = 5, eSize = 10, foundMatch = 0;

    pm_t*   pm  = (pm_t*)malloc(sizeof(pm_t));

    if (!pm)
    {
        perror("error: ");
        return TEST_HAS_FAILED;
    }

    pm_init(pm);

    for (size_t i = 0; i < pSize; i++)
        pm_addstring(pm, (unsigned char*)patterns[i], strlen(patterns[i]));

    pm_makeFSM(pm);

    dbllist_t* matchedPatterns = pm_fsm_search(pm->zerostate, (unsigned char*)text, strlen(text));

    if (dbllist_size(matchedPatterns) != eSize)
    {
        pm_destroy(pm);
        dbllist_destroy(matchedPatterns, DBLLIST_FREE_DATA);

        free(pm);
        free(matchedPatterns);

        pm              = NULL;
        matchedPatterns = NULL;

        return TEST_HAS_FAILED;
    }

    for (dbllist_node_t* ptr = dbllist_head(matchedPatterns); ptr != NULL; ptr = dbllist_next(ptr))
    {
        pm_match_t* match = (pm_match_t*)dbllist_data(ptr);

        for (size_t i = 0; i < eSize; i++)
        {   
            // check if the current match is one of the expected matches.
            if (match->start_pos == expectedPositions[i][0] && match->end_pos == expectedPositions[i][1])
            {
                expectedPositions[i][0] = -1, expectedPositions[i][1] = -1;
                foundMatch = 1;
                break;
            }
        }

        // if we didnt find any match the test has failed.
        if (!foundMatch)
        {
            dbllist_destroy(matchedPatterns, DBLLIST_FREE_DATA);
            pm_destroy(pm);

            free(pm);
            free(matchedPatterns);

            pm              = NULL;
            matchedPatterns = NULL;

            return TEST_HAS_FAILED;
        }

        // set again the found flag to be false.
        foundMatch = 0;

    }

    pm_destroy(pm);
    dbllist_destroy(matchedPatterns, DBLLIST_FREE_DATA);
    
    free(pm);
    free(matchedPatterns);

    pm              = NULL;
    matchedPatterns = NULL;

    return TEST_HAS_SUCCEEDED;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("[!] Usage: --<TEST_NAME>\n");
        return TEST_HAS_FAILED;
    }
        
    if (strcmp(argv[1], "--init") == 0)
        return InitTest();
    
    else if (strcmp(argv[1], "--insert") == 0)
        return InsertTest();
    
    else if (strcmp(argv[1], "--leave-destroy") == 0)
        return LeaveDestroyTest();
    
    else if (strcmp(argv[1], "--free-destroy") == 0)
        return FreeDestroyTest();
    
    else if (strcmp(argv[1], "--remove") == 0)
        return RemoveTest();
    
    else if (strcmp(argv[1], "--forward-transitions") == 0)
        return ForwardTransitionTest();
    
    else if (strcmp(argv[1], "--failure-transitions") == 0)
        return FailureTransitionTest();
    
    else if (strcmp(argv[1], "--pattern-scan") == 0)
        return PatternScanTest();
    
    else
        printf("[!] Info: invalid test name: %s\n", argv[1]);
    
    return TEST_HAS_FAILED;
}