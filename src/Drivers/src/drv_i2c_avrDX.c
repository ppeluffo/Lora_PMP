

#include "drv_i2c_avrDX.h"


#define I2C_TIMEOUT 10000

#define TWI1_BAUD(F_SCL, T_RISE) ((((((float)4000000 / (float)F_SCL)) - 10 - ((float)4000000 * T_RISE / 1000000))) / 2)

#define I2C_DIRECTION_BIT_WRITE               0
#define I2C_DIRECTION_BIT_READ                1
    
#define I2C_MAXTRIES_TO_IDLE    3
    
typedef enum TWIM_RESULT_enum {
	TWIM_RESULT_UNKNOWN          = (0x00<<0),
	TWIM_RESULT_OK               = (0x01<<0),
	TWIM_RESULT_BUFFER_OVERFLOW  = (0x02<<0),
	TWIM_RESULT_ARBITRATION_LOST = (0x03<<0),
	TWIM_RESULT_BUS_ERROR        = (0x04<<0),
	TWIM_RESULT_NACK_RECEIVED    = (0x05<<0),
	TWIM_RESULT_FAIL             = (0x06<<0),
	TWIM_RESULT_TIMEOUT          = (0x07<<0),
} TWIM_RESULT_t;

bool i2c_debug_flag;


bool i2c_send_devAddress( uint8_t slaveAddres, uint8_t directionBit);
bool i2c_send_dataAddress( uint16_t dataAddress, uint8_t dataAddressLength);
bool i2c_send_data( char *wrBuffer, uint8_t bytesToWrite );
bool i2c_rcvd_data( char *rdBuffer, uint8_t bytesToRead );
uint8_t i2c_busTransactionStatus(void);
void i2c_MasterArbitrationLostBusErrorHandler(void);
bool i2c_set_bus_idle(void);

//------------------------------------------------------------------------------
// FUNCIONES PUBLICAS PARA USO DE FRTOS
//------------------------------------------------------------------------------
int16_t drv_I2C_master_write ( const uint8_t devAddress, 
        const uint16_t dataAddress, 
        const uint8_t dataAddressLength, 
        char *pvBuffer, 
        size_t xBytes )
{

    // Paso 1: PONER EL BUS EN CONDICIONES
	if ( ! i2c_set_bus_idle() ) {
		I2C_reset();
		return(-1);
    }

	// Paso 2: Envio START + SLA_address + SLA_W
	if ( ! i2c_send_devAddress( devAddress, I2C_DIRECTION_BIT_WRITE ) ) {
		return(-1);
    }

	// Paso 3: DATA_ADDRESS
	if ( ! i2c_send_dataAddress ( dataAddress, dataAddressLength ))
		return(-1);

	// Paso 4: DATA
	if ( ! i2c_send_data( pvBuffer, xBytes))
		return(-1);


    if ( i2c_debug_flag ) {
        xprintf_P(PSTR("drv_I2C_master_write: EXIT\r\n"));
    }

	return(xBytes);
    
}
//------------------------------------------------------------------------------
int16_t drv_I2C_master_read ( const uint8_t devAddress, 
        const uint16_t dataAddress, 
        const uint8_t dataAddressLength, 
        char *pvBuffer, 
        size_t xBytes )

