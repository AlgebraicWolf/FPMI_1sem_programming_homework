#include <stdio.h>
#include <assert.h>

typedef int elem_t;

const size_t DEFAULT_INIT_SIZE = 1;

const int SIZE_MULTIPLIER = 2;

struct stack_t {
  elem_t *data;
  size_t size;
  size_t maxsize;
  int errcode;
};

int stackConstruct(stack_t *stack, size_t size = DEFAULT_INIT_SIZE);

int stackPush(stack_t *stack, elem_t element);

int stackPop(stack_t *stack, elem_t *destination);

int stackExtend(stack_t *stack);

int main() {
  stack_t stack = {};
  stackConstruct(&stack);
  elem_t el = {};
  stackPush(&stack, 1);
  stackExtend(&stack);
  stackPush(&stack, 1);
  stackExtend(&stack);
  stackPush(&stack, 1);
  stackPush(&stack, 1);
  stackPush(&stack, 1);
  stackPush(&stack, 1);
  stackPush(&stack, 1);
  stackPush(&stack, 1);
  stackPush(&stack, 1);
  stackExtend(&stack);
//  for (int i = 1; i <= 100; i++) stackPush(&stack, 1);
//  for (int i = 100; i >= 1; i--) {
//    stackPop(&stack, &el);
//    if (el != i) printf("el=%d, i=%d\n", el, i);
//  }
}

int stackConstruct(stack_t *stack, size_t size) {
  assert(stack);
  assert(size > 0);

  stack->maxsize = size;
  stack->data = (elem_t *) calloc(size, sizeof(elem_t));

  if (stack->data == nullptr) return 0;
  return 1;
}

int stackPush(stack_t *stack, elem_t element) {
  assert(stack);

  if ((stack->size) >= (stack->maxsize)) {
    if (!stackExtend(stack)) return 0;
  }

  stack->data[stack->size++] = element;

  return 1;
}

int stackPop(stack_t *stack, elem_t *destination) {
  assert(stack);
  assert(destination);

  if (stack->size == 0) return 0;
  *destination = stack->data[--stack->size];

  return 1;
}

int stackExtend(stack_t *stack) {
  assert(stack);

  extern const int SIZE_MULTIPLIER;

  int newSize = stack->maxsize * SIZE_MULTIPLIER;
  elem_t *newPointer = (elem_t *) realloc(stack->data, newSize);

  if (!newPointer) return 0;
  else {
    stack->data = newPointer;
    stack->maxsize = newSize;

    return 1;
  }
}