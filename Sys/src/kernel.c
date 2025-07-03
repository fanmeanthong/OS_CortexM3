#include "kernel.h"
// ====== Constants ======
#define NUM_OF_THREADS 1
#define STACK_SIZE     128
#define INITIAL_XPSR   0x01000000

TCB_t tcbs[NUM_OF_THREADS];
TCB_t *CurrentPt ;

// ====== Stack Memory ======
uint32_t TCB_STACK[NUM_OF_THREADS][STACK_SIZE];
// ====== Stack Init Function ======
uint32_t *osKernelStackInit(int i, void (*task_func)(void))
{
    tcbs[i].stackPt = &TCB_STACK[i][STACK_SIZE - 16]; // Point to R11 position

    // Stack frame for Cortex-M3 (xPSR, PC, LR, R12, R3, R2, R1, R0)
    TCB_STACK[i][STACK_SIZE - 1]  = INITIAL_XPSR;               // xPSR
    TCB_STACK[i][STACK_SIZE - 2]  = ((uint32_t)task_func) | 1;  // PC (ensure Thumb bit set)
    TCB_STACK[i][STACK_SIZE - 3]  = 0xFFFFFFFD;                 // LR (Exit to Thread mode using PSP)
    TCB_STACK[i][STACK_SIZE - 4]  = 0x12121212;                 // R12
    TCB_STACK[i][STACK_SIZE - 5]  = 0x03030303;                 // R3
    TCB_STACK[i][STACK_SIZE - 6]  = 0x02020202;                 // R2
    TCB_STACK[i][STACK_SIZE - 7]  = 0x01010101;                 // R1
    TCB_STACK[i][STACK_SIZE - 8]  = 0x00000000;                 // R0

    // Manually push R4–R11
    TCB_STACK[i][STACK_SIZE - 9]  = 0x11111111;                 // R11
    TCB_STACK[i][STACK_SIZE - 10] = 0x10101010;                 // R10
    TCB_STACK[i][STACK_SIZE - 11] = 0x09090909;                 // R9
    TCB_STACK[i][STACK_SIZE - 12] = 0x08080808;                 // R8
    TCB_STACK[i][STACK_SIZE - 13] = 0x07070707;                 // R7
    TCB_STACK[i][STACK_SIZE - 14] = 0x06060606;                 // R6
    TCB_STACK[i][STACK_SIZE - 15] = 0x05050505;                 // R5
    TCB_STACK[i][STACK_SIZE - 16] = 0x04040404;                 // R4

    return tcbs[i].stackPt;
}

static uint8_t taskCount = 0;

TCB_t *osKernelCreateTask(void (*task_func)(void))
{
    if (taskCount >= NUM_OF_THREADS)
        return 0;

    // Khởi tạo stack cho task mới
    uint32_t *sp = osKernelStackInit(taskCount, task_func);

    // Thiết lập TCB
    tcbs[taskCount].stackPt = sp;

    taskCount++;
    CurrentPt = &tcbs[0];
    return &tcbs[taskCount - 1];
}
