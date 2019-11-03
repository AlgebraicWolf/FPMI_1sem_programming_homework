#include <cstdio>
#include <cstdlib>
#include <cassert>

struct node_t {
    node_t *next;
    node_t *prev;
    void *value; // reconsider
};

struct list_t {
    node_t *head;
    node_t *tail;
    size_t size;
};

int main() {

    return 0;
}

list_t *createList() {
    list_t *newList = (list_t *) calloc(1, sizeof(list_t));
    newList->size = 0;
    newList->head = nullptr;
    newList->tail = nullptr;

    return newList;
}

void deleteList(list_t **list) {
    assert(list);
    assert(*list);
    node_t *curNode = nullptr;
    node_t *next = nullptr;

    curNode = (*list)->head;

    while(curNode != (*list)->tail) {
        next = curNode->next;
        free(curNode);
        curNode = next;
    }

    free(*list);
    *list = nullptr;
}

void addToHead(list_t *list, void *value) {
    assert(list);
    node_t *prev = list->head;
    node_t *newNode = (node_t *) calloc(1, sizeof(node_t));
    newNode->value = value;
    newNode->next = prev;

    if (prev) {
        prev->prev = newNode;
    }

    list->head = newNode;

    if(!list->tail) {
        list->tail = newNode;
    }

    list->size++;
}

void addToTail(list_t *list, void *value) {
    assert(list);
    node_t *prev = list->tail;
    node_t *newNode = (node_t *) calloc(1, sizeof(node_t));
    newNode->value = value;
    newNode->prev = prev;

    if (prev) {
        prev->next = newNode;
    }

    list->tail = newNode;

    if(!list->head) {
        list->head = newNode;
    }

    list->size++;
}