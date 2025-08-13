#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "stm32f1xx.h"

#include "uart.h"
#include "systick.h"
#include "crsf.h"
#include "motors.h"

#include "main.h"

#define RC_ARM_CHANNEL      ch4
#define RC_THROTTLE_CHANNEL ch2
#define RC_STEER_CHANNEL    ch0

static uint8_t rx_buf[64];

void motor_a_move_backward(void){
    GPIOA->ODR &= ~GPIO_ODR_ODR4;
    GPIOA->ODR |=  GPIO_ODR_ODR5;
}
void motor_a_move_forward(void){
    GPIOA->ODR |=  GPIO_ODR_ODR4;
    GPIOA->ODR &= ~GPIO_ODR_ODR5;
}
void motor_b_move_backward(void){
    GPIOA->ODR &= ~GPIO_ODR_ODR6;
    GPIOA->ODR |=  GPIO_ODR_ODR7;
}
void motor_b_move_forward(void){
    GPIOA->ODR |=  GPIO_ODR_ODR6;
    GPIOA->ODR &= ~GPIO_ODR_ODR7;
}

void motors_stop(void){
    GPIOA->ODR &= ~GPIO_ODR_ODR4;
    GPIOA->ODR &= ~GPIO_ODR_ODR5;
    GPIOA->ODR &= ~GPIO_ODR_ODR6;
    GPIOA->ODR &= ~GPIO_ODR_ODR7;
}

static void crsf_on_link_up(void){
    LED_ON();
}

static void crsf_on_link_down(void){
    LED_OFF();
    motors_stop();
}

static void crsf_on_packet_rc_channels(const crsf_channels_t* channels){
    uint16_t rc_arm = (uint16_t)channels->RC_ARM_CHANNEL;

    int throttle = (((int)channels->RC_THROTTLE_CHANNEL - CRSF_CHANNEL_VALUE_MID) * 100) / (CRSF_CHANNEL_VALUE_MAX - CRSF_CHANNEL_VALUE_MID);
    int steer = (((int)channels->RC_STEER_CHANNEL - CRSF_CHANNEL_VALUE_MID) * 100) / (CRSF_CHANNEL_VALUE_MAX - CRSF_CHANNEL_VALUE_MID);
    throttle = (throttle < -100) ? -100 : (throttle > 100) ? 100 : throttle;
    steer = (steer < -100) ? -100 : (steer > 100) ? 100 : steer;

    int motor_a_speed = throttle - steer;
    int motor_b_speed = throttle + steer;

    if(rc_arm <= CRSF_CHANNEL_VALUE_MID){
        motors_stop();
    } else {
        if(motor_a_speed > 0) motor_a_move_forward();
        else motor_a_move_backward();

        if(motor_b_speed > 0) motor_b_move_forward();
        else motor_b_move_backward();
    }

    motor_a_set_speed(abs(motor_a_speed));
    motor_b_set_speed(abs(motor_b_speed));
}


int main(void) 
{
    LED_OFF();
    
    motors_init(FREQ_CPU, 400, 201);

    uart_t uart;
    uart.base = USART2;
    uart.baudrate = CRSF_BAUDRATE;
    uart_init(&uart, 36000000);

    SysTick_Init(FREQ_CPU, NULL);

    crsf_handler_t crsf = crsf_new_handler(&uart, rx_buf);

    crsf.cb_link_up = crsf_on_link_up;
    crsf.cb_link_down = crsf_on_link_down;
    crsf.cb_on_packet_channels = crsf_on_packet_rc_channels;

    while(true)
    {   
        if(!crsf_loop(&crsf))
            continue;
    }

    return 0;
}


void clock_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;

    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CFGR = 0;
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_PLLON);

    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    RCC->CFGR |= RCC_CFGR_PLLSRC; // ??????
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // HCLK = 72 MHz
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 = 36 MHz
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 = 72 MHz

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    RCC->CR &= ~RCC_CR_HSION;
}

void gpio_init(void)
{
    // GPIO for LED PC13
    GPIOC->CRH  |= GPIO_CRH_MODE13_1;
    GPIOC->BSRR |= GPIO_BSRR_BS13;

    // GPIO for Motors PWM PA0 PA1
    GPIOA->CRL  = (GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1);
    GPIOA->CRL |= (GPIO_CRL_MODE0 | GPIO_CRL_MODE1);

    // GPIO for Motor A Direction PA4 PA5
    GPIOA->CRL  |= GPIO_CRL_MODE4_1 | GPIO_CRL_MODE5_1;
    GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BR5;

    // GPIO for Motor B Direction PA6 PA7
    GPIOA->CRL  |= GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1;
    GPIOA->BSRR |= GPIO_BSRR_BR6 | GPIO_BSRR_BR7;
}

void SystemInit(void)
{
    clock_init();
    gpio_init();
}

void HardFault_Handler(void)
{
    static int i = 0;
    while(1)
    {
        LED_ON();
        for(i = 0; i < 500000; i++);
        LED_OFF();
        for(i = 0; i < 500000; i++);
    }
}
