#include <stdio.h>
#include <assert.h>
#include <windows.h>

int savetxt(char **linePointers, char *savepath, int lines);

unsigned long filesize(FILE *f);

int cmp_str(const void *a, const void *b);

int main() {
    char loadpath[255], savepath[255];
    printf("Enter original path:");
    scanf("%s", loadpath);
    assert(loadpath);
    printf("Enter destination path:");
    scanf("%s", savepath);
    assert(savepath);
    FILE *original = fopen(loadpath, "r");
    if (original == nullptr) {
        printf("An error occured while opening the file\n");
        return -1;
    }

    unsigned long size = filesize(original);

    char *txt = (char *) malloc(size);
    char *txtbegin = txt;
    fread(txt, sizeof(char), size, original);
    int lines = 0;
    while (*txt != '\0') {
        if (*txt == '\n') {
            *txt = '\0';
            ++lines;
        }
        ++txt;
    }
    ++lines;


    char **linePointers = (char **) calloc(lines, sizeof(char *));
    char **lp;
    lp = linePointers;
    txt = txtbegin;
    for (int i = 0; i < lines; i++) {
        *lp = txtbegin;
        ++lp;
        while (*txtbegin != '\0') ++txtbegin;
        ++txtbegin;
    }
    fclose(original);
    qsort(linePointers, lines, sizeof(char *), cmp_str);
    if (savetxt(linePointers, savepath, lines) == EOF) {
        printf("An error occured while saving the file");
        return -1;
    }
    free(txt);
    free(linePointers);
    return 0;
}


/**
 * Function that calculates the size of the file
 * @param f File
 * @return Size of the file
 */
unsigned long filesize(FILE *f) {
    fseek(f, 0, SEEK_END);
    unsigned long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

/**
 * Function that saves lines into file
 * @param linePointers Pointer to the array of pointers to strings
 * @param savepath Path for saving
 * @param lines Amount of lines
 * @return Positive if file saved successfully, EOF if error occured
 */

int savetxt(char **linePointers, char *savepath, int lines) {
    assert(linePointers);
    assert(savepath);
    assert(lines);
    FILE *f = fopen(savepath, "w");
    for (int i = 0; i < lines; i++) {
        if (fputs(*linePointers, f) == EOF) return EOF;
        if (fputc('\n', f) == EOF) return EOF;
        ++linePointers;
    }
    fclose(f);
    return 1;
}

/**
 * qsort comparator for strings
 * @param a pointer to the pointer to the first string
 * @param b pointer to the pointer to the second string
 * @return zero if strings are equal, negative if a < b, positive if a > b
 */
int cmp_str(const void *a, const void *b) {

    assert(a);
    assert(b);

    char **string1 = (char **) a;
    char **string2 = (char **) b;
    return strcmp(*string1, *string2);
}