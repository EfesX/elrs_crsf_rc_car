#include "systick.h"

#include "stm32f1xx.h"

static volatile size_t ms_ticks = 0;
void (*systick_cb)(size_t) = NULL;

void SysTick_Handler(void){
    ms_ticks++;
    if(systick_cb)
        systick_cb(ms_ticks);
}

void SysTick_Init(size_t cpu_freq, void (*callback)(size_t)){
    SysTick->LOAD = (cpu_freq / 1000) - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    systick_cb = callback;
}

size_t millis(void){
    return ms_ticks;
}

void delay_ms(size_t ms){
    size_t start = ms_ticks;
    while((ms_ticks - start) < ms);
}