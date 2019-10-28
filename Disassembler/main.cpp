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

int parseParams(int argc, char *argv[], char **filename);

int loadFile(FILE **f, const char *loadpath, const char *mode);

unsigned long fileSize(FILE *f);

const char *defaultFilename = "prog.bin";

int disassemble(FILE *output, char *bin, int len, label_t *labels);

void i_to_a(int num, char *buffer, int radix = 10);

label_t *parseLabels(char *bin, int len);

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

    int size = fileSize(bin);
    char *machineCode = (char *) calloc(size + 1, sizeof(char));
    fread(machineCode, sizeof(char), size, bin);

    label_t *labels = parseLabels(machineCode, size);

    disassemble(disassembled, machineCode, size, labels);
    free(labels);
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

label_t *findLabel(label_t *labels, int pos) {
    while(labels->label != nullptr) {
        if (labels->pos == pos)
            break;
        labels++;
    }
    return labels;
}

label_t *parseLabels(char *bin, int len) {
    char number[MAX_INT_LENGTH] = "";
    int argNum = 0;
    char cmdName[MAX_NAME_LENGTH];
    auto labels = (label_t *) calloc(len + 1, sizeof(label_t));
    int labelNum = 0;
    enum argumentTypes {
        NONE,
        NUMBER,
        REGISTER,
        RAM_IMMED,
        RAM_REG,
        RAM_REG_IMMED,
        LABEL
    };
    int cursor = 0;
    while (cursor < len) {

#define DEF_CMD(name, args, overloaders) \
        argNum = args; \
        strcpy(cmdName, #name); \
        if (false) {} \
        overloaders


#define CMD_OVRLD(opcode, cond, argtype, execcode) \
    else if(bin[cursor] == opcode) { \
        cursor += sizeof(char); \
        if (argNum > 0) { \
            if((len - cursor) <= 3) { \
                printf(ANSI_COLOR_RED "Unexpected EOF. Terminating...\n" ANSI_COLOR_RESET); \
                return nullptr; \
            } \
            if (argtype == LABEL) {\
                int pos = *((int *)(bin + cursor)); \
                label_t *curLabel = findLabel(labels, pos); \
                if(curLabel->label == nullptr) { \
                    if(pos > len) { \
                        printf(ANSI_COLOR_RED "Found label linking outside the program. Terminating...\n" ANSI_COLOR_RESET); \
                        return nullptr; \
                    } \
                    curLabel->pos = pos; \
                    i_to_a(labelNum++, number, 10); \
                    curLabel->label = concatenate("meow", number); \
                } \
                cursor += sizeof(int); \
            } \
            else if (argtype == NUMBER) { \
                cursor += sizeof(int); \
            } \
            else if (argtype == REGISTER) { \
                cursor += sizeof(int); \
            } \
            else if (argtype == RAM_IMMED) { \
                cursor += sizeof(int); \
            } \
            else if (argtype == RAM_REG) { \
                cursor += sizeof(int); \
            } \
            else if (argtype == RAM_REG_IMMED) { \
                cursor += 2 * sizeof(int); \
            } \
        } \
    } \

#include "../commands.h"

#undef DEF_CMD
#undef CMD_OVRLD
    }
    return labels;
}


const char *parseRegister(int reg) {
    if (reg == 0) {
        return "ax";
    }
    else if (reg == 1) {
        return "bx";
    }
    else if (reg == 2) {
        return "cx";
    }
    else if (reg == 3) {
        return "dx";
    }
    else {
        return nullptr;
    }
}

int disassemble(FILE *output, char *bin, int len, label_t *labels) {
    enum argumentTypes {
        NONE,
        NUMBER,
        REGISTER,
        RAM_IMMED,
        RAM_REG,
        RAM_REG_IMMED,
        LABEL
    };
    int cursor = 0;
    label_t *curLabel = {};
    char cmdName[MAX_NAME_LENGTH] = "";
    int argNum = 0;
    while (cursor < len) {
        curLabel = findLabel(labels, cursor);
        if(curLabel->label != nullptr) {
            fprintf(output, "%s:\n", curLabel->label);
        }
#define DEF_CMD(name, args, overloaders) \
        argNum = args; \
        strcpy(cmdName, #name); \
        overloaders

#define CMD_OVRLD(opcode, cond, argtype, execcode) \
    if(bin[cursor] == opcode) { \
        fprintf(output, "%s", cmdName); \
        cursor += sizeof(char); \
        if (argNum > 0) { \
            if((len - cursor) <= 3) { \
                printf(ANSI_COLOR_RED "Unexpected EOF. Terminating...\n" ANSI_COLOR_RESET); \
                return -1; \
            } \
            if (argtype == LABEL) { \
                label_t *curLabel = findLabel(labels, *((int *)(bin + cursor))); \
                if(curLabel->label == nullptr) { \
                    printf(ANSI_COLOR_RED"Label not found! Terminating...\n" ANSI_COLOR_RESET); \
                    return -1; \
                } \
                fprintf(output, " %s", curLabel->label); \
                cursor += sizeof(int); \
            } \
            else if (argtype == NUMBER) { \
                fprintf(output, " %d", *((int *)(bin + cursor))); \
                cursor += sizeof(int); \
            } \
            else if (argtype == REGISTER) { \
                const char *reg = parseRegister(*((int *)(bin + cursor))); \
                if (reg == nullptr) { \
                    printf(ANSI_COLOR_RED "Unknown register number %d. Terminating...\n" ANSI_COLOR_RESET, *((int *)(bin + cursor))); \
                } \
                fprintf(output, " %s", reg); \
                cursor += sizeof(int); \
            } \
            else if (argtype == RAM_IMMED) { \
                fprintf(output, " [%d]", *((int *)(bin + cursor))); \
                cursor += sizeof(int); \
            } \
            else if (argtype == RAM_REG) { \
                const char *reg = parseRegister(*((int *)(bin + cursor))); \
                if (reg == nullptr) { \
                    printf(ANSI_COLOR_RED "Unknown register number %d. Terminating...\n" ANSI_COLOR_RESET, *((int *)(bin + cursor))); \
                } \
                fprintf(output, " [%s]", reg); \
                cursor += sizeof(int); \
            } \
            else if (argtype == RAM_REG_IMMED) { \
                if((len - cursor) <= 7) { \
                    printf(ANSI_COLOR_RED "Unexpected EOF. Terminating...\n" ANSI_COLOR_RESET); \
                    return -1; \
                } \
                const char *regarg = parseRegister(*((int *)(bin + cursor))); \
                if (regarg == nullptr) { \
                    printf(ANSI_COLOR_RED "Unknown register number %d. Terminating...\n" ANSI_COLOR_RESET, *((int *)(bin + cursor))); \
                } \
                int num = *((int *)(bin + cursor + sizeof(int))); \
                if (num >= 0) {\
                    fprintf(output, " [%s+%d]", regarg, num);\
                } \
                else { \
                    fprintf(output, " [%s-%d]", regarg, -num); \
                } \
                cursor += 2 * sizeof(int); \
            } \
        } \
        fprintf(output, "\n"); \
    }

        if (0 == 1) continue;
#include "../commands.h"
#undef DEF_CMD
#undef CMD_OVRLD
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


void i_to_a(int num, char *buffer, int radix) {
    assert(buffer);
    assert((radix >= 2) && (radix <= 36));
    if (num == 0) {
        *buffer++ = '0';
        *buffer = '\0';
        return;
    }
    int sign = 1;
    if (num < 0) {
        sign = -1;
        num = -num;
    }
    int numCopy = num, size = 0, digit;
    while (numCopy > 0) {
        numCopy /= radix;
        size++;
    }
    if (sign == -1) {
        *buffer = '-';
        ++size;
    }
    buffer += size;
    *buffer = '\0';
    --buffer;

    while (num > 0) {
        digit = num % radix;
        num /= radix;
        if ((digit >= 0) && (digit <= 9)) {
            *buffer = digit + '0';
        } else {
            *buffer = digit - 10 + 'A';
        }
        --buffer;
    }
}