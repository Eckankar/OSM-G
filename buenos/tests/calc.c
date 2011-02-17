#include "tests/lib.h"

typedef struct list_t {
  void* data;
  struct list_t* next;
} list_t;

typedef struct stack_t {
  struct list_t* top;
} stack_t;

void stack_init(stack_t*);

int stack_empty(stack_t*);

void* stack_top(stack_t*);

void* stack_pop(stack_t*);

int stack_push(stack_t*, void*);

void stack_init(stack_t* stack) {
  stack->top = NULL;
}

int stack_empty(stack_t* stack) {
  return stack->top == NULL;
}

void* stack_top(stack_t* stack) {
  return stack->top->data;
}

void* stack_pop(stack_t* stack) {
  void* v = stack->top->data;
  list_t* newtop = stack->top->next;
  free(stack->top);
  stack->top = newtop;
  return v;
}

int stack_push(stack_t* stack, void* data) {
  list_t* elem;
  if ((elem = (list_t*)malloc(sizeof(list_t))) == NULL) {
    return 1;
  } else {
    elem->data = data;
    elem->next = stack->top;
    stack->top = elem;
    return 0;
  }
}

typedef int (*binop)(int,int);

static void out_of_memory() {
  printf("Out of memory.\n");
  syscall_exit(1);
}

static void perform_binop(stack_t* stack, binop op) {
  int *x;
  int *y;
  int *z;
  if ((z = (int*)malloc(sizeof(int))) == NULL) {
    out_of_memory();
  }
  if (!stack_empty(stack)) {
    x = stack_pop(stack);
    if (!stack_empty(stack)) {
      y = stack_pop(stack);
      *z = (*op)(*x,*y);
      free(x);
      free(y);
      if (stack_push(stack, z) == 1) {
        out_of_memory();
      }
      return;
    } else {
      if (stack_push(stack, x) == 1) {
        out_of_memory();
      }
    }
  }
  free(z);
  printf("Too few elements on stack.\n");
}

static int plus(int x, int y) {
  return x+y;
}

static int sub(int x, int y) {
  return x-y;
}

static int mult(int x, int y) {
  return x*y;
}

static int divn(int x, int y) {
  return x/y;
}

int numeric(char* str)
{
  int i;
  for (i = 0; str[i]; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
  }
  return 1;
}

static const int BUFFER_SIZE = 20;

int main() {
  stack_t stack;
  char buffer[BUFFER_SIZE];
  heap_init();
  stack_init(&stack);
  while (1) {
    readline(buffer, BUFFER_SIZE);
    if (strcmp(buffer, "") == 0) {
      syscall_exit(0);
    } else if (strcmp(buffer, "+") == 0) {
      perform_binop(&stack, &plus);
    } else if (strcmp(buffer, "-") == 0) {
      perform_binop(&stack, &sub);
    } else if (strcmp(buffer, "*") == 0) {
      perform_binop(&stack, &mult);
    } else if (strcmp(buffer, "/") == 0) {
      perform_binop(&stack, &divn);
    } else if (strcmp(buffer, "p") == 0) {
      if (stack_empty(&stack)) {
        printf("Stack is empty.\n");
      } else {
        printf("%d\n", *(int*)stack_top(&stack));
      }
    } else if (numeric(buffer)) {
      int x = atoi(buffer);
      int* d;
      if ((d = (int*)malloc(sizeof(int))) == NULL) {
        out_of_memory();
      } else {
        *d = x;
        if (stack_push(&stack,d) != 0) {
          out_of_memory();
        }
      }
    } else {
      printf("Bad input.\n");
    }
  }
}
