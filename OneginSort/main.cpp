#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#define DEBUG

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

bool unitTesting();

int saveTxt(myString_t *linePointers, FILE *f, bool yieldErrors = true);

unsigned long fileSize(FILE *f);

int cmpStr(const void *a, const void *b);

void readPath(char *loadpath, char *savepath);

int loadFile(FILE **f, const char *loadpath, const char *mode);

int countLines(const char *txt);

void preprocessText(char *txt);

void findLines(const char *txt, myString_t *linePointers, int lines);

bool isLetter(char ch);

int reverseCmpStr(const void *a, const void *b);

char convertChar(char ch);

int saveOriginalTxt(const char *txt, int lines, FILE *f);

void skipNonLettersForward(char **string);

void skipNonLettersBackward(char **string, char *end);

int
loadRWFiles(FILE **readFile, FILE **writeFiles, const char *loadpath, const char *savepath, bool yieldErrors = true);

int main() {
#ifdef DEBUG
    char unitTestResult = unitTesting();
    if (unitTestResult) printf("====================\nUnit tests finished successfully\n====================\n");
    else printf("====================\nAn error occurred during unit testing, check the detailed log above\n====================\n");
#endif

    char loadpath[FILENAME_MAX] = {}, savepath[FILENAME_MAX] = {};
    FILE *original = nullptr, *sorted = nullptr;

    readPath(loadpath, savepath);

    if (loadRWFiles(&original, &sorted, loadpath, savepath) == -1) return -1;

    unsigned long size = fileSize(original);
    char *txt = (char *) calloc(1, size);
    fread(txt, sizeof(char), size, original);
    int lines = countLines(txt);
    preprocessText(txt);

    myString_t *linePointers = (myString_t *) calloc(lines + 1, sizeof(myString_t));
    findLines(txt, linePointers, lines);
    fclose(original);

    qsort(linePointers, lines, sizeof(myString_t), cmpStr);

    fprintf(sorted, "\n==Sorting from beginning to end==\n\n");
    if (saveTxt(linePointers, sorted) == EOF) return -1;

    qsort(linePointers, lines, sizeof(myString_t), reverseCmpStr);

    fprintf(sorted, "\n==Sorting from end to beginning==\n\n");
    if (saveTxt(linePointers, sorted) == EOF) return -1;

    fprintf(sorted, "\n==Original text==\n\n");
    saveOriginalTxt(txt, lines, sorted);

    fclose(sorted);

    free(txt);
    free(linePointers);

    return 0;
}

/**
 * Function that opens two files - one in read and one in write mode
 * @param readFile Pointer to FILE* to variable for file in read mode
 * @param writeFiles Pointer to FILE* to variable for file in write mode
 * @param loadpath Path to file in read mode
 * @param savepath Path to file in write mode
 * @param yieldErrors Whether function should yield errors in console
 * @return 1 if OK, -1 otherwise
 */

int loadRWFiles(FILE **readFile, FILE **writeFiles, const char *loadpath, const char *savepath, bool yieldErrors) {
    if (!loadFile(readFile, loadpath, "r"))
        {
        if (yieldErrors) printf("An error occurred while opening the file: %s\n", loadpath);
        return -1;
        }

    if (!loadFile(writeFiles, savepath, "w"))
        {
        if (yieldErrors) printf("An error occurred while opening the file: %s\n", savepath);
        return -1;
        }

    return 1;
}

/**
 * Function that performs unit testing
 * @return True when all the test are correct, false otherwise
 */

