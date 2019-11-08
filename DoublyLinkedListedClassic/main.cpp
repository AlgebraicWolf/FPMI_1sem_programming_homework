#include <cstdio>
#include <cstdlib>
#include <cassert>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define UTEST(cond, flag) {\
    if(!(cond)) {\
        flag = false;\
        printf(ANSI_COLOR_RED "Unit testing failed: %s\n" ANSI_COLOR_RESET, #cond); \
    }\
}

enum listValidity{
    OK = 0,
    LIST_NOT_FOUND = 1,
    CORRUPTED = 2
};

struct node_t {
    node_t *next;
    node_t *prev;
    void *value;
};

struct list_t {
    node_t *head;
    node_t *tail;
    size_t size;
};

list_t *createList();

node_t *getElementByPosition(list_t *list, size_t position);

node_t *getFirstElement(list_t *list);

node_t *getLastElement(list_t *list);

node_t *getNextElement(node_t *node);

node_t *getPreviousElement(node_t *node);

node_t *findFirstNode(list_t *list, void *value, bool (*cmp)(void *, void *));

node_t *findLastNode(list_t *list, void *value, bool (*cmp)(void *, void *));

listValidity validateList(list_t *list);

void deleteList(list_t **list);

void addToHead(list_t *list, void *value);

void addToTail(list_t *list, void *value);

void insertAfter(list_t *list, node_t *elem, void *value);

void insertBefore(list_t *list, node_t *elem, void *value);

void deleteNode(list_t *list, node_t *elem);

void clearList(list_t *list);

void dumpList(list_t *list, const char *dumpFilename,  char *(*nodeDump)(node_t *) = nullptr);

char *nodeDump(node_t *node) { // Example function
    static char str[65] = "";
    sprintf(str, "{VALUE|%d}|{NEXT|%p}|{PREVIOUS|%p}", *(int *)(node->value), node->next, node->prev);
    return (char *)str;
}

bool doUnitTesting() {
    bool valid = true;
    list_t *testList = createList();
    UTEST(testList->head == nullptr, valid);
    UTEST(testList->tail == nullptr, valid);
    int vals[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    addToHead(testList, &vals[1]);
    UTEST(testList->head == testList->tail, valid);
    UTEST(testList->size == 1, valid);

    addToTail(testList, &vals[8]);
    UTEST(testList->size == 2, valid);
    UTEST(testList->head != testList->tail, valid);

    insertAfter(testList, testList->tail, &vals[9]);
    UTEST(testList->size == 3, valid);

    insertBefore(testList, testList->head, &vals[0]);
    UTEST(testList->size == 4, valid);

    UTEST((int *)getElementByPosition(testList, 0)->value == &vals[0], valid);
    UTEST(getElementByPosition(testList, 1)->value == &vals[1], valid);
    UTEST(getElementByPosition(testList, 2)->value == &vals[8], valid);
    UTEST(getElementByPosition(testList, 3)->value == &vals[9], valid);

    for(int i = 2; i < 8; i++) {
        insertAfter(testList, getElementByPosition(testList, i - 1), &vals[i]);
        UTEST(getElementByPosition(testList, i)->value == &vals[i], valid);
    }

    UTEST(testList->size == 10, valid);

    node_t *old = testList->head;

    deleteNode(testList, testList->head);
    UTEST(testList->head != old, valid);

    old = testList->tail;

    deleteNode(testList, testList->tail);
    UTEST(testList->tail != old, valid);

    UTEST(validateList(testList) == OK, valid);
    dumpList(testList, "unitTestingDump.dot", nodeDump);
    deleteList(&testList);
    UTEST(!testList, valid);
    return valid;
}

int main() {
    if(doUnitTesting())
        printf(ANSI_COLOR_GREEN "##############################\nUnit testing finished successfully!\n##############################" ANSI_COLOR_RESET);
    else
        printf(ANSI_COLOR_RED "##############################\nUnit testing failed!\n##############################" ANSI_COLOR_RESET);
}

list_t *createList() {
    list_t *newList = (list_t *) calloc(1, sizeof(list_t));
    newList->size = 0;
    newList->head = nullptr;
    newList->tail = nullptr;

    return newList;
}

void clearList(list_t *list) {
    assert(list);

    node_t *curNode = list->head;
    node_t *next = nullptr;

    while(curNode != list->tail) {
        next = curNode->next;
        free(curNode);
        curNode = next;
    }

    list->size = 0;
}

void deleteList(list_t **list) {
    assert(list);
    assert(*list);

    clearList(*list);

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

node_t *findFirstNode(list_t *list, void *value, bool (*cmp)(void *, void *)) {
    assert(list);

    node_t *node = list->head;

    for(size_t i = 0; i < list->size; i++) {
        if(!node)
            return node;

        if(cmp(node->value, value))
            return node;

        node = node->next;
    }

    return nullptr;
}

node_t *findLastNode(list_t *list, void *value, bool (*cmp)(void *, void *)) {
    assert(list);

    node_t *node = list->tail;

    for(size_t i = list->size - 1; i >= 0; i--) {
        if(!node)
            return node;

        if(cmp(node->value, value))
            return node;

        node = node->prev;
    }

    return nullptr;
}

node_t *getFirstElement(list_t *list) {
    assert(list);

    return list->head;
}

node_t *getLastElement(list_t *list) {
    assert(list);

    return list->tail;
}

node_t *getNextElement(node_t *node) {
    assert(node);

    return node->next;
}

node_t *getPreviousElement(node_t *node) {
    assert(node);

    return node->prev;
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

listValidity validateList(list_t *list) {
    if (!list)
        return LIST_NOT_FOUND;

    size_t size = list->size;
    node_t *node = list->head;

    for(size_t i = 0; i < size - 1; i++) {
        if(!node)
            return CORRUPTED;

        node = node->next;
    }

    if(list->tail != node)
        return CORRUPTED;

    return OK;
}

void deleteNode(list_t *list, node_t *elem) {
    assert(list);
    assert(elem);
    assert(list->size > 0);

    if (elem->prev)
        elem->prev->next = elem->next;
    else
        list->head = elem->next;

    if(elem->next)
        elem->next->prev = elem->prev;
    else
        list->tail = elem->prev;

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
    fprintf(dumpFile, "node%p -> Tail;\n", list->tail);
    fprintf(dumpFile, "}");
    fclose(dumpFile);
}