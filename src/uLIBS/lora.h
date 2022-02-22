/* 
 * File:   lora.h
 * Author: pablo
 *
 * Created on 15 de febrero de 2022, 04:05 PM
 */

#ifndef LORA_H
#define	LORA_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "xc.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "frtos-io.h"
#include "xprintf.h"
#include "linearBuffer.h"
    
    
#define LORA_RTS_PORT	PORTE
#define LORA_RTS_PIN_bm	PIN4_bm
#define LORA_RTS_PIN_bp	PIN4_bp

#define LORA_CTS_PORT	PORTE
#define LORA_CTS_PIN_bm	PIN3_bm
#define LORA_CTS_PIN_bp	PIN3_bp
    
#define LORA_RESET_PORT     PORTE
#define LORA_RESET_PIN_bm	PIN2_bm
#define LORA_RESET_PIN_bp	PIN2_bp
   
// RTS es input
#define LORA_RTS_CONFIG()    ( LORA_RTS_PORT.DIR &= ~LORA_RTS_PIN_bm )
uint8_t lora_read_rts(void);

// CTS, RESET son outputs
#define LORA_CTS_CONFIG()    ( LORA_CTS_PORT.DIR |= LORA_CTS_PIN_bm )
#define SET_LORA_CTS()       ( LORA_CTS_PORT.OUT |= LORA_CTS_PIN_bm )
#define CLEAR_LORA_CTS()     ( LORA_CTS_PORT.OUT &= ~LORA_CTS_PIN_bm )
#define TOGGLE_LOTA_CTS()    ( LORA_CTS_PORT.OUT ^= 1UL << LORA_CTS_PIN_bp);
    
#define LORA_RESET_CONFIG()    ( LORA_RESET_PORT.DIR |= LORA_RESET_PIN_bm )
#define SET_LORA_RESET()       ( LORA_RESET_PORT.OUT |= LORA_RESET_PIN_bm )
#define CLEAR_LORA_RESET()     ( LORA_RESET_PORT.OUT &= ~LORA_RESET_PIN_bm )
#define TOGGLE_LOTA_RESET()    ( LORA_RESET_PORT.OUT ^= 1UL << LORA_RESET_PIN_bp)


#define lora_reset_on()     SET_LORA_RESET()
#define lora_reset_off()    CLEAR_LORA_RESET()


#define LORA_RX_BUFFER_SIZE 64
//char lora_rx_buffer[LORA_RX_BUFFER_SIZE];
//lBuffer_s lora_rx_sdata;

struct {
    uint8_t ptr;
    char rx_buffer[LORA_RX_BUFFER_SIZE];
} lora_rx_data;


void LORA_init(void);
void lora_flush_RxBuffer(void);
void lora_flash_led(void);
void lora_print_RxBuffer(void);
void lora_push_RxBuffer( char *c);
void lora_print_RxBuffer_stats(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LORA_H */