bool unitTesting() {
    // Tests for isLetter function
    bool successFlag = true;
    for (char ch = 'a'; ch <= 'z'; ch++)
    UTEST(isLetter(ch), successFlag);
    for (char ch = 'A'; ch <= 'Z'; ch++)
    UTEST(isLetter(ch), successFlag);
    UTEST(!isLetter('.'), successFlag);
    UTEST(!isLetter(','), successFlag);
    UTEST(!isLetter('\''), successFlag);
    UTEST(!isLetter('\"'), successFlag);
    UTEST(!isLetter('-'), successFlag);

    // Tests for convertChar function
    for (char ch = 'a'; ch <= 'z'; ch++)
    UTEST(convertChar(ch) == convertChar(ch - 'a' + 'A'), successFlag);
    UTEST(convertChar('.') == '.', successFlag);
    UTEST(convertChar(',') == ',', successFlag);
    UTEST(convertChar('\'') == '\'', successFlag);
    UTEST(convertChar('\"') == '\"', successFlag);
    UTEST(convertChar('-') == '-', successFlag);

    //Tests for strCmp and reverseStrCmp function
    char sraw1[9] = "aff  ffs";
    char sraw2[12] = "gffffe.-hhh";
    myString_t s1 = {sraw1, sraw1 + 7};
    myString_t s2 = {sraw2, sraw2 + 10};
    void *s1VoidPointer = (void *) &s1;
    void *s2VoidPointer = (void *) &s2;
    UTEST(cmpStr(s1VoidPointer, s2VoidPointer) > 0, successFlag);
    UTEST(reverseCmpStr(s1VoidPointer, s2VoidPointer) < 0, successFlag);

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
    skipNonLettersForward(&nonLettersF);
    skipNonLettersBackward(&nonLettersBLast, nonLettersB);
    UTEST(*nonLettersF == 'a', successFlag);
    UTEST(*nonLettersBLast == 'c', successFlag);
    return successFlag;
}

/**
 * Counts the number of lines
 * @param txt String for preprocessing
 * @return Number of lines
 */

int countLines(const char *txt) {
    ASSERT(txt);

    int lines = 0;
    while (*txt != '\0')
        {
        if (*txt == '\n')
            ++lines;
        ++txt;
        }
    ++lines;

    return lines;
}

/**
 * Function that preprocess text (i. e. replaces '\\n' symbols with '\0')
 * @param txt String for preprocessing
 */

void preprocessText(char *txt) {
    ASSERT(txt);

    while (*txt != '\0')
        {
        if (*txt == '\n')
            {
            *txt = '\0';
            }
        ++txt;
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
        while (*txt != '\0') ++txt;
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
 * @param loadpath Pointer to string for path to original text
 * @param savepath Pointer to string for path for saving
 */

void readPath(char *loadpath, char *savepath) {
    printf("Enter original path:");
    scanf("%s", loadpath);
    ASSERT(*loadpath);

    printf("Enter destination path:");
    scanf("%s", savepath);
    ASSERT(*savepath);
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
int saveTxt(myString_t *linePointers, FILE *f, bool yieldErrors) {
    ASSERT(linePointers);
    ASSERT(f);

    while (linePointers->beginning != nullptr)
        {
        if (*(linePointers->beginning) != '\0')
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
    for (int i = 0; i < lines; i++)
        {
        if (fputs(txt, f) == EOF)
            return EOF;
        if (fputc('\n', f) == EOF)
            return EOF;
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
        if (!isLetter(*string1) || !isLetter(*string2))
            {
            skipNonLettersForward(&string1);
            skipNonLettersForward(&string2);
            }
        else
            {
            if ((*string1 - *string2) != 0)
                break;
            }
        ++string1;
        ++string2;
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
    while ((string1 >= string1Pointer->beginning) && (string2 >= string2Pointer->beginning))
        {
        if (!isLetter(*string1) || !isLetter(*string2))
            {
            skipNonLettersBackward(&string1, string1Pointer->beginning);
            skipNonLettersBackward(&string2, string2Pointer->beginning);
            }
        else
            {
            if ((convertChar(*string1) - convertChar(*string2)) != 0)
                break;
            }
        --string1;
        --string2;
        }

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
    if (isLetter(ch)) return tolower(ch) - 'a';
    else return ch;
}

/**
 * Checks whether given character is a
 * \todo Add russian characters support
 *
 * @param ch Character for checking
 * @return is letter a character
 */

bool isLetter(char ch) {
    if ((ch >= 'a') && (ch <= 'z')) return true;
    else if ((ch >= 'A') && (ch <= 'Z')) return true;
        //else if((ch >= 'А') && (ch <= 'Я')) return true;
        //else if((ch >= 'А') && (ch <= 'Я')) return true;
    else return false;
}

/**
 * Function that skips all the non-letter characters moving towards the end of string
 * @param string Pointer to the string (pointer to the char array) for skipping
 */

void skipNonLettersForward(char **string) {
    while (!isLetter(**string) && (**string != '\0'))
        (*string)++;
}

/**
 * Function that skips all the non-letter characters moving towards the beginning
 * @param string Pointer to the string (pointer to the char array) for skipping
 * @param end Pointer to the character to stop
 */

void skipNonLettersBackward(char **string, char *end) {
    while (!isLetter(**string) && (*string != end))
        (*string)--;
}