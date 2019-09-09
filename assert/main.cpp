#include <stdio.h>
#include <process.h>

#define DEBUG
#define MEOW


#ifdef MEOW
#define PRINTF printf
#else
#define PRINTF if(0) printf
#endif


#ifdef DEBUG
#define ASSERT( cond ) \
    { \
        if(!(cond)) { \
            printf("Assertion failed: %s, file %s, function %s,  line %d\n", #cond, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
            abort(); \
        } \
    } \

#else
#define ASSERT( cond ) ;
#endif

int main() {
    ASSERT(false);
}
void assert(bool cond) { //Not working
    if (!cond) {
        printf("Assertion failed: %s, file: %s, line: %d\n", "cond", __FILE__, __LINE__);
        abort();
    }
}