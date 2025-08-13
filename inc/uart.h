#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "stm32f1xx.h"


typedef struct {
    USART_TypeDef* base;
    size_t baudrate;
} uart_t;

bool uart_available(const uart_t*);
uint8_t uart_read_byte(const uart_t*);
void uart_send_byte(const uart_t*, uint8_t);
void uart_send(const uart_t*, const void*, size_t);
void uart_init(const uart_t*, size_t);
