#pragma once

struct cpu_state_struct {};

static inline int cpu_state_build(struct cpu_state_struct * cpuss,
		    void *(*start)(void*),
		    void *args,
		    void *stack,
		    void  (*end)())
{
  return 0;
}
