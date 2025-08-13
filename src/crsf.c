#include <stdlib.h>

#include "crsf.h"
#include "systick.h"

#include "main.h"

static void crsf_start_loop(crsf_handler_t* crsf){
    if(!uart_available(crsf->uart))
        return;

    uint8_t byte = uart_read_byte(crsf->uart);
    crsf->last_time_received = millis();
  
    if(byte != CRSF_ADDRESS_FLIGHT_CONTROLLER)
        return;

    crsf->buffer[crsf->bytes_received++] = byte;
    crsf->state = CRSF_STATE_SIZE;
}

static void crsf_size_loop(crsf_handler_t* crsf){
    if(!uart_available(crsf->uart))
        return;

    uint8_t byte = uart_read_byte(crsf->uart);
    crsf->last_time_received = millis();

    if(byte > 60){
        crsf->state = CRSF_STATE_START;
        crsf->bytes_received = 0;
        return;
    }

    crsf->buffer[crsf->bytes_received++] = byte;
    crsf->state = CRSF_STATE_TYPE;
}

static void crsf_type_loop(crsf_handler_t* crsf){
    if(!uart_available(crsf->uart))
        return;

    uint8_t byte = uart_read_byte(crsf->uart);
    crsf->last_time_received = millis();

    if(byte != CRSF_FRAMETYPE_RC_CHANNELS_PACKED){
        crsf->state = CRSF_STATE_START;
        crsf->bytes_received = 0;
        return;
    }

    crsf->buffer[crsf->bytes_received++] = byte;
    crsf->state = CRSF_STATE_DATA;
}

static bool crsf_data_loop(crsf_handler_t* crsf){
    if(!uart_available(crsf->uart))
        return false;

    uint8_t byte = uart_read_byte(crsf->uart);
    crsf->last_time_received = millis();

    crsf->buffer[crsf->bytes_received++] = byte;
    
    crsf_header_t *header = (crsf_header_t*)crsf->buffer;

    if(crsf->bytes_received - 3 == header->frame_size){
        crsf->state = CRSF_STATE_START;
        crsf->bytes_received = 0;
        return true;
    }

    return false;
}


bool crsf_loop(crsf_handler_t* crsf){
    if(crsf == NULL)
        return false;

    if(crsf->uart == NULL)
        return false;

    bool result = false;

    switch (crsf->state)
    {
    case CRSF_STATE_START:
        crsf_start_loop(crsf);
        break;

    case CRSF_STATE_SIZE:
        crsf_size_loop(crsf);
        break;

    case CRSF_STATE_TYPE:
        crsf_type_loop(crsf);
        break;

    case CRSF_STATE_DATA:
        result = crsf_data_loop(crsf);
        if(!crsf->link_is_up){
            crsf->link_is_up = true;
            if(crsf->cb_link_up)
                crsf->cb_link_up();
        }
        break;
    
    default:
        break;
    }

    if(crsf->link_is_up && millis() - crsf->last_time_received > CRSF_RX_TIMEOUT_MS){
        crsf->state = CRSF_STATE_START;
        crsf->bytes_received = 0;
        crsf->link_is_up = false;
        crsf->cb_link_down();
    }

    const crsf_header_t *header = (crsf_header_t*)crsf->buffer;
    const crsf_channels_t *channels = (crsf_channels_t*)header->data;

    if(result && crsf->link_is_up && header->type == CRSF_FRAMETYPE_RC_CHANNELS_PACKED && crsf->cb_on_packet_channels)
        crsf->cb_on_packet_channels(channels);

    return result;
}

crsf_handler_t crsf_new_handler(uart_t* uart, uint8_t *buffer){
    crsf_handler_t crsf_handler;

    crsf_handler.uart = uart;
    crsf_handler.state = CRSF_STATE_START;
    crsf_handler.bytes_received = 0;
    crsf_handler.link_is_up = false;
    crsf_handler.buffer = buffer;

    return crsf_handler;
    
}
