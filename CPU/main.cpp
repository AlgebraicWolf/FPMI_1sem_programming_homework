
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "stack.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

const size_t RAM_SIZE = 1024;

const char *defaultFilename = "prog.bin";

int get_int(stack_t *stk);

void setIntToRAM(char *RAM, size_t n, int val);

int pop(stack_t *stk);

void push(stack_t *stk, int value);

int parseParams(int argc, char *argv[], char **filename);

int peak_n(stack_t *stk, int n);

int getIntFromRAM(char *RAM, size_t n);

int loadFile(FILE **f, const char *loadpath, const char *mode);

unsigned long fileSize(FILE *f);

int countLines(const char *txt, const char delimiter);

int execute(char *bin, int len);

int main(int argc, char *argv[]) {
    char *filename = nullptr;

    if (parseParams(argc, argv, &filename) == -1) return -1;

    printf(ANSI_COLOR_BLUE "Executing file %s\n" ANSI_COLOR_RESET, filename);

    FILE *source = {};

    if (!loadFile(&source, filename, "r")) {
        printf(ANSI_COLOR_RED "Error while loading the file. Check the filename and permissions. Terminating...\n" ANSI_COLOR_RESET);
        return -1;
    }

    int size = fileSize(source);
    char *sourceCode = (char *) calloc(size + 1, sizeof(char));
    fread(sourceCode, sizeof(char), size, source);
    fseek(source, 0, SEEK_SET);
    fclose(source);
    execute(sourceCode, size);

    free(sourceCode);
    free(filename);
}

int get_int(stack_t *stk) {
    int value = 0;
    while(scanf("%d", &value) == EOF);
    return value;
}

int pop(stack_t *stk) {
    int value = 0;
    if (!stackPop(stk, &value)) {
        printf(ANSI_COLOR_RED "Stack underflow error! Terminating...\n" ANSI_COLOR_RESET);
        exit(-1);
    }
    return value;
}

int peak_n(stack_t *stk, int n) {
    auto values = (int *) calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        values[i] = pop(stk);
    }
    int value = values[n - 1];
    for(int i = n - 1; i >= 0; i--) {
        push(stk, values[i]);
    }
    free(values);
    return value;
}

void push(stack_t *stk, int value) {
    if (!stackPush(stk, value)) {
        printf(ANSI_COLOR_RED "Stack overflow error! Terminating...\n" ANSI_COLOR_RESET);
        exit(-1);
    }
}

int getIntFromRAM(char *RAM, size_t n) {
    if (n > RAM_SIZE - 4) {
        printf(ANSI_COLOR_RED "Accessing non-existing RAM adress! Terminating...\n" ANSI_COLOR_RESET);
        exit(-1);
    }
    sleep(1);
    return *(int *) (RAM + n);
}

void setIntToRAM(char *RAM, size_t n, int val) {
    if (n > RAM_SIZE - 4) {
        printf(ANSI_COLOR_RED "Accessing non-existing RAM adress! Terminating...\n" ANSI_COLOR_RESET);
        exit(-1);
    }
    sleep(1);
    *(int *) (RAM) = val;
}


int execute(char *bin, int len) {
    int precision = 100;
    stack_t stk = {};
    stackConstruct(&stk, "CPUStack", 1024, 4417);
    auto RAM = (char *) calloc(RAM_SIZE, sizeof(char));
    int registers[4] = {};
    char *binStart = bin;
    char cmd = 0;
    int arg = 0;
    while((bin - binStart) < len) {

#define DEF_CMD(name, args, overloaders) \
        overloaders \

#define CMD_OVRLD(opcode, cond, argtype, execcode) \
    else if(*bin == opcode) { \
        execcode \
    } \

        if (0 == 1) {}
#include "../commands.h"

        else {
            printf(ANSI_COLOR_RED "Unknown instruction. Terminating...\n" ANSI_COLOR_RESET);
            return 0;
        }

#undef DEF_CMD
#undef CMD_OVRLD

        bin++;
    }
    stackDestruct(&stk);
    free(RAM);
    return 1;
}

int parseParams(int argc, char *argv[], char **filename) {
    if (argc == 1) {
        printf(ANSI_COLOR_YELLOW "Neither file nor compiler type specified. Using default parameters\n" ANSI_COLOR_RESET);
        *filename = (char *)calloc(strlen(defaultFilename) + 1, sizeof(char));
        strcpy(*filename, defaultFilename);
    } else if (argc == 2) {
        *filename = (char *)calloc(strlen(argv[1]) + 1, sizeof(char));
        strcpy(*filename, argv[1]);
    } else {
        printf(ANSI_COLOR_RED "Invalid number of arguments. Terminating...\n" ANSI_COLOR_RESET);
        return -1;
    }

    return 0;
}

int loadFile(FILE **f, const char *loadpath, const char *mode) {
    assert(f);
    assert(loadpath);
    assert(*loadpath);
    assert(mode);
    assert(*mode);

    *f = fopen(loadpath, mode);
    return *f != nullptr;
}

unsigned long fileSize(FILE *f) {
    assert(f);

    fseek(f, 0, SEEK_END);
    unsigned long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    return size;
}

int countLines(const char *txt, const char delimiter) {
    assert(txt);
    const char *pointer = NULL;
    int lines = 0;
    while ((pointer = strchr(txt, delimiter)) != nullptr) {
        ++lines;
        txt = pointer + 1;
    }
    ++lines;

    return lines;
}