#include "motors.h"

#include "stm32f1xx.h"

#define PWM_RANGE 201

void motors_init(size_t cpu_freq, size_t pwm_period, uint16_t range){
    TIM2->PSC = (cpu_freq / (range * pwm_period)) - 1;
    TIM2->ARR = range;

    TIM2->CCR1 = 0;
    TIM2->CCR2 = 0;

    TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
    TIM2->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;

    TIM2->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;

    TIM2->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
    TIM2->CR1  |= TIM_CR1_ARPE;
    TIM2->CR1  |= TIM_CR1_CEN;
}

void motor_a_set_speed(uint16_t speed){
    if(speed > TIM2->ARR)
        speed = TIM2->ARR - 1;
    TIM2->CCR1 = speed;
}

void motor_b_set_speed(uint16_t speed){
    if(speed > TIM2->ARR)
        speed = TIM2->ARR - 1;
    TIM2->CCR2 = speed;
}
