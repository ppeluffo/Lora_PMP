/* 
 * File:   pines.h
 * Author: pablo
 *
 * Created on 11 de febrero de 2022, 06:02 PM
 */

#ifndef PINES_H
#define	PINES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>

//--------------------------------------------------------------------------
// Pines generadores de senal AC para el transformador
    
#define AC0_PORT	PORTE
#define AC0_PIN_bm	PIN7_bm
#define AC0_PIN_bp	PIN7_bp
    
#define AC1_PORT	PORTE
#define AC1_PIN_bm	PIN6_bm
#define AC1_PIN_bp	PIN6_bp

    
#define SET_AC0()       ( AC0_PORT.OUT |= AC0_PIN_bm )
#define CLEAR_AC0()     ( AC0_PORT.OUT &= ~AC0_PIN_bm )
#define TOGGLE_AC0()    ( AC0_PORT.OUT ^= 1UL << AC0_PIN_bp);
    
#define SET_AC1()       ( AC1_PORT.OUT |= AC1_PIN_bm )
#define CLEAR_AC1()     ( AC1_PORT.OUT &= ~AC1_PIN_bm )
#define TOGGLE_AC1()    ( AC1_PORT.OUT ^= 1UL << AC1_PIN_bp);
    
void ACGEN_init(void);


#ifdef	__cplusplus
}
#endif

#endif	/* PINES_H */

