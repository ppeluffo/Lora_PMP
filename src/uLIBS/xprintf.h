/*
 * l_printf.h
 *
 *  Created on: 8 dic. 2018
 *      Author: pablo
 */

#ifndef SRC_SPX_LIBS_L_PRINTF_H_
#define SRC_SPX_LIBS_L_PRINTF_H_

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "frtos-io.h"
//#include "printf.h"

void xprintf_init(void);
void xputChar( char c);
int xnprint( const char *pvBuffer, const uint16_t xBytes );
void xprintf_cmd( char *s );

int xprintf( const char *fmt, ...);
int xfprintf( uint8_t fd, const char *fmt, ...);

int xprintf_P( PGM_P fmt, ...);

#define BYTE_TO_BINARY_PATTERN %c%c%c%c%c%c%c%c
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#endif /* SRC_SPX_LIBS_L_PRINTF_H_ */
