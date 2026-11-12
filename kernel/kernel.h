#ifndef KERNEL_H
#define KERNEL_H

#include "task.h"

void kernel_start(void); //starts tick + first task, dosent return
void kernel_switch(void); //called from pendsv to pick next tcb

#endif 
