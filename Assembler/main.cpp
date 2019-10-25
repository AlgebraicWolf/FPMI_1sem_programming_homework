#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "MyAsm.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

const unsigned short MAX_CMD_SIZE = 33;
const unsigned short MAX_SARG_SIZE = 33;

struct label_t {
    char *label;
    int pos;
};

int parseParams(int argc, char *argv[], char **filename);

int loadFile(FILE **f, const char *loadpath, const char *mode);

unsigned long fileSize(FILE *f);

int countLines(const char *txt, const char delimiter);

char *concatenate(const char *str1, const char *str2);

char *makeName(char *original, char *ext);

char *
generateMachineCode(FILE *source, int lines, int *fileSize, label_t *labels = NULL, int *err = NULL, int *errline = NULL);

const char *defaultFilename = "prog.asm";

int main(int argc, char *argv[]) {
    char *filename = nullptr;

    if (parseParams(argc, argv, &filename) == -1) return -1;

    printf(ANSI_COLOR_BLUE "Compiling file %s\n" ANSI_COLOR_RESET, filename);

    FILE *source = {};

    if (!loadFile(&source, filename, "r")) {
        printf(ANSI_COLOR_RED "Error while loading the file. Check the filename and permissions. Terminating...\n" ANSI_COLOR_RESET);
        return -1;
    }

    int size = fileSize(source);
    char *sourceCode = (char *) calloc(size + 1, sizeof(char));
    fread(sourceCode, sizeof(char), size, source);
    fseek(source, 0, SEEK_SET);
    //fclose(source);

    int lines = countLines(sourceCode, '\n');
    int fSize = 0;
    char *bin = generateMachineCode(source, lines, &fSize);
    char *outputName = makeName(filename, (char *) ".bin");


    FILE *output = fopen(outputName, "wb");
    fwrite(bin, 1, fSize, output);
    fclose(output);

    free(bin);
    free(sourceCode);
    free(filename);
    free(outputName);

    return 0;
}

