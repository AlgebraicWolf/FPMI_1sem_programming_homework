#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

unsigned long getFileSize(FILE* f);

float unaryOperation(char* str);

float binaryOperation(char* str);

float Not(float a);

float Or(float a, float b);

float And(float a, float b);

int main() {
    FILE* finput = fopen("input.txt", "r");
    FILE* foutput = fopen("output.txt", "w");
    assert(finput);
    assert(foutput);

    unsigned long size = getFileSize(finput);
    char* inputString = (char*)calloc(size, sizeof(char));
    fread(inputString, sizeof(char), size, finput);
    fclose(finput);

    float result = 0.0f;

    if(*inputString == '~') {
        result = unaryOperation(inputString);
    }
    else {
        result = binaryOperation(inputString);
    }
    fprintf(foutput, "%0.2f\n",result);
    fclose(foutput);
}

float unaryOperation(char* str) {
    assert(str);
    ++str;
    float parsedFloat = atof(str);
    return Not(parsedFloat);
}

float binaryOperation(char* str) {
    assert(str);
    float parsedFloat1 = atof(str);
    while((*str != '*' ) && (*str != '+')) {
        str++;
    }
    char op = *str++;
    float parsedFloat2 = atof(str);
    if (op == '+') return Or(parsedFloat1, parsedFloat2);
    else if (op == '*') return And(parsedFloat1, parsedFloat2);
    return 0;
}

float Not(float a) {
    return 1 - a;
}

float Or(float a, float b) {
    return a + b - a * b;
}

float And(float a, float b) {
    return a * b;
}

unsigned long getFileSize(FILE* f) {
    assert(f);
    fseek(f, 0, SEEK_END);
    unsigned int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}