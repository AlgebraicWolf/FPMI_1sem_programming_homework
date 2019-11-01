#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>


#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0"

struct label_t {
    char *label;
    int pos;
};

enum argumentTypes {
    NONE,
    NUMBER,
    REGISTER,
    RAM_IMMED,
    RAM_REG,
    RAM_REG_IMMED,
    LABEL
};

int parseParams(int argc, char *argv[], char **filename);

int loadFile(FILE **f, const char *loadpath, const char *mode);

size_t fileSize(FILE *f);

const char *defaultFilename = "prog.bin";

const unsigned int defaultFilenameLength = 9;

int disassemble(FILE *output, char *bin, size_t len, label_t *labels);

label_t *parseLabels(char *bin, size_t len);

char *concatenate(const char *str1, const char *str2);

char *makeName(char *original, char *ext);

const unsigned int MAX_INT_LENGTH = 11;

const unsigned int MAX_NAME_LENGTH = 33;

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

    size_t size = fileSize(bin);
    char *machineCode = (char *) calloc(size + 1, sizeof(char));
    fread(machineCode, sizeof(char), size, bin);

    label_t *labels = parseLabels(machineCode, size);

    disassemble(disassembled, machineCode, size, labels);
    free(labels);
    fclose(disassembled);
    fclose(bin);
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

label_t *findLabel(label_t *labels, int pos) {
    assert(labels);

    while(labels->label) {
        if (labels->pos == pos)
            break;
        labels++;
    }

    return labels;
}

label_t *defineLabelParserOverload(int argNum, int len,  argumentTypes argtype, int *cursor, int *labelNum, char *bin, label_t *labels) {
    *cursor += sizeof(char);
    int pos = 0;
    label_t *curLabel = nullptr;
        if (argNum) {
            if((len - *cursor) <= sizeof(int) - 1) {
                printf(ANSI_COLOR_RED "Unexpected EOF. Terminating...\n" ANSI_COLOR_RESET);
                return nullptr;
            }
            switch(argtype) {
                case LABEL:
                pos = *((int *)(bin + *cursor));
                curLabel = findLabel(labels, pos);
                if(!curLabel->label) {
                    if(pos > len) {
                        printf(ANSI_COLOR_RED "Found label linking outside the program. Terminating...\n" ANSI_COLOR_RESET);
                        return nullptr;
                    }
                    curLabel->pos = pos;
                    curLabel->label = (char *) calloc(MAX_INT_LENGTH + 4, sizeof(char));
                    sprintf(curLabel->label, "meow%d", *labelNum++);
                }
                *cursor += sizeof(int);
                break;
                case RAM_REG_IMMED:
                    *cursor += 2 * sizeof(int);
                    break;
                case NONE:
                break;
                default:
                    *cursor += sizeof(int);
                break;
            }
        }
}

