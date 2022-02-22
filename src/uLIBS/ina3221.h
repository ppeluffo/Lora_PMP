/* 
 * File:   ina3221.h
 * Author: pablo
 *
 * Created on 22 de febrero de 2022, 11:21 AM
 */

#ifndef INA3221_H
#define	INA3221_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "frtos-io.h"
#include "stdint.h"
#include "xprintf.h"

//------------------------------------------------------------------------------------

// WORDS de configuracion de los INAs
#define CONF_INAS_SLEEP			0x7920
#define CONF_INAS_AVG128		0x7927
#define CONF_INAS_PWRDOWN		0x0000


// Direcciones de los registros de los INA
#define INA3231_CONF			0x00
#define INA3221_CH1_SHV			0x01
#define INA3221_CH1_BUSV		0x02
#define INA3221_CH2_SHV			0x03
#define INA3221_CH2_BUSV		0x04
#define INA3221_CH3_SHV			0x05
#define INA3221_CH3_BUSV		0x06
#define INA3221_MFID			0xFE
#define INA3221_DIEID			0xFF

#define INA3221_VCC_SETTLE_TIME	500

#define INA3221_BUS_ADDRESS     0x80
    
//------------------------------------------------------------------------------------
// API publica
void ina3221_config( uint16_t conf_reg_value );
#define ina_config_avg128(ina_id)	ina3221_config( CONF_INAS_AVG128 )
#define ina_config_sleep(ina_id)	ina3221_config( CONF_INAS_SLEEP )
//
int16_t ina3221_write( uint16_t wrAddress, char *data, uint8_t length );
int16_t ina3221_read( uint16_t rdAddress, char *data, uint8_t length );
//
int16_t ina3221_test_write ( char *rconf_val_str );
int16_t ina3221_test_read ( char *regs );
//
//------------------------------------------------------------------------------

#ifdef	__cplusplus
}
#endif

#endif	/* INA3221_H */

