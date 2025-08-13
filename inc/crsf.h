#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "uart.h"
#include "crsf_protocol.h"

#define CRSF_RX_TIMEOUT_MS 100

typedef enum {
    CRSF_STATE_START = 0,
    CRSF_STATE_SIZE,
    CRSF_STATE_TYPE,
    CRSF_STATE_DATA
} crsf_state_t;

typedef struct {
    uart_t *uart;
    uint8_t *buffer;
    crsf_state_t state;
    uint8_t bytes_received;
    size_t last_time_received;
    bool link_is_up;
    void (*cb_link_up)(void);
    void (*cb_link_down)(void);
    void (*cb_on_packet_channels)(const crsf_channels_t*);
} crsf_handler_t;

crsf_handler_t crsf_new_handler(uart_t* uart, uint8_t *buffer);
bool crsf_loop(crsf_handler_t *crsf);