label_t *parseLabels(char *bin, size_t len) {
    char number[MAX_INT_LENGTH] = "";
    int argNum = 0;
    char cmdName[MAX_NAME_LENGTH];
    auto labels = (label_t *) calloc(len + 1, sizeof(label_t));
    int labelNum = 0;
    int cursor = 0;
    while (cursor < len) {

#define DEF_CMD(name, args, overloaders) \
        argNum = args; \
        strcpy(cmdName, #name); \
        overloaders {};


#define CMD_OVRLD(opcode, cond, argtype, execcode) \
    if(bin[cursor] == opcode) { \
        defineLabelParserOverload(argNum, len, argtype, &cursor, &labelNum, bin, labels); \
    } \
    else

#include "../commands.h"

#undef DEF_CMD
#undef CMD_OVRLD
    }
    return labels;
}


const char *parseRegister(int reg) {
    const char *regs[4] = {"ax", "bx", "cx", "dx"};
    if ((reg < 4) && (reg >= 0)) {
        return regs[reg];
    }
    else {
        return nullptr;
    }
}

int defineCommandOverload(char opcode, int argNum, int len, argumentTypes argtype, int *cursor, char *bin, char *cmdName, label_t *labels, FILE *output) {
    fprintf(output, "%s", cmdName);
        *cursor += sizeof(char);
        const char *reg = nullptr;
        const char *regarg = nullptr;
        label_t *curLabel = nullptr;
        if (argNum > 0) {
            if((len - *cursor) <= 3) {
                printf(ANSI_COLOR_RED "Unexpected EOF. Terminating...\n" ANSI_COLOR_RESET);
                return -1;
            }
            switch(argtype) {
                case LABEL:
                curLabel = findLabel(labels, *((int *)(bin + *cursor)));
                if(curLabel->label == nullptr) {
                    printf(ANSI_COLOR_RED"Label not found! Terminating...\n" ANSI_COLOR_RESET);
                    return -1;
                }
                fprintf(output, " %s", curLabel->label);
                *cursor += sizeof(int);
            break;
                case NUMBER:
                fprintf(output, " %d", *((int *)(bin + *cursor)));
                *cursor += sizeof(int);
            break;
                case REGISTER:
                reg = parseRegister(*((int *)(bin + *cursor)));
                if (reg == nullptr) {
                    printf(ANSI_COLOR_RED "Unknown register number %d. Terminating...\n" ANSI_COLOR_RESET, *((int *)(bin + *cursor)));
                }
                fprintf(output, " %s", reg);
                *cursor += sizeof(int);
            break;
                case RAM_IMMED:
                fprintf(output, " [%d]", *((int *)(bin + *cursor)));
                *cursor += sizeof(int);
            break;
                case RAM_REG:
                reg = parseRegister(*((int *)(bin + *cursor)));
                if (reg == nullptr) {
                    printf(ANSI_COLOR_RED "Unknown register number %d. Terminating...\n" ANSI_COLOR_RESET, *((int *)(bin + *cursor)));
                }
                fprintf(output, " [%s]", reg);
                *cursor += sizeof(int);
            break;
                case RAM_REG_IMMED:
                if((len - *cursor) <= 7) {
                    printf(ANSI_COLOR_RED "Unexpected EOF. Terminating...\n" ANSI_COLOR_RESET);
                    return -1;
                }
                regarg = parseRegister(*((int *)(bin + *cursor)));
                if (regarg == nullptr) {
                    printf(ANSI_COLOR_RED "Unknown register number %d. Terminating...\n" ANSI_COLOR_RESET, *((int *)(bin + *cursor)));
                }
                int num = *((int *)(bin + *cursor + sizeof(int)));
                if (num >= 0) {
                    fprintf(output, " [%s+%d]", regarg, num);
                }
                else { 
                    fprintf(output, " [%s-%d]", regarg, -num);
                }
                *cursor += 2 * sizeof(int);
            }
        }
        fprintf(output, "\n");
}

int disassemble(FILE *output, char *bin, size_t len, label_t *labels) {
    int cursor = 0;
    label_t *curLabel = {};
    char cmdName[MAX_NAME_LENGTH] = "";
    int argNum = 0;
    while (cursor < len) {
        curLabel = findLabel(labels, cursor);
        if(curLabel->label) {
            fprintf(output, "%s:\n", curLabel->label);
        }

#define DEF_CMD(name, args, overloaders) \
        argNum = args; \
        strcpy(cmdName, #name); \
        overloaders

#define CMD_OVRLD(opcode, cond, argtype, execcode) \
    if(bin[cursor] == opcode) defineCommandOverload(opcode, argNum, len, argtype, &cursor, bin, cmdName, labels, output);

#include "../commands.h"
#undef DEF_CMD
#undef CMD_OVRLD
    }
    return 1;
}

int parseParams(int argc, char *argv[], char **filename) {
    if (argc == 1) {
        printf(ANSI_COLOR_YELLOW "File have not been specified. Using default %s\n" ANSI_COLOR_RESET, defaultFilename);
        *filename = (char *) calloc(defaultFilenameLength, sizeof(char));
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

size_t fileSize(FILE *f) {
    assert(f);

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    return size;
}
