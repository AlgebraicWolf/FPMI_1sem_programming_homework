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

int parseRegister(const char *reg);

char *
generateMachineCode(FILE *source, int lines, int *fileSize, label_t *labels = NULL);

const char *defaultFilename = "prog.asm";

enum argumentTypes {
    NONE,
    NUMBER,
    REGISTER,
    RAM_IMMED,
    RAM_REG,
    RAM_REG_IMMED,
    LABEL
};

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
    assert(original);
    assert(ext);
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

int parseRegister(const char *reg) {
    assert(reg);
    if (strcmp(reg, "ax") == 0) {
        return 0;
    } else if (strcmp(reg, "bx") == 0) {
        return 1;
    } else if (strcmp(reg, "cx") == 0) {
        return 2;
    } else if (strcmp(reg, "dx") == 0) {
        return 3;
    } else {
        return -1;
    }
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

int processNumberArgument(char **machineCode, const char *sarg) {
    assert(machineCode);
    assert(*machineCode);
    assert(sarg);
    int arg = atoi(sarg);
    *((int *) (*machineCode)) = arg;
    *machineCode = (char *) ((int *) (*machineCode) + 1);
    return 0;
}

int processRegisterArgument(char **machineCode, const char *sarg) {
    assert(machineCode);
    assert(*machineCode);
    assert(sarg);
    int arg = parseRegister(sarg);
    if (arg == -1) {
        printf(ANSI_COLOR_RED "Invalid register name provided. Terminating...\n" ANSI_COLOR_RESET);
        return -1;
    }
    *((int *) (*machineCode)) = arg;
    *machineCode = (char *) ((int *) (*machineCode) + 1);
    return 0;
}

int processMixedRAM(char **machineCode, const char *sarg) {
    assert(machineCode);
    assert(*machineCode);
    assert(sarg);
    int arg = 0;
    char *ram_sarg = nullptr;
    if (strchr(sarg, '+')) {
        if (sscanf(sarg, "[%[a-z]+%d]", ram_sarg, &arg) == EOF) {
            printf(ANSI_COLOR_RED "Invalid RAM address declaration. Terminating..." ANSI_COLOR_RESET);
            return -1;
        }
    } else if (sarg, '-') {
        if (sscanf(sarg, "[%[a-z]-%d]", ram_sarg, &arg) == EOF) {
            printf(ANSI_COLOR_RED "Invalid RAM address declaration. Terminating..." ANSI_COLOR_RESET);
            return -1;
        }
        arg *= -1;
    } else {
        printf(ANSI_COLOR_RED"Invalid register + immediate RAM value. Terminating...\n" ANSI_COLOR_RESET);
        return -1;
    }
    processRegisterArgument(machineCode, ram_sarg);
    *((int *) (*machineCode)) = arg;
    *machineCode = (char *) ((int *) machineCode + 1);
}


int processLabel(char **machineCode, const label_t *labels, const char *sarg) {
    assert(machineCode);
    assert(*machineCode);
    assert(labels);
    assert(sarg);
    bool found = false;
    while (labels->label != nullptr) {
        if (strcmp(labels->label, sarg) == 0) {
            *((int *) (*machineCode)) = labels->pos;
            return 0;
        }
        labels++;
    }
    printf(ANSI_COLOR_RED "Label \"%s\" not found. Terminating...\n" ANSI_COLOR_RESET, sarg);
    return -1;
}

char *
defineCommandOverload(char **machine_code, int *len, int opcode, argumentTypes argtype, bool parseLabels, char *sarg,
                      label_t *labels) {
    char ram_sarg[MAX_SARG_SIZE] = "";
    int arg = 0;
    int arg2 = 0;
    **machine_code = opcode;
    (*machine_code)++;
    *len += sizeof(char);
    if (argtype == NUMBER) {
        processNumberArgument(machine_code, sarg);
        *len += sizeof(int);
    } else if (argtype == REGISTER) {
        if (processRegisterArgument(machine_code, sarg) == -1)
            return nullptr;
        *len += sizeof(int);
    } else if (argtype == RAM_REG) {
        if (sscanf(sarg, "[%[a-z]]", ram_sarg) != EOF) {
            if (processRegisterArgument(machine_code, sarg) == -1)
                return nullptr;
            *len += sizeof(int);
        } else {
            printf(ANSI_COLOR_RED "Invalid RAM address declaration. Terminating..." ANSI_COLOR_RESET);
            return nullptr;
        }
    } else if (argtype == RAM_IMMED) {
        if (sscanf(sarg, "[%d]", &arg) != EOF) {
            processNumberArgument(machine_code, sarg);
        } else {
            printf(ANSI_COLOR_RED "Invalid RAM address declaration. Terminating..." ANSI_COLOR_RESET);
            return nullptr;
        }
    } else if (argtype == NONE) {}
    else if (argtype == RAM_REG_IMMED) {
        if (processMixedRAM(machine_code, sarg) == -1) return nullptr;
        *len += 2 * sizeof(int);
    } else if (argtype == LABEL) {
        if (!parseLabels) {
            if (processLabel(machine_code, labels, sarg) == -1)
                return nullptr;
        } else {
            *((int *) (*machine_code)) = 0;
        }
        (*machine_code) = (char *) ((int *) (*machine_code) + 1);
        *len += sizeof(int);
    }
    return (char *)1;
}

char *generateMachineCode(FILE *sourceFile, int lines, int *fileSize, label_t *labels) {
    assert(sourceFile);
    assert(fileSize);

    auto machine_code = (char *) calloc(sizeof(char) + 2 * sizeof(int),
                                        lines); // TODO aprroximate worst-case scenario depending on the current maximal cmd size

    bool parseLabels = false;
    if (labels == nullptr) {
        parseLabels = true;
        labels = (label_t *) calloc(lines + 1, sizeof(label_t));
    }

    char *machine_code_start = machine_code;
    char cmd[MAX_CMD_SIZE] = "";
    char sarg[MAX_SARG_SIZE] = "";
    int len = 0;

    for (int i = 0; i < lines; i++) {
        if (fscanf(sourceFile, "%s", cmd) == 0) break;

#define CMD_OVRLD(opcode, cond, argtype, execcode) \
        else if (cond) defineCommandOverload(&machine_code, &len, opcode, argtype, parseLabels, sarg, labels);


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

        if (false);

#include "../commands.h"

        else {
            char *end = strchr(cmd, ':');
            if ((end != nullptr) && parseLabels) {
                for (char *i = end + 1; (*i != '\n') && (*i != '\0'); i++)
                    if (!isblank(*i)) {
                        printf(ANSI_COLOR_RED "Invalid label declaration! Unexpected symbol %d. Terminating...\n" ANSI_COLOR_RESET,
                               *i);
                        return nullptr;
                    }

                label_t *curLabel = labels;
                *end = '\0';
                auto name = (char *) calloc(end - cmd, sizeof(char));
                strcpy(name, cmd);
                *end = ':';
                label_t lbl = {};
                while (curLabel->label != nullptr) {
                    if (strcmp(curLabel->label, name) == 0) {
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
    if (!parseLabels) {
        return machine_code;
        free(labels);
    } else
        return generateMachineCode(sourceFile, lines, fileSize, labels);
}

int parseParams(int argc, char *argv[], char **filename) {
    assert(filename);
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