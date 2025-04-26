#ifndef INC_FIBER_H
#define INC_FIBER_H

#include <stdint.h>

struct __attribute__((packed)) fiber {
  uint64_t rip;
  uint64_t stack;
};

extern uint64_t fiber_get_rip(struct fiber*);
extern uint64_t fiber_get_stack(struct fiber*);

#endif