char *makeName(char *original, char *ext) {
    char *dotPosition = strrchr(original, '.');
    char *newName = nullptr;
    if (dotPosition) {
        *dotPosition = '\0';
        newName = concatenate(original, ext);
        *dotPosition = '.';
    } else {
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

char *generateMachineCode(FILE *sourceFile, int lines, int *fileSize, label_t *labels, int *err, int *errline) {
    enum argumentTypes { \
        NONE, \
        NUMBER, \
        RAM_IMMED, \
        RAM_REG, \
        RAM_REG_IMMED, \
        LABEL \
    }; \
    auto machine_code = (char *) calloc(sizeof(char) + sizeof(int),
                                        lines); // TODO aprroximate worst-case scenario depending on the current maximal cmd size

    bool parseLabels = false;
    if(labels == nullptr) {
        parseLabels = true;
        labels = (label_t *) calloc (lines + 1, sizeof(label_t));
    }

    char *machine_code_start = machine_code;
    char cmd[MAX_CMD_SIZE] = "";
    char sarg[MAX_SARG_SIZE] = "";
    int arg = 0;
    int len = 0;

    for (int i = 0; i < lines; i++) {
        if (fscanf(sourceFile, "%s", cmd) == 0) break;

/*#define DEF_CMD(name, code, args, source) \
    if (strcmp(cmd, #name) == 0) { \
            *(machine_code++) = code; \
            len += sizeof(char); \
            for(int i = 0; i < args; i++) { \
                len += sizeof(int); \
                if (fscanf(sourceFile,"%d", &arg) != EOF) {\
                    *((int *)(machine_code)) = arg; \
                    machine_code = (char *)((int *)machine_code + 1); \
                } \
                else { \
                    printf(ANSI_COLOR_RED "INVALID NUMBER OF ARGUMENTS1" ANSI_COLOR_RESET); \
                } \
            } \
        } \*/


#define CMD_OVRLD(opcode, cond, argtype, execcode) \
        else if (cond) { \
            *(machine_code++) = opcode; \
            len += sizeof(char); \
            if (argtype == NUMBER) { \
                arg = atoi(sarg); \
                *((int *)(machine_code)) = arg; \
                machine_code = (char *)((int *)machine_code + 1); \
                len += sizeof(int); \
            } \
            else if (argtype == REGISTER) == 0) { \
                if(strcmp(sarg, "ax") == 0) { \
                    arg = 0; \
                } \
                else if(strcmp(sarg, "bx") == 0) { \
                    arg = 1; \
                } \
                else if(strcmp(sarg, "cx") == 0) { \
                    arg = 2; \
                } \
                else if(strcmp(sarg, "dx") == 0) { \
                    arg = 3; \
                } \
                else { \
                    printf(ANSI_COLOR_RED "Invalid argument provided. Terminating...\n" ANSI_COLOR_RESET); \
                    return nullptr; \
                } \
                *((int *)(machine_code)) = arg; \
                machine_code = (char *)((int *)machine_code + 1); \
                len += sizeof(int); \
            } \
            else if (argtype == RAM_REG) { \
                if (fscanf("\[%s\]", sarg) != EOF) { \
                    if (strcmp(sarg, "ax") == 0) {\
                        arg = 0; \
                    } \
                    else if (strcmp(sarg, "bx") == 0) { \
                        arg = 1; \
                    } \
                    else if (strcmp(sarg, "cx") == 0) { \
                        arg = 2; \
                    } \
                    else if (strcmp(sarg, "dx") == 0) { \
                        arg = 3; \
                    } \
                    else { \
                        printf(ANSI_COLOR_RED "Invalid register identifier. Terminating...\n" ANSI_COLOR_RESET); \
                        return nullptr; \
                    } \
                    *((int *)(machine_code)) = arg; \
                    machine_code = (char *)((int *)machine_code + 1); \
                    len += sizeof(int); \
                } \
                else { \
                    printf(ANSI_COLOR_RED "Invalid RAM address declaration. Terminating..." ANSI_COLOR_RESET); \
                } \
            } \
            else if (argtype == NONE) {} \
            else if (argtype == LABEL) == 0) { \
                if(!parseLabels) { \
                    label_t *curLabel = labels; \
                    bool found = false; \
                    while (curLabel->label != nullptr) { \
                        if(strcmp(curLabel->label, sarg) == 0) { \
                            *((int *)(machine_code)) = curLabel->pos; \
                            found = true; \
                            break; \
                        } \
                        curLabel++; \
                    } \
                    if(!found) {\
                        printf(ANSI_COLOR_RED "Label \"%s\" not found. Terminating...\n" ANSI_COLOR_RESET, sarg); \
                        return nullptr; \
                    }\
                } \
                else { \
                    *((int *)(machine_code)) = 0; \
                } \
                machine_code = (char *)((int *)machine_code + 1); \
                len += sizeof(int); \
            } \
        }


#define DEF_CMD(name, args, overloaders) \
    else if(strcmp(cmd, #name) == 0) {\
        if(args > 0) \
            if(!fscanf(sourceFile, "%s", sarg)) {\
            printf(ANSI_COLOR_RED "Invalid number of arguments!\n" ANSI_COLOR_RESET);\
            return nullptr;\
         }\
            if (false); \
        overloaders \
        else {\
            printf(ANSI_COLOR_RED "Invalid argument parameter %s for command %s in line %d. Terminating...\n" ANSI_COLOR_RESET, sarg, cmd, i + 1); \
            return nullptr; \
        } \
    }\

if(false);
#include "../commands.h"
    else {
        char *end = strchr(cmd, ':');
        if ((end != nullptr) && parseLabels) {
            for(char *i = end+1; (*i != '\n') && (*i != '\0'); i++)
                if(!isblank(*i)) {
                    printf(ANSI_COLOR_RED "Invalid label declaration! Unexpected symbol %d. Terminating...\n" ANSI_COLOR_RESET, *i);
                    return nullptr;
                }

            label_t *curLabel = labels;
            *end = '\0';
            auto name = (char *) calloc(end - cmd, sizeof(char));
            strcpy(name, cmd);
            *end = ':';
            label_t lbl = {};
            while(curLabel->label != nullptr) {
                if(strcmp(curLabel->label, name) == 0) {
                    printf(ANSI_COLOR_RED "Label already defined. Terminating...\n" ANSI_COLOR_RESET);
                    return nullptr;
                }
                curLabel++;
            }

            curLabel->pos = len;
            curLabel->label = name;
        }
    }
#undef DEF_CMD
#undef CMD_OVRLD

        fscanf(sourceFile, "\n");
        memset(sarg, 0, 33);
        memset(cmd, 0, 33);
    }
    *fileSize = sizeof(char) * len;
    machine_code = (char *) realloc(machine_code_start, *fileSize);
    rewind(sourceFile);
    if(!parseLabels) {
        return machine_code;
        free(labels);
    }
    else
        return generateMachineCode(sourceFile, lines, fileSize, labels);
}

int parseParams(int argc, char *argv[], char **filename) {
    if (argc == 1) {
        printf(ANSI_COLOR_YELLOW "Neither file nor compiler type specified. Using default parameters\n" ANSI_COLOR_RESET);
        *filename = (char *) calloc(strlen(defaultFilename) + 1, sizeof(char));
        strcpy(*filename, defaultFilename);
    } else if (argc == 2) {
        *filename = (char *) calloc(strlen(argv[1]) + 1, sizeof(char));
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