/* 
 * File:   drv_i2c_avrDX.h
 * Author: pablo
 *
 * Created on 16 de febrero de 2022, 11:49 AM
 */

#ifndef DRV_I2C_AVRDX_H
#define	DRV_I2C_AVRDX_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "xc.h"
#include "stdio.h" 
#include "stdbool.h"
#include "xprintf.h"
#include "task.h"
#include "atmel_start_pins.h"
   
int16_t drv_I2C_master_write ( const uint8_t devAddress, 
        const uint16_t dataAddress, 
        const uint8_t dataAddressLength, 
        char *pvBuffer, 
        size_t xBytes );

int16_t drv_I2C_master_read ( const uint8_t devAddress, 
        const uint16_t dataAddress, 
        const uint8_t dataAddressLength, 
        char *pvBuffer, 
        size_t xBytes );


void drv_I2C_init(void);
void drv_I2C_config_debug(bool debug_flag);
void I2C_reset(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DRV_I2C_AVRDX_H */

