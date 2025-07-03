#include "timebase.h"

#define MAX_DELAY 0xFFFFFFFF
__IO uint32_t g_curr_tick;
__IO uint32_t g_curr_tick_p;
__IO uint32_t tick_freq =1;
void sram_test_write(void)
{
    volatile uint32_t *p = (uint32_t *)0x20003ff0;

    // Ghi dữ liệu mẫu
    p[0] = 0xDEADBEEF;
    p[1] = 0xCAFEBABE;
    p[2] = 0x12345678;
    p[3] = 0x87654321;

    // Đọc lại để đảm bảo compiler không bỏ qua
    volatile uint32_t r0 = p[0];
    volatile uint32_t r1 = p[1];
    volatile uint32_t r2 = p[2];
    volatile uint32_t r3 = p[3];

    // Đặt breakpoint tại đây để kiểm tra các giá trị
    (void)r0; (void)r1; (void)r2; (void)r3;
}
void Sys_Init(void){
/*************Config for SYSTICK in SysTick Control Reg******************
	*Bit[0]-ENABLE: the Counter Enable Bit, set to 1 to Enable the counter. 
	*Bit[1]-TICKINT: Enables SysTick exception request:
						0 = counting down to zero does not assert the SysTick exception request
						1 = counting down to zero asserts the SysTick exception request.
	*Bit[2]-CLKSOURCE: Indicates the clock source:
						0 = external clock
						1 - processor clock
	*Bit[16]-COUNTFLAG: Returns 1 if timer counted to 0 since last time this was read.
	*/
	
/*************Config SysTick Reload value in SysTick Reload Value Reg******************
	*24 bits[0-23] hold the Reload Value:
	-f(sys) = number of clocks per second.
	-Reload Value is the number of clock to count after reload.
	*/

/*************Steps to reload SysTick:*/
	/*1/Reload timer with number of cycles per second.*/
	//F(sys) = 72000000Hz, RealoadValue = 72000000 => Reload After 1 second.
	SysTick->LOAD = 72000-1;
	/*2/Clear Current Value Reg.*/
	SysTick->VAL = 0;
	/*3/Select Internal Clock Source.*/
	SysTick->CTRL |= 1<<2;
	/*4/Enable Interupt.*/
	SysTick->CTRL |= 1<<1;
	/*5/Enable Systick.*/
	SysTick->CTRL |= 1<<0;
	/*6/Enable Global Interupt.*/
	__enable_irq();
}

void Tick_Increment(void){
	g_curr_tick += tick_freq;
    return ;
}

uint32_t get_tick(void){
	__disable_irq();
	g_curr_tick_p = g_curr_tick;
	__enable_irq();
	return g_curr_tick_p;
}


void delay(uint32_t delay){
	uint32_t tick_start = get_tick();
	uint32_t wait = delay;
	if(wait <MAX_DELAY){
		wait += (uint32_t)(tick_freq);
	}
	
	while((get_tick()- tick_start) < wait){}
}	
void SysTick_Handler(){
    g_curr_tick += tick_freq; // Break Point Ở đây
}