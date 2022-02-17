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

void LORA_init(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LORA_H */

