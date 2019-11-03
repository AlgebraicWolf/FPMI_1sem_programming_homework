#include <cstdio>

struct node_t {
    node_t *next;
    node_t *prev;
    void *value; // reconsider
};

struct list_t {
    node_t *head;
    node_t *tail;
    size_t elems;
};

int main() {

    return 0;
}