/* 
 * File:   led.h
 * Author: pablo
 *
 * Created on 8 de febrero de 2022, 11:33 AM
 */

#ifndef LED_H
#define	LED_H

#ifdef	__cplusplus
extern "C" {
#endif


#include <xc.h> 
#include "FreeRTOS.h"
#include "task.h"
    
#define LED_PORT	PORTB
#define LED_PIN_bm	PIN4_bm

#define PRENDER_LED() ( LED_PORT.OUT |= LED_PIN_bm )
#define APAGAR_LED() ( LED_PORT.OUT &= ~LED_PIN_bm )

void LED_init(void);
void led_flash(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */

