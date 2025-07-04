#include <stdint.h>
#include "stm32f10x.h"
#include "kernel.h"
#include "timebase.h"
#include "uart.h"

#define FLASH_BASE           ((uint32_t)0x40022000)
#define FLASH_ACR            (*(volatile uint32_t *)(FLASH_BASE + 0x00))

#define RCC_CR               (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR             (*(volatile uint32_t *)(RCC_BASE + 0x04))

#define FLASH_ACR_PRFTBE     (1 << 4)
#define FLASH_ACR_LATENCY_2  (2 << 0)
#define FLASH_ACR_LATENCY    (0x7 << 0)

#define RCC_CR_HSEON         (1 << 16)
#define RCC_CR_HSERDY        (1 << 17)
#define RCC_CR_PLLON         (1 << 24)
#define RCC_CR_PLLRDY        (1 << 25)

#define RCC_CFGR_SW          (0x3 << 0)
#define RCC_CFGR_SW_PLL      (0x2 << 0)
#define RCC_CFGR_SWS         (0x3 << 2)
#define RCC_CFGR_SWS_PLL     (0x2 << 2)

#define RCC_CFGR_HPRE        (0xF << 4)
#define RCC_CFGR_PPRE1       (0x7 << 8)
#define RCC_CFGR_PPRE2       (0x7 << 11)

#define RCC_CFGR_PPRE1_DIV2  (0x4 << 8)

#define RCC_CFGR_PLLSRC      (1 << 16)
#define RCC_CFGR_PLLMULL     (0xF << 18)
#define RCC_CFGR_PLLMULL9    (0x7 << 18)

#define GPIO_CRH(GPIO_BASE)  (*(volatile uint32_t *)((GPIO_BASE) + GPIO_CRH_OFFSET))
#define GPIO_ODR(GPIO_BASE)  (*(volatile uint32_t *)((GPIO_BASE) + GPIO_ODR_OFFSET))


uint32_t cnt;
extern TCB_t *CurrentPt ;
void task0(void);

void SystemClock_Config(void)
{
    // Enable HSE
    RCC_CR |= RCC_CR_HSEON;
    while (!(RCC_CR & RCC_CR_HSERDY));

    // Prefetch + 2 WS for 72MHz
    FLASH_ACR |= FLASH_ACR_PRFTBE;
    FLASH_ACR &= ~FLASH_ACR_LATENCY;
    FLASH_ACR |= FLASH_ACR_LATENCY_2;

    // PLL config: HSE source, x9
    RCC_CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC_CFGR |= (RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

    // Prescaler
    RCC_CFGR &= ~RCC_CFGR_HPRE;
    RCC_CFGR &= ~RCC_CFGR_PPRE1;
    RCC_CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC_CFGR &= ~RCC_CFGR_PPRE2;

    // Enable PLL
    RCC_CR |= RCC_CR_PLLON;
    while (!(RCC_CR & RCC_CR_PLLRDY));

    // Switch SYSCLK to PLL
    RCC_CFGR &= ~RCC_CFGR_SW;
    RCC_CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC_CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void task0(void)
{
    RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;

    // PC13: Output push-pull, 2 MHz
    GPIO_CRH(GPIOC_BASE) &= ~(0xF << 20);
    GPIO_CRH(GPIOC_BASE) |=  (0x2 << 20);

    UART1_SendString("Enter Task0! \r\n");
    while (1)
    {
        GPIO_ODR(GPIOC_BASE) ^= (1 << 13);
        delay(2500);
        UART1_SendString("Blinking Task 0! \r\n");
    }
}

void task1(void){
    while(1){
        UART1_SendString("Blinking Task 1! \r\n");
        cnt++;
        delay(2500);
    }
}

// ====== Scheduler Launch Function ======
             
int main(void)
{
    SystemClock_Config();
    UART1_Init();
   
    osKernelCreateTask(task0);
    osKernelCreateTask(task1);
    UART1_SendString("Entering OS !\r\n");
    osInit();
    while (1);  
}


