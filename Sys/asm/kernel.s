.syntax unified
.cpu cortex-m3
.thumb

.global PendSV_Handler
.global osSchedulerLaunch
.extern CurrentPt

.section .text
.type osSchedulerLaunch, %function

osSchedulerLaunch:
    // Load PSP from CurrentPt->stackPt
    LDR     R0, =CurrentPt
    LDR     R2, [R0]          // R2 = CurrentPt
    LDR     R0, [R2]          // R0 = CurrentPt->stackPt
    MSR     PSP, R0           // Load StackPt into PSP

    // Switch to PSP in Thread mode
    MOV     R0, #2
    MSR     CONTROL, R0
    ISB

    // Restore registers
    POP     {R4-R11}
    POP     {R0-R3}
    POP     {R12}
    POP     {LR}
    POP     {PC}              // xPSR auto-restored

.size osSchedulerLaunch, . - osSchedulerLaunch


// ====== PendSV: Context Switch ======
.type PendSV_Handler, %function
PendSV_Handler:
    // Save current task context
    MRS     R0, PSP
    STMDB   R0!, {R4-R11}         // Push R4–R11 onto PSP

    LDR     R1, =CurrentPt
    LDR     R2, [R1]              // R2 = CurrentPt
    STR     R0, [R2]              // Save PSP to CurrentPt->stackPt

    // Switch to next task
    LDR     R2, [R2, #4]          // R2 = CurrentPt->nextPt
    STR     R2, [R1]              // CurrentPt = CurrentPt->nextPt

    LDR     R0, [R2]              // R0 = CurrentPt->stackPt
    LDMIA   R0!, {R4-R11}         // Restore R4–R11
    MSR     PSP, R0

    BX      LR

.size PendSV_Handler, . - PendSV_Handler