{
	/*
	devAddress: Direccion I2C del dispositivo
	dataAddress: Direccion interna del dispositivo de comienzo de lectura
	dataAddressLengtgh: Tamanio del memAddress. ( 0,1,2 bytes)
	 - Si es 0 no mandamos el memAddress
	pvBuffer: Donde almacenamos los datos que el dispositivo transmite
	xBytes: cuantos bytes leemos (payload)
	*/

    // Paso 1: PONER EL BUS EN CONDICIONES
	if ( ! i2c_set_bus_idle() ) {
		I2C_reset();
		return(-1);
    }

	// Paso 2: Envio START + SLA_address + SLA_W
	if ( ! i2c_send_devAddress( devAddress, I2C_DIRECTION_BIT_WRITE ) ) {
		return(-1);
    }

	// Paso 3: DATA_ADDRESS
	if ( ! i2c_send_dataAddress ( dataAddress, dataAddressLength ))
		return(-1);
    
    
    	// Paso 4: REPEATED START + DEVICE_ADDRESS + RD
	if ( ! i2c_send_devAddress( devAddress, I2C_DIRECTION_BIT_READ ) ) {
		return(-1);
    }

    // Paso 5: DATA
	if ( !  i2c_rcvd_data( pvBuffer, xBytes)) {
		return(-1);
    }

    if ( i2c_debug_flag ) {
        xprintf_P(PSTR("drv_I2C_master_read: EXIT\r\n"));
    }

	return(xBytes);

}
//------------------------------------------------------------------------------
void drv_I2C_config_debug(bool debug_flag)
{
    i2c_debug_flag = debug_flag;
}
//------------------------------------------------------------------------------
void drv_I2C_init(void)
{

	i2c_debug_flag = true;
    
	PF3_set_level(false);
	PF3_set_dir(PORT_DIR_OUT);
	PF3_set_pull_mode(PORT_PULL_OFF);
	PF3_set_inverted(false);
	PF3_set_isc(PORT_ISC_INTDISABLE_gc);

	PF2_set_level(false);
	PF2_set_dir(PORT_DIR_OUT);
	PF2_set_pull_mode(PORT_PULL_OFF);
	PF2_set_inverted(false);
	PF2_set_isc(PORT_ISC_INTDISABLE_gc);

	// set i2c bit rate to 100KHz
	TWI1.MBAUD = (uint8_t)TWI1_BAUD(100000, 0);
	
	// enable TWI (two-wire interface)
	TWI1.MCTRLA = 1 << TWI_ENABLE_bp        /* Enable TWI Master: enabled */
	| 0 << TWI_QCEN_bp						/* Quick Command Enable: disabled */
	| 0 << TWI_RIEN_bp						/* Read Interrupt Enable: disabled */
	| 0 << TWI_SMEN_bp						/* Smart Mode Enable: disabled */
	| TWI_TIMEOUT_DISABLED_gc				/* Bus Timeout Disabled */
	| 0 << TWI_WIEN_bp;						/* Write Interrupt Enable: disabled */

	//I2C_reset(true);

}
//------------------------------------------------------------------------------
void I2C_reset(void)
{
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	if ( i2c_debug_flag ) {
		xprintf("i2c_reset\r\n");
    }
	TWI1.MCTRLB |= TWI_FLUSH_bm;
	TWI1.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
	// Reset module
	TWI1.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);

}
//------------------------------------------------------------------------------
bool i2c_send_devAddress( uint8_t slaveAddres, uint8_t directionBit)
{
    
uint8_t txAddress;
bool retS = false;

    if ( i2c_debug_flag ) {
        xprintf("i2c_send_devAddress START\r\n");
    }

	txAddress = (slaveAddres << 1) + directionBit;
	TWI1.MADDR = txAddress;
	if ( i2c_busTransactionStatus() == TWIM_RESULT_OK) {
        if ( i2c_debug_flag ) {
            xprintf("i2c_send_devAddress: ADDR=0x%02x\r\n",txAddress);
        }
		retS = true;
    } else {
        if ( i2c_debug_flag ) {
            xprintf_P(PSTR("i2c_send_devAddress: ERROR\r\n"));
        }
        retS = false;
    }
    
    if ( i2c_debug_flag ) {
        xprintf("i2c_send_devAddress END\r\n");
    }
    
    return(retS);
}
//------------------------------------------------------------------------------
bool i2c_send_dataAddress( uint16_t dataAddress, uint8_t dataAddressLength)
{

uint8_t txData;
bool retS = false;

    if ( i2c_debug_flag ) {
        xprintf("i2c_send_dataAddress START\r\n");
    }

    if ( i2c_debug_flag ) {
        xprintf_P(PSTR("i2c_send_dataAddress: (%d)(%d)\r\n"), dataAddress,dataAddressLength );
    }

	// HIGH address
	if ( dataAddressLength == 2 ) {
		txData = (dataAddress) >> 8;
        if ( i2c_debug_flag ) {
            xprintf_P(PSTR("i2c_send_dataAddress: ADDR_H=0x%02x, status=0x%02x\r\n"), txData, TWI1.MSTATUS );
        }
		TWI1.MDATA = txData;
		if ( i2c_busTransactionStatus() != TWIM_RESULT_OK)
			goto quit;
	}

	// LOW address
	if ( dataAddressLength >= 1 ) {
		txData = (dataAddress) & 0x00FF;
        if ( i2c_debug_flag ) {
            xprintf_P( PSTR("i2c_send_dataAddress: ADDR_L=0x%02x, status=0x%02x\r\n"), txData, TWI1.MSTATUS );
        }
		TWI1.MDATA = txData;
		if ( i2c_busTransactionStatus() != TWIM_RESULT_OK)
			goto quit;
	}

	retS = true;

quit:

	if ( !retS ) {
        if ( i2c_debug_flag ) {
            xprintf_P(PSTR("i2c_send_dataAddress: ERROR (status=%d)\r\n"), TWI1.MSTATUS );
        }
    }

    if ( i2c_debug_flag ) {
        xprintf("i2c_send_dataAddress END\r\n");
    }

	return(retS);

}
//------------------------------------------------------------------------------
bool i2c_send_data( char *wrBuffer, uint8_t bytesToWrite )
{
	// Envio todo un buffer en modo write.

uint8_t bytesWritten;
uint8_t txData;
bool retS;

    if ( i2c_debug_flag ) {
        xprintf("i2c_send_data START\r\n");
    }

	// No hay datos para enviar: dummy write.
	if ( bytesToWrite == 0 )
		return(true);

	// Mando el buffer de datos. Debo recibir 0x28 (DATA_ACK) en c/u
	for ( bytesWritten=0; bytesWritten < bytesToWrite; bytesWritten++ ) {
		txData = *wrBuffer++;
        if ( i2c_debug_flag ) {
            xprintf_P( PSTR("i2c_send_data: 0x%02x(%c),0x%02x\r\n"),txData,txData, TWI1.MSTATUS );
        }
		TWI1.MDATA = txData;
		if ( i2c_busTransactionStatus() != TWIM_RESULT_OK)
			goto quit;
	}
	retS = true;

quit:

	if (! retS ) {
        if ( i2c_debug_flag ) { 
            xprintf_P( PSTR("i2c_send_data: ERROR (status=%d)\r\n"), TWI1.MSTATUS );
        }
    }

	// Envie todo el buffer. Termino con un STOP.
	TWI1.MCTRLB = TWI_MCMD_gm;  // STOP
    
    if ( i2c_debug_flag ) {
        xprintf("i2c_send_data END\r\n");
    }
    
	return(retS);
}
//------------------------------------------------------------------------------
bool i2c_rcvd_data( char *rdBuffer, uint8_t bytesToRead )
{
	// Envio todo un buffer en modo write.

uint8_t bytesRead;
uint8_t rxData;
bool retS = false;

    if ( i2c_debug_flag ) {
        xprintf("i2c_rcvd_data START\r\n");
    }

	// Leo el buffer de datos.
	for ( bytesRead=0; bytesRead < bytesToRead; bytesRead++ ) {
		rxData = TWI1.MDATA;
		*rdBuffer++ = rxData;
        if ( i2c_debug_flag ) {
            xprintf_P( PSTR("i2c_rcvd_data: 0x%02x(%c),0x%02x\r\n"),rxData,rxData, TWI1.MSTATUS );
        }
        TWI1.MCTRLB &= !TWI_ACKACT_bm;  // Send ACK
		if ( i2c_busTransactionStatus() != TWIM_RESULT_OK)
			goto quit;
	}
	retS = true;

quit:

	if (! retS ) {
		xprintf_P( PSTR("i2c_rcvd_data: ERROR (status=%d)\r\n"), TWI1.MSTATUS );
    }

	TWI1.MCTRLB |= TWI_ACKACT_bm;   // STOP + NACK
 
    if ( i2c_debug_flag ) {
        xprintf("i2c_rcvd_data END\r\n");
    }
    
	return(retS);
}
//------------------------------------------------------------------------------
uint8_t i2c_busTransactionStatus(void)
{

	// Evalua el estatus y actua en consecuencia para dejar el bus listo para otra transaccion

uint8_t ticks_to_wait = 50;		// ( 50ms es el maximo tiempo que espero )
uint8_t currentStatus;

	// wait for i2c interface to complete write operation ( MASTER WRITE INTERRUPT )
	while ( ticks_to_wait-- > 0 ) {

		currentStatus = TWI1.MSTATUS;
        if ( i2c_debug_flag ) {
            xprintf_P(PSTR("i2c_busTransactionStatus: status=0x%02x\r\n"), currentStatus );
        }
        
		/* If arbitration lost or bus error. */
		if ( currentStatus & TWI_ARBLOST_bm ) {
			i2c_MasterArbitrationLostBusErrorHandler();
			return(TWIM_RESULT_ARBITRATION_LOST);

		} else if ( currentStatus & TWI_BUSERR_bm ) {
			i2c_MasterArbitrationLostBusErrorHandler();
			return(TWIM_RESULT_BUS_ERROR);

		/* If master write interrupt. */
		} else if (currentStatus & TWI_WIF_bm ) {
			// NACK
			if ( currentStatus & TWI_RXACK_bm ) {
				TWI1.MCTRLB = TWI_MCMD_gm;  // STOP
				return(TWIM_RESULT_NACK_RECEIVED);

			} else {
				// ACK
				return(TWIM_RESULT_OK);
			}

		/* If master read interrupt. */
		} else if (currentStatus & TWI_RIF_bm ) {
			return(TWIM_RESULT_OK);
		}

		vTaskDelay( ( TickType_t)( 1 ) );
	}

	// DEBUG
	xprintf_P( PSTR("i2c_busTransactionStatus: TIMEOUT status=0x%02x\r\n"), currentStatus );
	TWI1.MCTRLB = TWI_MCMD_gm;  // STOP
	return(TWIM_RESULT_TIMEOUT);

}
//------------------------------------------------------------------------------
void i2c_MasterArbitrationLostBusErrorHandler(void)
{

uint8_t currentStatus = TWI1.MSTATUS;

    if ( i2c_debug_flag )
        xprintf_P(PSTR("i2c_MasterArbitrationLostBusErrorHandler\r\n"));

	/* If bus error. */
	if (currentStatus & TWI_BUSERR_bm ) {
	//	I2C_control.result = TWIM_RESULT_BUS_ERROR;

	} else {
	/* If arbitration lost. */
	//	I2C_control.result = TWIM_RESULT_ARBITRATION_LOST;
	}

	/* Clear interrupt flag. */
	TWI1.MSTATUS = currentStatus | TWI_BUSERR_bm;
    
    I2C_reset();

	//I2C_control.status = TWIM_STATUS_READY;
}
//------------------------------------------------------------------------------
bool i2c_set_bus_idle(void)
{

	// Para comenzar una operacion el bus debe estar en IDLE o OWENED.
	// Intento pasarlo a IDLE hasta 3 veces antes de abortar, esperando 100ms
	// entre c/intento.

uint8_t	reintentos = I2C_MAXTRIES_TO_IDLE;
bool retS = false;

    if ( i2c_debug_flag ) {
        xprintf("i2c_set_bus_idle START\r\n");
    }

	while ( reintentos-- > 0 ) {

		// Los bits CLKHOLD y RXACK son solo de read por eso la mascara !!!

		//xprintf_PD(DEBUG_I2C, PSTR("I2C: drv_i2c:SetBusIdle tryes=(%d): status=0x%02x\r\n\0"),reintentos,TWIE.MASTER.STATUS );

		if (  ( ( TWI1.MSTATUS & TWI_BUSSTATE_gm ) == 0x00 ) ||
				( ( TWI1.MSTATUS & TWI_BUSSTATE_gm ) == 0x02 ) ) {
			retS = true;
            goto quit;

		} else {
			// El status esta indicando errores. Debo limpiarlos antes de usar la interface.
			if ( (TWI1.MSTATUS & TWI_ARBLOST_bm) != 0 ) {
				TWI1.MSTATUS  |= TWI_ARBLOST_bm;
			}
			if ( (TWI1.MSTATUS & TWI_BUSERR_bm) != 0 ) {
				TWI1.MSTATUS |= TWI_BUSERR_bm;
			}
			if ( (TWI1.MSTATUS & TWI_WIF_bm) != 0 ) {
				TWI1.MSTATUS |= TWI_WIF_bm;
			}
			if ( (TWI1.MSTATUS & TWI_RIF_bm) != 0 ) {
				TWI1.MSTATUS |= TWI_RIF_bm;
			}

            I2C_reset();
			//TWI1.MSTATUS |= TWI_BUSERR_bm;	// Pongo el status en 01 ( idle )
            //TWI1.MCTRLB |= TWI_FLUSH_bm;
			//vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
		}
        
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	}

	// No pude pasarlo a IDLE: Error !!!
	xprintf_P( PSTR("i2c_set_bus_idle: ERROR!!: status=0x%02x\r\n\0"),TWI1.MSTATUS );
    
quit:
    
    if ( i2c_debug_flag ) {
        xprintf("i2c_set_bus_idle END\r\n");
    }
	return(retS);
}
//------------------------------------------------------------------------------
