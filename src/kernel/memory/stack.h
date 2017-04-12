#pragma once

#include <stdint.h>

struct stack_struct {
  void * stack_base;
  size_t stack_size;
};

struct stack_struct* stack_alloc(struct stack_struct *ss, size_t size, int flags);
struct stack_struct  boot_stack(size_t size, int flags);

int   stack_check(const struct stack_struct * s);
int   stack_remaining(const struct stack_struct * s);
void  stack_free(struct stack_struct * s);
void* stack_top(struct stack_struct * s);

