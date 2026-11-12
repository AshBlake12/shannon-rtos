#ifndef FAULT_H 
#define FAULT_H

#include <stdint.h>

void fault_init(void);
//called from fault.S with the stacked frame, dumps regs and hangs
void fault_report(uint32_t *frame, uint32_t exc_return);

#endif 
