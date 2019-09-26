#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

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


typedef int elem_t;

const size_t DEFAULT_INIT_SIZE = 16;

const int SIZE_MULTIPLIER = 2;

const unsigned char DEFAULT_POISON = 0;

struct stack_t {
    elem_t *data;
    unsigned char poisonValue;
    size_t size;
    size_t maxsize;
};

int stackConstruct(stack_t *stack, size_t size = DEFAULT_INIT_SIZE, elem_t poison = DEFAULT_POISON);

int stackPush(stack_t *stack, elem_t element);

int stackPop(stack_t *stack, elem_t *destination);

int stackExtend(stack_t *stack);

int stackOk(stack_t stack);

int stackDestruct(stack_t *stack);

int stackDump(FILE *f, stack_t *stack, char *stack_name, char *prompt = (char *) "", bool colored = true);

bool doUnitTesting();

int main() {
    if (!doUnitTesting()) {
        printf(ANSI_COLOR_RED "\n====================\nUNIT TESTING FAILED\n====================\n\n" ANSI_COLOR_RESET);
    } else {
        printf(ANSI_COLOR_GREEN "\n====================\nUNIT TESTING DONE\n====================\n\n" ANSI_COLOR_RESET);
    }
    stack_t stk = {};
    stackConstruct(&stk);
    for (int i = 0; i <= 47; i++) {
        stackPush(&stk, i);
    }
    FILE *dump = fopen("dumpTest.txt", "w");
    stackDump(dump, &stk, (char *) "stk", (char *) "Testing stackDump");
}

/**
 * Fucntion that performs unit-testing
 * @return true if all tests are successful, false otherwise
 */

bool doUnitTesting() {
    stack_t testStk = {};
    stackConstruct(&testStk);

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
    const int numSavedAndLoadedElems = 256;
    for (int i = 0; i < numSavedAndLoadedElems; i++) {
        stackPush(&testStk, (elem_t) i);
    }

    for (int i = numSavedAndLoadedElems - 1; i <= 0; i--) {
        stackPop(&testStk, &el);
        UTEST(el == i, testResult);
    }

    // Check whether stack maxsize is increased correctly
    oldSize = testStk.maxsize;
    stackExtend(&testStk);
    UTEST(SIZE_MULTIPLIER * oldSize == testStk.maxsize, testResult);

    stackDestruct(&testStk);
    UTEST(testStk.size == 0, testResult);
    UTEST(testStk.maxsize == 0, testResult);
    UTEST(testStk.data == nullptr, testResult);

    return testResult;
}

/**
 * Stack constructor that initializes structure
 * @param stack Pointer to stack_t structure
 * @param size Desired size of stack
 * @param poisonValue Desired poison value
 * @return 0 if allocation error happened, 0 otherwise
 */

int stackConstruct(stack_t *stack, size_t size, elem_t poisonValue) {
    assert(stack);
    assert(size > 0);

    stack->maxsize = size;
    stack->data = (elem_t *) calloc(size, sizeof(elem_t));
    stack->size = 0;
    stack->poisonValue = poisonValue;


    if (stack->data == nullptr) return 0;
    return 1;
}

/**
 * Function that pushes element to stack
 * @param stack Pointer to stack
 * @param element Element to push
 * @return 1 in case of success, 0 if allocation error happened and stack cannot be expanded
 */

int stackPush(stack_t *stack, elem_t element) {
    assert(stack);

    if ((stack->size) >= (stack->maxsize)) {
        if (!stackExtend(stack)) return 0;
    }

    stack->data[stack->size++] = element;

    return 1;
}

/**
 * Function that pops the element from stack
 * @param stack Pointer to stack
 * @param destination Where to put stack element
 * @return 1 if successful, 0 if no elements have left
 */

int stackPop(stack_t *stack, elem_t *destination) {
    assert(stack);
    assert(destination);

    if (stack->size == 0) return 0;
    *destination = stack->data[--stack->size];
    stack->data[stack->size] = stack->poisonValue;

    return 1;
}

/**
 * Function that extends stack
 * @param stack Pointer to stack
 * @return 0 if allocation error happened, 1 otherwise
 */

int stackExtend(stack_t *stack) {
    assert(stack);
    
    extern const int SIZE_MULTIPLIER;

    size_t newSize = stack->maxsize * SIZE_MULTIPLIER;
    elem_t *newPointer = (elem_t *) realloc(stack->data, newSize * sizeof(elem_t));

    if (!newPointer) return 0;
    else {
        stack->data = newPointer;
        memset(stack->data + stack->maxsize, 0, (newSize - stack->maxsize) * sizeof(elem_t));
        stack->maxsize = newSize;

        return 1;
    }
}

/**
 * Checks whether given stack is valid or not
 * @param stack Stack to check
 * @return 1 if stack is OK, 0 if data pointer is 0, -1 if size>maxsize, -2 if maxsize = 0
 */

int stackOk(stack_t stack) {
    if (stack.data == nullptr) return 0;
    if (stack.size > stack.maxsize) return -1;
    if (stack.maxsize == 0) return -2;
    return 1;
}

/**
 * Stack destructor
 * @param stack Pointer to stack
 * @return 1 if Ok, 0 otherwise
 */

int stackDestruct(stack_t *stack) {
    assert(stack);
    assert(stack->data);

    stack->size = 0;
    stack->maxsize = 0;
    free(stack->data);
    stack->data = nullptr;
    return 1;
}


/**
 * Saves stack dump to provided file
 * @param f Pointer to file
 * @param stack Pointer to stack
 * @param stack_name Name of stack for printing
 * @param prompt
 * @param colored
 * @return
 */
int stackDump(FILE *f, stack_t *stack, char *stack_name, char *prompt, bool colored) {
    assert(f);
    assert(stack);
    assert(stack_name);
    assert(prompt);

    fprintf(f, "%s\n", prompt);
    fprintf(f, "stack_t %s [%p] {\n", stack_name, stack);
    fprintf(f, "    size = %llu;\n    poison = %d;\n    data[%llu] = [%p]; {\n", stack->size, stack->poisonValue,
            stack->maxsize, stack->data);
    char customCh = '*';
    for (size_t i = 0; i < stack->maxsize; i++) {
        if (i < stack->size) {
            if (colored)
                if (stack->data[i] == stack->poisonValue) fprintf(f, ANSI_COLOR_RED);
                else fprintf(f, ANSI_COLOR_GREEN);
            customCh = '*';
        } else {
            if (colored) fprintf(f, ANSI_COLOR_YELLOW);
            customCh = ' ';
        }
        fprintf(f, "      %c [%llu] = %d", customCh, i, stack->data[i]);
        if (stack->data[i] == stack->poisonValue)
            fprintf(f, " [POISON]\n" ANSI_COLOR_RESET);
        else
            fprintf(f, "\n" ANSI_COLOR_RESET);
    }
    fprintf(f, "    }\n}\n");
    return 1;
}