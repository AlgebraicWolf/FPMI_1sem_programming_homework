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
    list->value = (void **) calloc(maxsize, sizeof(void *));
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

void clearList(list_t *list) {
    assert(list);

    long long curNode = list->head;
    long long next = -1;

    while(curNode != list->tail) {
        next = list->next[curNode];

        list->next[curNode] = -1;
        list->prev[curNode] = -1;
        list->value[curNode] = nullptr;
        stackPush(list->free, curNode);

        curNode = next;
    }

    list->head = -1;
    list->tail = -1;
    list->size = 0;
}

void deleteList(list_t **list) {
    assert(list);
    assert(*list);

    clearList(*list);

    free(*list);
    *list = nullptr;
}

int main() {

}