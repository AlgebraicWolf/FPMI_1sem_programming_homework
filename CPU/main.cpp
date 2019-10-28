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

#define ANSI_BGCOLOR_BLACK "\x1b[40m"
#define ANSI_BGCOLOR_RED "\x1b[41m"
#define ANSI_BGCOLOR_GREEN "\x1b[42m"
#define ANSI_BGCOLOR_YELLOW "\x1b[43m"
#define ANSI_BGCOLOR_BLUE "\x1b[44m"
#define ANSI_BGCOLOR_MAGENTA "\x1b[45m"
#define ANSI_BGCOLOR_CYAN "\x1b[46m"
#define ANSI_BGCOLOR_WHITE "\x1b[47m"
#define ANSI_BGCOLOR_RESET "\x1b[49m"

const size_t RAM_SIZE = 1024;

const size_t WIDTH = 64;

const size_t HEIGHT = 64;

const char *defaultFilename = "prog.bin";

int get_int(stack_t *stk);

void setIntToRAM(int *RAM, size_t n, int val);

int pop(stack_t *stk);

void push(stack_t *stk, int value);

void drawScreen(char *VRAM);

int setPixel(char *VRAM, unsigned int desc);

int parseParams(int argc, char *argv[], char **filename);

int peak_n(stack_t *stk, int n);

int getIntFromRAM(int *RAM, size_t n);

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
    assert(stk);
    int value = 0;
    while(scanf("%d", &value) == EOF);
    return value;
}

int pop(stack_t *stk) {
    assert(stk);
    int value = 0;
    if (!stackPop(stk, &value)) {
        printf(ANSI_COLOR_RED "Stack underflow error! Terminating...\n" ANSI_COLOR_RESET);
        exit(-1);
    }
    return value;
}

int peak_n(stack_t *stk, int n) {
    assert(stk);
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
    assert(stk);
    if (!stackPush(stk, value)) {
        printf(ANSI_COLOR_RED "Stack overflow error! Terminating...\n" ANSI_COLOR_RESET);
        exit(-1);
    }
}

int getIntFromRAM(int *RAM, size_t n) {
    assert(RAM);
    if (n >= RAM_SIZE) {
        printf(ANSI_COLOR_RED "Accessing non-existing RAM adress! Terminating...\n" ANSI_COLOR_RESET);
        exit(-1);
    }
    sleep(1);
    return RAM[n];
}

void drawScreen(char *VRAM) {
    assert(VRAM);
    usleep(25000);
    printf("\033[2J\033[1;1H");
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            switch(VRAM[y * WIDTH + x]) {
                case 0:
                    printf(ANSI_BGCOLOR_BLACK "  " ANSI_BGCOLOR_RESET);
                    break;
                case 1:
                    printf(ANSI_BGCOLOR_RED "  " ANSI_BGCOLOR_RESET);
                    break;
                case 2:
                    printf(ANSI_BGCOLOR_GREEN "  " ANSI_BGCOLOR_RESET);
                    break;
                case 3:
                    printf(ANSI_BGCOLOR_YELLOW "  " ANSI_BGCOLOR_RESET);
                    break;
                case 4:
                    printf(ANSI_BGCOLOR_BLUE "  " ANSI_BGCOLOR_RESET);
                    break;
                case 5:
                    printf(ANSI_BGCOLOR_MAGENTA "  " ANSI_BGCOLOR_RESET);
                    break;
                case 6:
                    printf(ANSI_BGCOprintfLOR_CYAN "  " ANSI_BGCOLOR_RESET);
                    break;
                case 7:
                    printf(ANSI_BGCOLOR_WHITE "  " ANSI_BGCOLOR_RESET);
                    break;
            }
        }
        printf("\n");
    }
}

int setPixel(char *VRAM, unsigned int desc) {
    assert(VRAM);
    /*
     * Pixel format:
     * 0b11111111 11110000 00000000 00000000 - x coord
     * 0b00000000 00001111 11111111 00000000 - y coord
     * 0b00000000 00000000 00000000 11111111 - color
     */
    /*unsigned int x = desc >> (unsigned int)20;
    unsigned int y = (desc & (unsigned int)1048320) >> (unsigned int)8;
    unsigned char color = desc & (unsigned int)255;*/
    unsigned int x = desc / 10000;
    unsigned int y = desc / 10 % 1000;
    if((x >= WIDTH) || (y >= HEIGHT)) {
        printf(ANSI_COLOR_RED "Invalid coordinates x:%d y:%d. Terminating...\n" ANSI_COLOR_RESET, x, y);
        exit(-1);
    }
    unsigned int color = desc % 10;
    VRAM[y * WIDTH + x] = (char) color;
}

void setIntToRAM(int *RAM, size_t n, int val) {
    if (n >= RAM_SIZE) {
        printf(ANSI_COLOR_RED "Accessing non-existing RAM adress! Terminating...\n" ANSI_COLOR_RESET);
        exit(-1);
    }
    sleep(1);
    RAM[n] = val;
}


int execute(char *bin, int len) {
    int precision = 10;
    stack_t stk = {};
    stackConstruct(&stk, "CPUStack", 1024, 4417);
    auto RAM = (int *) calloc(RAM_SIZE, sizeof(int));
    auto VRAM = (char *) calloc(WIDTH * HEIGHT, sizeof(char));
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
    free(VRAM);
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