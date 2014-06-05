#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct stack_st {
    size_t top;
    size_t size;
    void *base[0];
}STACK;

static inline STACK* NEW_STACK(size_t size) {
    STACK *s = NULL;
    assert(size);
    s = (STACK *)calloc(1, sizeof(STACK) + sizeof(void *) * size);
    assert(s);
    s->size = size;
    return s;
}

#define STACK_FULL(stack) ((stack)->top >= ((stack)->size - 1))
#define STACK_EMPTY(stack) ((stack)->top == 0)

#define STACK_PUSH(stack, data) (STACK_FULL(stack) ? NULL : ((stack)->base[((stack)->top)++] = (data)))
#define STACK_POP(stack) (STACK_EMPTY(stack) ? NULL : ((stack)->base[--((stack)->top)]))

static inline void DESTROY_STACK(STACK *s) {
    void *t = NULL;
    assert(s);
    while((t = STACK_POP(s)) != NULL) free(t);
    free(s);
    return;
}

#endif/*stack.h*/
