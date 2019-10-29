#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *extendNumber(char *num, int n);

char *reverseNumber(char *num);

char *naiveMultiplication(char *a, char *b);

char *summation(char *a, char *b);

char *shiftNumber(char *num, int n);

char *karatsubaMultiplication(char *a, char *b);


const int NAIVE_THRESHOLD = 2;

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");
    auto a = (char *)calloc(1025, sizeof(int));
    auto b = (char *)calloc(1025, sizeof(int));;
    printf("%s\n", reverseNumber(summation(reverseNumber("999"), reverseNumber("111"))));

    fscanf(input, "%s %s", a, b);
    int la = strlen(a);
    int lb = strlen(b);

    if (la > lb) b = reverseNumber(extendNumber(reverseNumber(b), la));
    else if (lb > la) a = reverseNumber(extendNumber(reverseNumber(a), lb));

    if((strlen(a) % 2) == 1) {
        b = reverseNumber(extendNumber(reverseNumber(b), strlen(a) + 1));
        a = reverseNumber(extendNumber(reverseNumber(a), strlen(b)));
    }

    char *res = reverseNumber(karatsubaMultiplication(reverseNumber(a), reverseNumber(b)));
    printf("%s", res);

    fclose(input);
    fclose(output);
    return 0;
}

void multiply(char *a, char *b, char *res) {
    int la = strlen(a);
    int lb = strlen(b);
    int len = 0;
    if (la < lb) len = lb;
    else len = la;
    if(len == 1)
    if (len % 2 != 0) len++;

}

char *reverseNumber(char *num) {
    int len = strlen(num);
    auto rnum = (char *) calloc(len, sizeof(char));
    for(int i = 0; i < len; i++) {
        rnum[i] = num[len - 1 - i];
    }
    return rnum;
}

char *extendNumber(char *num, int n) {
    auto longNum = (char *) calloc(n, sizeof(char));
    memset(longNum + strlen(num), '0', n - strlen(num));
    strncpy(longNum, num, strlen(num));
    return longNum;
}

char *naiveMultiplication(char *a, char *b) {
    int len = strlen(a); // Assuming they have the same length
    auto result = (char *) calloc(2 * len, sizeof(char));
    for(int i = 0; i < len; i++) {
        for(int j = 0; j < len; j++) {
            result[i + j] += (a[i] - '0') * (b[j] - '0');
        }
    }
    for(int i = 0; i < len + 1; i++) {
        result[i+1] += result[i] / 10;
        result[i] = result[i] % 10 + '0';
    }
    return result;
}

char *summation(char *a, char *b) {
    int len = strlen(a); // Assuming they have the same length
    auto result = (char *) calloc(1 + len, sizeof(char));

    for(int i = 0; i < len; i++) {
        result[i] += a[i] + b[i] - '0';
        result[i + 1] += (result[i] - '0') / 10;
        result[i] = (result[i] - '0' ) % 10 + '0';
    }
    if (result[len] != 0) result[len] += '0';
    return result;
}

char *substraction(char *a, char *b) {

}

char *karatsubaMultiplication(char *a, char *b) {
    int len = strlen(a); // Assuming numbers have equal length;

    if(len <= NAIVE_THRESHOLD)
        return naiveMultiplication(a, b);
    int subLen = len / 2;

    auto result = (char *) calloc(2 * len, sizeof(char));
    auto a_r = (char *) calloc(subLen, sizeof(char));
    auto a_l = (char *) calloc(subLen, sizeof(char));
    auto b_r = (char *) calloc(subLen, sizeof(char));
    auto b_l = (char *) calloc(subLen, sizeof(char));

    strncpy(a_r, a, subLen);
    strncpy(a_l, a + subLen, subLen);
    strncpy(b_r, b, subLen);
    strncpy(b_l, b + subLen, subLen);

    char *a_rl = summation(a_r, a_l);
    char *b_rl = summation(b_r, b_l);

    char *product1 = karatsubaMultiplication(a_l, b_l);
    char *product2 = karatsubaMultiplication(a_r, b_r);
    char *product3 = karatsubaMultiplication(a_rl, b_rl);

    for(int i = 0; i < 2 * len; i++) {
        product3[i] -= (product1[i] - '0') + (product2[i] - '0');
    }
    for(int i = 0; i < len; i++) {
        result[i] = product2[i];
    }
    for(int i = len; i < 2 * len; i++ ) {
        result[i] = product1[i - len];
    }
    for(int i = subLen; i < len + subLen; i++) {
        result[i] += product3[i-subLen] - '0';
    }

    return result;
}

char *shiftNumber(char *num, int n) {
    return reverseNumber(extendNumber(reverseNumber(num), n + strlen(num)));
}