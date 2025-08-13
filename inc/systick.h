#pragma once

#include <stddef.h>

void SysTick_Init(size_t cpu_freq, void (*callback)(size_t));

size_t millis(void);
void delay_ms(size_t ms);