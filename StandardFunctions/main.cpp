#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


int a_to_i(const char *str);

void i_to_a(int num, char *buffer, int radix);

int str_cmp(char *string1, char *string2);

int put_s(char *string);

char *f_get_s(char *str, int count, FILE *stream);

int main() {
    char atoi_test[8] = "1234 hi";
    char buf[32];
    char s1[5]="AAAA";
    char s2[5]="AAAa";
    char filename[10] = "input.txt";
    char prompt[36] = "This string was printed using put_s";
    FILE *f = fopen(filename, "r");

    printf("a_to_i(\"1234 hi\") = %d\n", a_to_i(atoi_test));

    i_to_a(1337, buf, 18);
    printf("i_to_a(1337, buf, 18) = %s\n", buf);

    printf("str_cmp(\"AAAA\", \"AAAa\") = %d\n", str_cmp(s1, s2));
    put_s(prompt);

    f_get_s(buf, 8, f);
    printf("First eight symbols of input.txt are \"%s\"", buf);
    fclose(f);
}

/**
 * \brief Function that translates string (char array) into number
 * Function skips whitespaces in the beginning of the string, then decodes number until it meets either end of string or non-digit character
 * @param str Pointer to char array that contains number
 * @return Decoded number
 */
int a_to_i(const char *str) {
    assert(str);
    int result = 0;
    int i = 0;
    int sign = 1;
    while (*str == ' ') ++str;
    if (*str == '-') {
        sign = -1;
        ++str;
    }
    while ((*str >= '0') && (*str <= '9')) {
        result = result * 10 + (int) *str - '0';
        ++str;
    }
    return sign * result;
}

/**
 * \brief Function that encodes number into string
 * Function encodes number into string representation using given radix. Default base is 10.
 * @param num Number to encode
 * @param buffer Pointer to char array for writing number
 * @param radix Radix (base) of positional numeral system used for encoding the number. Shoud be number between 2 and 36 (including borders)
 */
void i_to_a(int num, char *buffer, int radix = 10) {
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

/**
 * \brief function that compares two strings
 * @param string1 Pointer to the first string
 * @param string2 Pointer to the second string
 * @return Result of comparison, 0 if the strings are equal, positive number if first string greater than second, negative number if second greater than first.
 */
int str_cmp(char *string1, char *string2) {
    assert(string1);
    assert(string2);
    while ((*string1 - *string2) == 0) {
        if (*string1 == '\0') return 0;
        ++string1;
        ++string2;
    }
    return *string1 - *string2;
}


/**
 * Function that prints given string and newline character.
 * @param string String for printing
 * @return Non-negative number integer in case of success. If error occured, EOF
 */
int put_s(char *string) {
    assert(string);
    int code;
    while (*string != '\0') {
        code = putchar(*string);
        if (code == EOF) return EOF;
        ++string;
    }
    code = putchar('\n');
    return code;
}

/**
 * \brief Function for reading from file
 * @param str Pointer to string for saving the number
 * @param count Number of symbols to read
 * @param stream Pointer to the file for reading
 * @return NULL if the end is reached. Otherwise, pointer to string
 */
char *f_get_s(char *str, int count, FILE *stream) {
    assert(str);
    assert(stream);
    char *orig;
    orig = str;
    char ch;
    int i;
    for (i = 0; i < (count - 1); i++) {
        ch = fgetc(stream);
        if (ch == '\n') {
            *str = ch;
            ++str;
            break;
        } else if (ch == EOF) {
            *str = '\0';
            if (i == 0) return NULL;
            else {
                --stream;
                ++str;
                break;
            }
        }
        *str = ch;
        ++str;
    }
    *str = '\0';
    return orig;
}