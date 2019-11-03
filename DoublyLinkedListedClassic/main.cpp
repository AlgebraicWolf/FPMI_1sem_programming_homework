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

list_t deleteList(list_t **list) {
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