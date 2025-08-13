#pragma once

#include <stddef.h>
#include <stdint.h>

#include "stm32f1xx.h"

typedef struct {
    void (*move)(int,int);
} motors_t;


motors_t motors_setup(size_t cpu_freq, size_t pwm_period);

void motors_init(size_t cpu_freq, size_t pwm_period, uint16_t range);

void motor_a_set_speed(uint16_t speed);
void motor_b_set_speed(uint16_t speed);
