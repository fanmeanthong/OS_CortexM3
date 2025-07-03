#include <stdint.h>
// ====== TCB Structure ======
typedef struct tcb {
    uint32_t *stackPt;
    /*
    More Task Info
    */
} TCB_t;
__attribute__((naked)) void osSchedulerLaunch(void);
uint32_t *osKernelStackInit(int i, void (*task_func)(void));
TCB_t *osKernelCreateTask(void (*task_func)(void));