#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#define RELEASE

#ifdef DEBUG
#define UTEST(cond, successFlag) { \
    if(!cond) { \
        printf("Unit testing failed: %s\n", #cond); \
        successFlag = false; \
    } \
    else { \
        printf("Unit testing OK: %s\n", #cond); \
    } \
} \

#else
#define UTEST(cond, successFlag) ;
#endif

#ifdef DEBUG
#define ASSERT(cond) { \
    if (!cond) {\
        printf("Assertion failed: %s, file: %s, function: %s, line: %d\n", #cond, __FILE__, __PRETTY_FUNCTION__, __LINE__);\
        abort(); \
    }\
}\

#else
#define ASSERT(cond) ;
#endif

/**
 * Structure that saves pointers to the first and last characters of string
 * @param beginning Pointer to the first character of string
 * @param ending Pointer to the last character of string (excluding '\0' symbol)
 */
struct myString_t {
    char *beginning;
    char *ending;
};

const bool yieldErrors = true;

bool unitTesting();

int saveTxt(const myString_t *linePointers, FILE *f);

unsigned long fileSize(FILE *f);

int cmpStr(const void *a, const void *b);

void readPath(char *path, const char *prompt);

int loadFile(FILE **f, const char *loadpath, const char *mode);

int countLines(const char *, const char delimiter);

void preprocessText(char *txt, const char delimiter);

void findLines(const char *txt, myString_t *linePointers, int lines);

int reverseCmpStr(const void *a, const void *b);

char convertChar(const char ch);

int saveOriginalTxt(const char *txt, int lines, FILE *f);

void skipNonLettersForward(const char **string);

void skipNonLettersBackward(const char **string, const char *end);

int doLoadingRoutine(const char *loadpath, char **txt, myString_t **linePointers);

int main() {
#ifdef DEBUG
    char unitTestResult = unitTesting();
    if (unitTestResult) printf("====================\nUnit tests finished successfully\n====================\n");
    else printf("====================\nAn error occurred during unit testing, check the detailed log above\n====================\n");
#endif

    char loadpath[FILENAME_MAX] = {}, savepath[FILENAME_MAX] = {};
    FILE *sorted = nullptr;
    char *txt = nullptr;
    myString_t *linePointers = nullptr;

    readPath(loadpath, "Enter source path:");
    readPath(savepath, "Enter destination path:");

    loadFile(&sorted, savepath, "w");
    int lines = doLoadingRoutine(loadpath, &txt, &linePointers);

    if(lines == -1) return -1;

    qsort(linePointers, lines, sizeof(myString_t), cmpStr);

    fprintf(sorted, "\n==Sorting from beginning to end==\n\n");
    if (saveTxt(linePointers, sorted) == EOF) return -1;

    qsort(linePointers, lines, sizeof(myString_t), reverseCmpStr);

    fprintf(sorted, "\n==Sorting from end to beginning==\n\n");
    if (saveTxt(linePointers, sorted) == EOF) return -1;

    fprintf(sorted, "\n==Original text==\n\n");
    if (saveOriginalTxt(txt, lines, sorted) == EOF) return -1;

    fclose(sorted);

    free(txt);
    free(linePointers);

    return 0;
}

/**
 * Function that reads file and prepares it for the later operations
 * @param loadpath Path to the file
 * @param txt Pointer to the string array
 * @param linePointers Pointer to the myString_t array
 * @return Number of lines in file
 */

int doLoadingRoutine(const char *loadpath, char **txt, myString_t **linePointers) {
    ASSERT(loadpath);
    FILE *original = nullptr;

    if (!loadFile(&original, loadpath, "r"))
        {
        if (yieldErrors) printf("An error occurred while opening the file: %s\n", loadpath);
        return -1;
        }

    unsigned long size = fileSize(original);
    *txt = (char *) calloc(1, size);
    fread(*txt, sizeof(char), size, original);

    int lines = countLines(*txt, '\n');
    preprocessText(*txt, '\n');

    *linePointers = (myString_t *) calloc(lines + 1, sizeof(myString_t));

    findLines(*txt, *linePointers, lines);
    fclose(original);

    return lines;
}

/**
 * Function that performs unit testing
 * @return True when all the test are correct, false otherwise
 */

bool unitTesting() {
    // Tests for isalpha function
    bool successFlag = true;
    for (char ch = 'a'; ch <= 'z'; ch++)
    UTEST(isalpha(ch), successFlag);
    for (char ch = 'A'; ch <= 'Z'; ch++)
    UTEST(isalpha(ch), successFlag);
    UTEST(!isalpha('.'), successFlag);
    UTEST(!isalpha(','), successFlag);
    UTEST(!isalpha('\''), successFlag);
    UTEST(!isalpha('\"'), successFlag);
    UTEST(!isalpha('-'), successFlag);

    // Tests for convertChar function
    for (char ch = 'a'; ch <= 'z'; ch++)
    UTEST(convertChar(ch) == convertChar(ch - 'a' + 'A'), successFlag);
    UTEST(convertChar('.') == '.', successFlag);
    UTEST(convertChar(',') == ',', successFlag);
    UTEST(convertChar('\'') == '\'', successFlag);
    UTEST(convertChar('\"') == '\"', successFlag);
    UTEST(convertChar('-') == '-', successFlag);

    //Tests for strCmp and reverseStrCmp function
    char sraw1[10] = "aff  ffs,";
    char sraw2[7] = ".-hhh.";
    char sraw3[4] = "hhh";
    myString_t s1 = {sraw1, sraw1 + 8};
    myString_t s2 = {sraw2, sraw2 + 5};
    myString_t s3 = {sraw3, sraw3 + 2};
    void *s1VoidPointer = (void *) &s1;
    void *s2VoidPointer = (void *) &s2;
    void *s3VoidPointer = (void *) &s3;
    UTEST(cmpStr(s1VoidPointer, s2VoidPointer) > 0, successFlag);
    UTEST(reverseCmpStr(s1VoidPointer, s2VoidPointer) < 0, successFlag);
    UTEST(reverseCmpStr(s1VoidPointer, s3VoidPointer) < 0, successFlag);
    UTEST(reverseCmpStr(s2VoidPointer, s3VoidPointer) == 0, successFlag);

    //Tests for countLines function
    char unprocessedString1[27] = "Hello\nHow\n\nAre you\n\n\ndoing";
    char unprocessedString2[29] = "Hello\nHow\n\nAre you\n\n\ndoing\n\n";
    UTEST(countLines(unprocessedString1) == 7, successFlag);
    UTEST(countLines(unprocessedString2) == 9, successFlag);

    //Tests for fileSize function
    FILE *fSizeTest = tmpfile();
    fprintf(fSizeTest, "Ajfemprejvf 546g5d6 d54g 4453 \n\n ewqawr --");
    rewind(fSizeTest);
    UTEST(fileSize(fSizeTest) == 43, successFlag);
    fclose(fSizeTest);

    //tests for skipNonLettersForward and skipNonLettersBackward
    char *nonLettersF = (char *) ".- -!-abc";
    char *nonLettersB = (char *) "-!-abc!?!? ---- ,//.,";
    char *nonLettersBLast = nonLettersB + 20;
    skipNonLettersForward((const char **)&nonLettersF);
    skipNonLettersBackward((const char **)&nonLettersBLast, nonLettersB);
    UTEST(*nonLettersF == 'a', successFlag);
    UTEST(*nonLettersBLast == 'c', successFlag);
    return successFlag;
}

/**
 * Counts the number of lines
 * @param txt String for preprocessing
 * @return Number of lines
 */

int countLines(const char *txt, const char delimiter) {
    ASSERT(txt);
    char* pointer = NULL;
    int lines = 0;
    while ((pointer = strchr(txt, delimiter)) != nullptr)
        {
        ++lines;
        txt = pointer + 1;
        }
    ++lines;

    return lines;
}

/**
 * Function that preprocess text (i. e. replaces '\\n' symbols with '\0')
 * @param txt String for preprocessing
 */

void preprocessText(char *txt, const char delimiter) {
    ASSERT(txt);
    char* pointer = NULL;
    while ((pointer = strchr(txt, delimiter)) != nullptr)
        {
            *pointer = '\0';
            txt = pointer + 1;
        }
}

/**
 * Function that parses raw text into array of myString_t objects
 * @param txt Text
 * @param linePointers Pointer to myString_t array
 * @param lines Number of lines
 */

void findLines(const char *txt, myString_t *linePointers, int lines) {
    ASSERT(txt);
    ASSERT(linePointers);

    for (int i = 0; i < lines; i++)
        {
        linePointers->beginning = (char *) txt;
        txt = strchr(txt, '\0');
        linePointers->ending = (char *) (txt - 1);
        ++linePointers;
        ++txt;
        }

    linePointers->beginning = nullptr;
    linePointers->ending = nullptr;
}

/**
 * Function that loads a file
 * @param f Pointer to a file
 * @param loadpath Path for loading a file
 * @param mode File mode
 * @return 0 if error occurred, 1 otherwise
 */
int loadFile(FILE **f, const char *loadpath, const char *mode) {
    ASSERT(f);
    ASSERT(loadpath);
    ASSERT(*loadpath);
    ASSERT(mode);
    ASSERT(*mode);

    *f = fopen(loadpath, mode);
    return *f != nullptr;
}

/**
 * Asks user for pathes for loading and saving
 * @param loadpath Pointer to string for path
 * @param savepath Prompt that asks user to give the path
 */

void readPath(char *loadpath, const char *prompt) {
    printf(prompt);
    scanf("%s", loadpath);
    ASSERT(*loadpath);
}

/**
 * Function that calculates the size of the file
 * @param f File
 * @return Size of the file
 */
unsigned long fileSize(FILE *f) {
    ASSERT(f);

    fseek(f, 0, SEEK_END);
    unsigned long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    return size;
}

/**
 * Function that saves lines into file
 * @param linePointers Pointer to the array of pointers to strings
 * @param f File for saving
 * @param lines Amount of lines
 * @param yieldErrors Whether function should yield error info into console
 * @return Positive if file saved successfully, EOF if error occurred
 */
int saveTxt(const myString_t *linePointers, FILE *f) {
    ASSERT(linePointers);
    ASSERT(f);

    extern const bool yieldErrors;

    while (linePointers->beginning != nullptr)
        {
        if (*(linePointers->beginning) != '\0' && *linePointers->beginning != '\n')
            {
            if (fputs(linePointers->beginning, f) == EOF)
                {
                if (yieldErrors) printf("An error occured while saving the file");
                return EOF;
                }
            if (fputc('\n', f) == EOF)
                {
                if (yieldErrors) printf("An error occured while saving the file");
                return EOF;
                }
            }
        ++linePointers;
        }

    return 1;
}

/**
 * Function that saves original preprocessed string
 * @param txt Preprocessed text
 * @param f File for saving
 * @param lines Number of lines
 * @return EOF when error occurres, 1 otherwise
 */

int saveOriginalTxt(const char *txt, int lines, FILE *f) {
    ASSERT(txt);
    ASSERT(f);

    extern const bool yieldErrors;

    for (int i = 0; i < lines; i++)
        {
        if (fputs(txt, f) == EOF)
            {
            if (yieldErrors) printf("An error occured while saving the file");
            return EOF;
            }
        if (fputc('\n', f) == EOF)
            {
            if (yieldErrors) printf("An error occured while saving the file");
            return EOF;
            }
        while (*txt != '\0') txt++;
        txt++;
        }
    return 1;
}

/**
 * qsort comparator for strings (from beginning to end)
 * @param a pointer to the first myString_t
 * @param b pointer to the second myString_t
 * @return Zero if strings are equal, negative if a < b, positive if a > b
 */
int cmpStr(const void *a, const void *b) {
    ASSERT(a);
    ASSERT(b);

    myString_t *string1Pointer = (myString_t *) a;
    myString_t *string2Pointer = (myString_t *) b;

    char *string1 = string1Pointer->beginning;
    char *string2 = string2Pointer->beginning;
    while ((*string1 != '\0') && (*string2 != '\0'))
        {
        if (!isalpha(*string1) || !isalpha(*string2))
            {
            skipNonLettersForward((const char **)&string1);
            skipNonLettersForward((const char **)&string2);
            }
        else
            {
            if ((*string1 - *string2) != 0)
                break;

            ++string1;
            ++string2;
            }
        }

    return *string1 - *string2;
}

/**
 * qsort comparator for strings (from end to beginning)
 * @param a pointer to the first myString_t
 * @param b pointer to the second myString_t
 * @return Zero if strings are equal, negative if a < b, positive if a > b
 */

int reverseCmpStr(const void *a, const void *b) {
    ASSERT(a);
    ASSERT(b);

    myString_t *string1Pointer = (myString_t *) a;
    myString_t *string2Pointer = (myString_t *) b;

    char *string1 = string1Pointer->ending;
    char *string2 = string2Pointer->ending;
    while ((string1 > string1Pointer->beginning) && (string2 > string2Pointer->beginning))
        {
        if (!isalpha(*string1) || !isalpha(*string2))
            {
            skipNonLettersBackward((const char **)&string1, string1Pointer->beginning);
            skipNonLettersBackward((const char **)&string2, string2Pointer->beginning);
            }
        else
            {
            if ((convertChar(*string1) - convertChar(*string2)) != 0)
                break;
            --string1;
            --string2;
            }
        }
    skipNonLettersBackward((const char **)&string1, string1Pointer->beginning);
    skipNonLettersBackward((const char **)&string2, string2Pointer->beginning);
    return convertChar(*string1) - convertChar(*string2);
}

/**
 * Function that converst character to its number in alphabet
 * \todo Add russian characters support
 *
 * @param ch Character to convert
 * @return Either number of letter in alphabet or, if given character is not a letter, code of the character
 */

char convertChar(char ch) {
    /*if ((ch >= 'a') && (ch <= 'z')) return ch - 'a';
    if ((ch >= 'A') && (ch <= 'Z')) return ch - 'A';
    //if ((ch >= 'а') && (ch <= 'я')) return ch - 'а';
    //if ((ch >= 'А') && (ch <= 'Я')) return ch - 'А';

    return ch;*/
    if (isalpha(ch)) return tolower(ch) - 'a';
    else return ch;
}

/**
 * Function that skips all the non-letter characters moving towards the end of string
 * @param string Pointer to the string (pointer to the char array) for skipping
 */

void skipNonLettersForward(const char **string) {
    ASSERT(string);

    while (!isalpha(**string) && (**string != '\0'))
        (*string)++;
}

/**
 * Function that skips all the non-letter characters moving towards the beginning
 * @param string Pointer to the string (pointer to the char array) for skipping
 * @param end Pointer to the character to stop
 */

void skipNonLettersBackward(const char **string, const char *end) {
    ASSERT(string);
    ASSERT(end);
    while (!isalpha(**string) && (*string != end))
        (*string)--;
}