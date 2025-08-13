#include "uart.h"

#include <stdlib.h>

#include "main.h"

bool uart_available(const uart_t* uart){
    return (uart->base->SR & USART_SR_RXNE);
}   

uint8_t uart_read_byte(const uart_t* uart){
    return (uint8_t)uart->base->DR;
}

void uart_send_byte(const uart_t* uart, uint8_t byte){
    while(!(uart->base->SR & USART_SR_TXE));
    uart->base->DR = byte;
}

void uart_send(const uart_t* uart, const void* data, size_t sz){
    uint8_t *str = (uint8_t*) data;
    for(size_t i = 0; i < sz; i++)
        uart_send_byte(uart, str[i]);
}

void uart_init(const uart_t* uart, size_t cpu_freq){
    if(uart == NULL)
        return;
    
    if (uart->baudrate == 0)
        return;

    // GPIO for UART
    if(uart->base == USART1){
        GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9;
        GPIOA->CRH |= GPIO_CRH_CNF10_0;
    } else if(uart->base == USART2){
        GPIOA->CRL |= GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2;
        GPIOA->CRL |= GPIO_CRL_CNF3_0;
    } else if(uart->base == USART3){
        GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10;
        GPIOB->CRH |= GPIO_CRH_CNF11_0;
    }

    

    uart->base->BRR = cpu_freq / uart->baudrate;
    uart->base->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}
