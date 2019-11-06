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

list_t *createList();

void deleteList(list_t **list);

void addToHead(list_t *list, void *value);

void addToTail(list_t *list, void *value);

void insertAfter(list_t *list, node_t *elem, void *value);

void insertBefore(list_t *list, node_t *elem, void *value);

void deleteNode(list_t *list, node_t *elem);

void dumpList(list_t *list, const char *dumpFilename,  char *(*nodeDump)(node_t *) = nullptr);

char *nodeDump(node_t *node) { // Example function
    static char str[65] = "";
    sprintf(str, "{VALUE|%d}|{NEXT|%p}|{PREVIOUS|%p}", *(int *)(node->value), node->next, node->prev);
    return (char *)str;
}

int main() {
    list_t *lst = createList();
    int a = 10;
    int b = 20;
    int c = 30;
    int d = 40;
    int e = 50;
    addToHead(lst, &a);
    addToTail(lst, &e);
    insertAfter(lst, lst->head, &b);
    insertAfter(lst, lst->head->next, &b);
    insertBefore(lst, lst->tail->prev, &d);
    insertBefore(lst, lst->tail->prev->prev, &c);

    dumpList(lst, "dump.dot", nodeDump);
    deleteList(&lst);
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

    node_t *temp = list->head;
    node_t *newNode = (node_t *) calloc(1, sizeof(node_t));
    newNode->value = value;
    newNode->next = temp;

    if (temp) {
        temp->prev = newNode;
    }

    list->head = newNode;

    if(!list->tail) {
        list->tail = newNode;
    }

    list->size++;
}

void addToTail(list_t *list, void *value) {
    assert(list);

    node_t *temp = list->tail;
    node_t *newNode = (node_t *) calloc(1, sizeof(node_t));
    newNode->value = value;
    newNode->prev = temp;

    if (temp) {
        temp->next = newNode;
    }

    list->tail = newNode;

    if(!list->head) {
        list->head = newNode;
    }

    list->size++;
}

void insertAfter(list_t *list, node_t *elem, void *value) {
    assert(list);
    assert(elem);

    node_t *tmp = elem->next;
    node_t *newNode = (node_t *) calloc(1, sizeof(node_t));

    newNode->prev = elem;
    newNode->next = tmp;
    newNode->value = value;

    elem->next = newNode;
    if(tmp) {
        tmp->prev = newNode;
    }
    else {
        list->tail = newNode;
    }

    list->size++;
}

void insertBefore(list_t *list, node_t *elem, void *value) {
    assert(list);
    assert(elem);

    node_t *tmp = elem->prev;
    node_t *newNode = (node_t *) calloc(1, sizeof(node_t));

    newNode->prev = tmp;
    newNode->next = elem;
    newNode->value = value;

    elem->prev = newNode;
    if(tmp) {
        tmp->next = newNode;
    }
    else {
        list->head = newNode;
    }

    list->size++;
}

node_t *getElementByPosition(list_t *list, size_t position) {
    assert(list);

    if(position >= list->size)
        return nullptr;

    node_t *curNode = list->head;

    for(int i = 0; i < position; i++) {
        if (!curNode)
            return nullptr;
        curNode = curNode->next;
    }
    
    return curNode;
}

void deleteNode(list_t *list, node_t *elem) {
    assert(elem);
    assert(list);
    assert(list->size > 0);

    if (elem->prev)
        elem->prev->next = elem->next;
    else
        list->head = elem->next;

    if(elem->next)
        elem->next->prev = elem->prev;
    else
        list->tail = list->head;

    (list->size)--;
    free(elem);
}

void dumpList(list_t *list, const char *dumpFilename, char *(*nodeDump)(node_t *)) {
    assert(list);
    assert(dumpFilename);

    FILE *dumpFile = fopen(dumpFilename, "w");
    fprintf(dumpFile, "digraph {\n");

    node_t *node = list->head;

    fprintf(dumpFile, "node%p[label=\"{{%p}", node, node);
    if(nodeDump) {
        fprintf(dumpFile, "|{%s}", (*nodeDump)(node));
    }
    fprintf(dumpFile, "}\",shape=record];\n", node, node);

    while(node != list->tail) {
        fprintf(dumpFile, "node%p[label=\"{{%p}", node->next, node->next);
        if(nodeDump) {
            fprintf(dumpFile, "|{%s}", (*nodeDump)(node->next));
        }
        fprintf(dumpFile, "}\",shape=record];\n", node->next, node->next);

        fprintf(dumpFile, "node%p -> node%p;\n", node, node->next);
        fprintf(dumpFile, "node%p -> node%p;\n", node->next, node);
        node = node->next;
    }

    fprintf(dumpFile, "Head -> node%p;\n", list->head);
    fprintf(dumpFile, "Tail -> node%p;\n", list->tail);
    fprintf(dumpFile, "}");
    fclose(dumpFile);
}