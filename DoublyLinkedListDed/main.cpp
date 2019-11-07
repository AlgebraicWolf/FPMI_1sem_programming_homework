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

int addToHead(list_t *list, void *value) {
    assert(list);

    if(list->size == list->maxsize)
        return 0;

    long long temp = list->head;
    long long newNode = 0;

    stackPop(list->free, &newNode);

    list->value[newNode] = value;

    if(temp != -1)  {
        list->prev[temp] = newNode;
    }

    list->head = newNode;

    if(list->tail == -1) {
        list->tail = newNode;
    }

    list->size++;

    return 1;
}

int addToTail(list_t *list, void *value) {
    assert(list);

    if(list->size == list->maxsize)
        return 0;

    long long temp = list->head;
    long long newNode = 0;

    stackPop(list->free, &newNode);

    list->value[newNode] = value;

    if(temp != -1) {
        list->next[temp] = newNode;
    }

    list->tail = newNode;

    if(list->head == -1) {
        list-> head = newNode;
    }

    list->size++;

    return 1;
}

int insertAfter(list_t *list, long long elem, void *value) {
    assert(list);
    assert(elem >= 0);

    if(list->size == list->maxsize)
        return 0;

    long long tmp = list->next[elem];
    long long newNode = 0;

    stackPop(list->free, &newNode);

    list->prev[newNode] = elem;
    list->next[newNode] = tmp;
    list->value[newNode] = value;

    list->next[elem] = newNode;

    if(tmp != -1) {
        list->prev[tmp] = newNode;
    }
    else {
        list->tail = newNode;
    }

    list->size++;

    return 1;
}

int insertBefore(list_t *list, long long elem, void *value) {
    assert(list);
    assert(elem >= 0);

    if(list->size == list->maxsize)
        return 0;

    long long tmp = list->prev[elem];
    long long newNode = 0;

    stackPop(list->free, &newNode);

    list->next[newNode] = elem;
    list->prev[newNode] = tmp;
    list->value[newNode] = value;

    list->prev[elem] = newNode;

    if(tmp != -1) {
        list->next[tmp] = newNode;
    }
    else {
        list->head = newNode;
    }

    list->size++;

    return 1;
}

long long getFirstElement(list_t *list) {
    assert(list);

    return list->head;
}

int main() {

}