#include <stdio.h>
#include <assert.h>

int mathematicalReverser(int integer);

void semiPointerReverser(int* integer);

void fullPointerReverser(int* integer);

void swapChar(unsigned char *a, unsigned char *b);

int main() {
    printf("Enter integer:");
    int integer = 0, integerCopy = 0;
    scanf("%d", &integer);
    integerCopy = integer;
    printf("Mathematical reversion: %d\n", mathematicalReverser(integer));
    semiPointerReverser(&integer);
    printf("Semi-pointer reversion: %d\n", integer);
    fullPointerReverser(&integerCopy);
    printf("Fully-pointer reversion: %d\n", integerCopy);
}

/**
 * Integer bytewise reversion using pointers to access bytes and mathematica to combine new integers
 * @param integer Pointer to integer
 */
void semiPointerReverser(int* integer) {
    unsigned char* recast = (unsigned char*) integer;
    *integer = *recast * 256 * 256 * 256 + *(recast + 1) * 256 * 256 + *(recast + 2) * 256 + *(recast + 3);
}


void fullPointerReverser(int* integer) {
    unsigned char* recast = (unsigned char*) integer;
    swapChar(recast, recast + 3);
    swapChar(recast + 1, recast + 2);
}


/**
 * Integer bytewise reversion using mathematics
 * @param integer Integer
 * @return Reversed integer
 */
int mathematicalReverser(int integer) {
    return (integer % 256) * 256 * 256 * 256 + (integer / 256 % 256) * 256 * 256 + (integer / 256 / 256 % 256) * 256 + integer / 256 / 256 / 256;
}


/**
 * Swapping values of two char variables
 * @param a Pointer to the first char variable
 * @param b Pointer to the second char variable
 */
void swapChar(unsigned char *a, unsigned char *b) {
    assert(a);
    assert(b);
    char tmp = '\0';
    tmp = *a;
    *a = *b;
    *b = tmp;
}