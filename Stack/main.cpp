#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"



#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define DEBUG

#ifdef DEBUG
#define UTEST(cond, flag) {\
    if(!(cond)) {\
        flag = false;\
        printf(ANSI_COLOR_RED "Unit testing failed: %s\n" ANSI_COLOR_RESET, #cond); \
    }\
}

#else
#define UTEST(cond) ;
#endif

bool doUnitTesting();

int main() {

    if (!doUnitTesting()) {
        printf(ANSI_COLOR_RED "\n====================\nUNIT TESTING FAILED\n====================\n\n" ANSI_COLOR_RESET);
    } else {
        printf(ANSI_COLOR_GREEN "\n====================\nUNIT TESTING DONE\n====================\n\n" ANSI_COLOR_RESET);
    }
    stack_t stk = {};
    CONSTRUCT_STACK(stk);
    for (int i = 0; i <= 47; i++) {
        stackPush(&stk, i);
    }

    unsigned long a = getStructHash(&stk);

    FILE *dump = fopen("dumpTest.txt", "w");
    stackDump(dump, &stk, (const char *) "Testing stackDump");
    stackPush(&stk, 47);
}

/**
 * Proof-Of-Concept stack crasher that substitudes pointer to the data array
 * @param stk S    stk->data = (elem_t *)42;
tack to crush
 */
int dataPointerChangeTest(stack_t *stk) {
    stk->data = (elem_t *) 42;
    if (!checkStackValidity(stk))
        return 1;
    return 0;
}

int compromiseHash() {
    stack_t comprHashStack = {};
    CONSTRUCT_STACK(comprHashStack);
    stackPush(&comprHashStack, 1);
    stackPush(&comprHashStack, 2);
    stackPush(&comprHashStack, 3);
    FILE *f = fopen("hashCompromise.txt", "w");
    stackDump(f, &comprHashStack, "Hash before attack");

    int offset = 0;
#ifdef USE_CANARIES
    offset = CANARY_STACK_SIZE;
#endif
    comprHashStack.data[0+offset] = 42;
    comprHashStack.data[1+offset] = 42;
    comprHashStack.data[2+offset] = 42;
    updateHashes(&comprHashStack);
    stackDump(f, &comprHashStack, "Hash after attack");
    return checkStackValidity(&comprHashStack);
}

void stackZeroDump() {
    stack_t stack = {};
    FILE *f = fopen("emptyDump.txt", "w");
    stackDump(f, &stack, "TEST");
}

/**
 * Function that performs unit-testing
 * @return true if all tests are successful, false otherwise
 */

bool doUnitTesting() {
    stack_t testStk = {};
    CONSTRUCT_STACK(testStk);

    elem_t el = {};

    bool testResult = true;

    // Stack should be empty at first
    UTEST(testStk.size == 0, testResult);

    // Check adding integrity
    const int numAddedElems = 10;
    size_t oldSize = testStk.size;
    for (int i = 0; i < numAddedElems; i++)
        stackPush(&testStk, el);
    UTEST((testStk.size - oldSize) == numAddedElems, testResult);

    // Check whether elements are saved and loaded properly
    const int numSavedAndLoadedElems = 8;
    for (int i = 0; i < numSavedAndLoadedElems; i++) {
        if (!stackPush(&testStk, (elem_t) i)) break;
    }

    for (int i = numSavedAndLoadedElems - 1; i <= 0; i--) {
        stackPop(&testStk, &el);
        UTEST(el == i, testResult);
        if (el != i) break;
    }

    // Check whether stack maxsize is increased correctly
    oldSize = testStk.maxsize;
    stackExtend(&testStk);
    UTEST(SIZE_MULTIPLIER * oldSize == testStk.maxsize, testResult);
//int dataPointerChangeTest(stack_t *stk) {
//    stk->data = (elem_t *) 42;
//    if (!checkStackValidity(stk))
//        return 1;
//    return 0;
//}
//    UTEST(dataPointerChangeTest(&testStk), testResult);

//#ifdef USE_HASH
//    UTEST(compromiseHash(), testResult);
//#endif

    stackDestruct(&testStk);
    UTEST(testStk.size == 0, testResult);
    UTEST(testStk.maxsize == 0, testResult);
    UTEST(testStk.data == nullptr, testResult);


    return testResult;
}