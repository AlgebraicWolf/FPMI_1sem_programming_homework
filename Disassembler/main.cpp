#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0"

int parseParams(int argc, char *argv[], char **filename);

int loadFile(FILE **f, const char *loadpath, const char *mode);

unsigned long fileSize(FILE *f);

const char *defaultFilename = "prog.bin";

int disassemble(FILE *output, char *bin, int len);

char *concatenate(const char *str1, const char *str2);

char *makeName(char *original, char *ext);

int main(int argc, char *argv[]) {
    char *filename = {};
    parseParams(argc, argv, &filename);
    FILE *bin = {};
    FILE *disassembled = {};

    if (!loadFile(&bin, filename, "rb")) {
        printf(ANSI_COLOR_RED "Error while loading the %s file. Terminating...\n" ANSI_COLOR_RESET, filename);

    }
    if (!loadFile(&disassembled, makeName(filename, (char *) ".asm"), "w")) {
        printf(ANSI_COLOR_RED "Error while loading the %s file. Terminating...\n" ANSI_COLOR_RESET, filename);
    }

    int size = fileSize(bin);
    char *machineCode = (char *) calloc(size + 1, sizeof(char));
    fread(machineCode, sizeof(char), size, bin);

    disassemble(disassembled, machineCode, size);
    fclose(disassembled);
    fclose(bin);
}

char *makeName(char *original, char *ext) {
    char *dotPosition = strrchr(original, '.');
    char *newName = nullptr;
    if (dotPosition) {
        *dotPosition = '\0';
        newName = concatenate(original, ext);
        *dotPosition = '.';
    }
    else {
        newName = concatenate(original, ext);
    }
    return newName;
}

char *concatenate(const char *str1, const char *str2) {
    assert(str1);
    assert(str2);
    const char *end1 = strchr(str1, '\0');
    const char *end2 = strchr(str2, '\0');
    if ((end1 == nullptr) || (end2 == nullptr)) return nullptr;
    int len = (end1 - str1) + (end2 - str2) + 1;
    auto concatenated = (char *) calloc(len, sizeof(char));
    strcpy(concatenated, str1);
    strcat(concatenated, str2);
    return concatenated;
}

int disassemble(FILE *output, char *bin, int len) {
    int cursor = 0;
    while (cursor < len) {

#define DEF_CMD(name, code, args, source) \
    else if(bin[cursor] == code) { \
        fprintf(output, "%s", #name); \
        cursor += sizeof(char); \
        for(int i = 0; i < args; i++) { \
            if((len - cursor) <= 3) return -1;\
            fprintf(output, " %d", *((int *)(bin + cursor))); \
            cursor += sizeof(int); \
        }\
        fprintf(output, "\n"); \
    }\

        if (0 == 1) continue;
#include "../commands.h"
#undef DEF_CMD
    }
    return 1;
}

int parseParams(int argc, char *argv[], char **filename) {
    if (argc == 1) {
        printf(ANSI_COLOR_YELLOW "File have not been specified. Using default %s\n" ANSI_COLOR_RESET, defaultFilename);
        *filename = (char *) calloc(strlen(defaultFilename) + 1, sizeof(char));
        strcpy(*filename, defaultFilename);
        return 1;
    } else if (argc == 2) {
        *filename = (char *) calloc(strlen(argv[1]) + 1, sizeof(char));
        strcpy(*filename, argv[1]);
        return 1;
    } else {
        printf(ANSI_COLOR_RED "Invalid number of arguments. Terminating...\n" ANSI_COLOR_RESET);
        return -1;
    }
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
