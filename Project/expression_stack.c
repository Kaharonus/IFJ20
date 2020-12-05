//
// Created by tomas on 01.12.20.
//

#include "expression_stack.h"

expression_stack *expression_stack_init() {
    expression_stack* ptr = malloc(sizeof(expression_stack));
    ptr->index = 0;
    return ptr;
}

bool expression_stack_empty(expression_stack *stack) {
    return stack->index == 0;
}

expression_stack_data expression_stack_pop(expression_stack *stack, bool* err) {
    if(!expression_stack_empty(stack)){
        stack->index--;
        *err = false;
        return stack->data[stack->index];
    }
    *err = true;
    expression_stack_data ret;
    return ret;
}

void expression_stack_push(expression_stack *stack, expression_stack_data data) {
    stack->data[stack->index] = data;
    stack->index++;

}

expression_stack_data expression_stack_top(expression_stack *ss) {
    return ss->data[ss->index - 1];
}
