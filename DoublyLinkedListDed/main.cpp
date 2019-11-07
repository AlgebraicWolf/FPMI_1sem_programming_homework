#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "stack.h"

struct list_t {
    void **value;
    long long *next;
    long long *prev;
    long long head;
    long long tail;
    size_t size;
    size_t maxsize;
    stack_t *free;
};

list_t *createList(size_t maxsize) {
    list_t *list = (list_t *) calloc(1, sizeof(list_t));
    list->size = 0;
    list->maxsize = maxsize;
    list->next = (long long *) calloc(maxsize, sizeof(long long));
    list->prev = (long long *) calloc(maxsize, sizeof(long long));
    list->head = -1;
    list->tail = -1;
    stackConstruct(list->free, "ListFreeStack", maxsize, -1);

    for(long long i = maxsize-1; i >= 0; i--)
        stackPush(list->free, i);

    return list;
}

int main() {

}