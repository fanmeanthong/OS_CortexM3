#ifndef __KERNEL_H // 
#define __KERNEL_H

#include <stdint.h>
#include "timebase.h"
#include "uart.h"
// ====== TCB Structure ======
typedef struct tcb {
    uint32_t *stackPt;
    struct tcb *nextPt;
    /*
    More Task Info
    */
} TCB_t;
__attribute__((naked)) void osSchedulerLaunch(void);

uint32_t *osKernelStackInit(int i, void (*task_func)(void));

TCB_t *osKernelCreateTask(void (*task_func)(void));

void osInit(void);

#endif //