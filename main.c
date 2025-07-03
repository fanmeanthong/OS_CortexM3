#include <stdint.h>
#include "stm32f10x.h"
#include "kernel.h"
#include "timebase.h"
#include "uart.h"
uint32_t cnt;
extern TCB_t *CurrentPt ;
void task0(void);
void SystemClock_Config(void)
{
    // HSE (High Speed External) - 8 MHz 
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));  // 

    // prefetch buffer & set wait state = 2 (for 72 MHz)
    FLASH->ACR |= FLASH_ACR_PRFTBE;          // Prefetch enable
    FLASH->ACR &= ~FLASH_ACR_LATENCY;        // Clear latency bits
    FLASH->ACR |= FLASH_ACR_LATENCY_2;       // 2 wait states

    // 3. Config PLL:
    // PLL source = HSE, PLL = x9 --> 8MHz * 9 = 72MHz
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL); // Clear config
    RCC->CFGR |= (RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9); // HSE source, MUL = 9

    // 4. Config Prescaler:
    RCC->CFGR &= ~RCC_CFGR_HPRE;            // AHB = SYSCLK / 1
    RCC->CFGR &= ~RCC_CFGR_PPRE1;           // APB1 = HCLK / 1 
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;       // APB1 = HCLK / 2 (max 36 MHz)
    RCC->CFGR &= ~RCC_CFGR_PPRE2;           // APB2 = HCLK / 1

    // 5. Activate PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));     // 

    // 6. Chuyển SYSCLK sang PLL
    RCC->CFGR &= ~RCC_CFGR_SW;              // Clear bits
    RCC->CFGR |= RCC_CFGR_SW_PLL;           // PLL = SYSCLK

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // 
}
// ====== Dummy Task ======
void task0(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // PC13 as Output Push-Pull, max speed 2 MHz
    GPIOC->CRH &= ~(0xF << 20);       // Clear 4 bits
    GPIOC->CRH |=  (0x2 << 20);       // Output mode 2 MHz, Push-pull (00: GP output, 10: 2MHz)
    UART1_SendString("Enter Task0! \r\n");
    while (1)
    {
        // Reverse PC13
        GPIOC->ODR ^= (1 << 13);

        // Delay 500ms
        delay(500);
        UART1_SendString("Blinking! \r\n");
    }
}


// ====== Scheduler Launch Function ======
             
int main(void)
{
    SystemClock_Config();
    Sys_Init();
    UART1_Init();
   
    osKernelCreateTask(task0);
    log_stack(CurrentPt->stackPt);
    osSchedulerLaunch();

    while (1); // 
}


