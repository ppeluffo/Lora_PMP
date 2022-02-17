

#include "drv_i2c_avrDX.h"

void i2c_tx_slave_devaddress( uint8_t slaveAddres, uint8_t directionBit);
bool i2c_await_ack( void );
bool i2c_rx_slave_byte( char *rxChar);
bool i2c_tx_slave_byte( char txChar);
void i2c_setACKAction(void);
void i2c_setNACKAction(void);
void i2c_sendMasterCommand(uint8_t newCommand);

#define I2C_TIMEOUT 10000

#define TWI1_BAUD(F_SCL, T_RISE) ((((((float)4000000 / (float)F_SCL)) - 10 - ((float)4000000 * T_RISE / 1000000))) / 2)

bool i2c_debug_flag;

//------------------------------------------------------------------------------
// FUNCIONES PUBLICAS PARA USO DE FRTOS
//------------------------------------------------------------------------------
int16_t drv_I2C_master_write ( const uint8_t devAddress, 
        const uint16_t dataAddress, 
        const uint8_t dataAddressLength, 
        char *pvBuffer, 
        size_t xBytes )
{
   

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

uint8_t length = xBytes;
char rxChar;
char txbyte;

	// Paso 1: Escribo la direccion interna desde donde quiero leer.
	if ( dataAddressLength > 0 ) {
		// Paso 1.1: Envio START + SLA_address + SLA_W
		i2c_tx_slave_devaddress(devAddress, I2C_DIRECTION_BIT_WRITE);
		if ( ! i2c_await_ack()) {
			if ( i2c_debug_flag )
				xprintf("i2c_read NOT ACK.\r\n");
			return(-1);
		}
		
		// Paso 1.2: Envio el Address High Byte
		if ( dataAddressLength == 2 ) {
			// HIGH address
			txbyte = (dataAddress) >> 8;
			i2c_tx_slave_byte(txbyte);
			if ( ! i2c_await_ack()) {
				if ( i2c_debug_flag )
					xprintf("i2c_read NOT ACK.\r\n");
				return(-1);				
			}
		}
		
		// Paso 1.3: Envio el Address Low Byte
		// LOW address
		txbyte = (dataAddress) & 0x00FF;
		i2c_tx_slave_byte(txbyte);
		if ( ! i2c_await_ack()) {
			if ( i2c_debug_flag )
				xprintf("i2c_read NOT ACK.\r\n");
			return(-1);
		}			
	}
	
	// Paso 2:   Leo.
	// Paso 2.1: Envio START + SLA_address + SLA_R
	i2c_tx_slave_devaddress(devAddress, I2C_DIRECTION_BIT_READ);
	if ( ! i2c_await_ack()) {
		if ( i2c_debug_flag )
			xprintf("i2c_read NOT ACK.\r\n");
		return(-1);
	}

	// Paso 2.2: Leo el resto de los bytes.
	while( length-- > 1) {
		i2c_rx_slave_byte(&rxChar);
		*pvBuffer++ = rxChar;
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
		if ( i2c_debug_flag ) {
			xprintf("i2c_read a: status=0x%02x\r\n", TWI1.MSTATUS);
			xprintf("i2c_read b: rxchar=0x%02x\r\n", rxChar);
		}
		i2c_setACKAction();
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
		if ( i2c_debug_flag )
            xprintf("i2c_read c: status=0x%02x\r\n", TWI1.MSTATUS);
		i2c_sendMasterCommand(TWI_MCMD_RECVTRANS_gc);
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
		if ( i2c_debug_flag )
            xprintf("i2c_read d: status=0x%02x\r\n", TWI1.MSTATUS);
	}
	
	// Paso 2.3: Ultimo byte: NACK,STOP
	i2c_rx_slave_byte(&rxChar);
	*pvBuffer++ = rxChar;
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	if ( i2c_debug_flag ) {
		xprintf("i2c_read e: status=0x%02x\r\n", TWI1.MSTATUS);
		xprintf("i2c_read f: rxchar=0x%02x\r\n", rxChar);
	}
	i2c_setNACKAction();
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	if ( i2c_debug_flag )
		xprintf("i2c_read g: status=0x%02x\r\n", TWI1.MSTATUS);
	i2c_sendMasterCommand(TWI_MCMD_STOP_gc);
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	if ( i2c_debug_flag )
		xprintf("i2c_read h: status=0x%02x\r\n", TWI1.MSTATUS);
	
	return(xBytes);

}
//------------------------------------------------------------------------------
void drv_I2C_config_debug(bool debug_flag)
{
    i2c_debug_flag = debug_flag;
}
//------------------------------------------------------------------------------
void drv_I2C_reset(void)
{
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	if ( i2c_debug_flag )
		xprintf("i2c_reset\r\n");
	TWI1.MCTRLB |= TWI_FLUSH_bm;
	TWI1.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
	// Reset module
	TWI1.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);

}
//------------------------------------------------------------------------------
void drv_I2C_init(void)
{

	i2c_debug_flag = false;
    
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
// FUNCIONES PRIVADAS
//------------------------------------------------------------------------------
void i2c_tx_slave_devaddress(uint8_t slaveAddres, uint8_t directionBit)
{
	// Pass1: Mando un START y el SLAVE_ADDRESS (SLA_W/R)
	// El START se genera automaticamente al escribir en el reg MADDR.

	uint8_t txAddress;

	txAddress = (slaveAddres << 1) + directionBit;
	TWI1.MADDR = txAddress;
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	if ( i2c_debug_flag )
		xprintf("i2c_tx_slave_devaddress: ADDR=0x%02x\r\n",txAddress);

}
//------------------------------------------------------------------------------
bool i2c_await_ack(void)
{

uint8_t tryes = 10;

	while(tryes-- > 0) {
		if ((TWI1.MSTATUS & TWI_RXACK_bm) == 0) {
            vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
			if ( i2c_debug_flag ) {
				xprintf("i2c_await_ack ACK: STATUS=0x%02x\r\n",TWI1.MSTATUS);
			}
			return(true);
		}
		vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	}
	if ( i2c_debug_flag )
		xprintf("i2c_await_ack TO: STATUS=0x%02x\r\n",TWI1.MSTATUS);
		
	return(false);
}
//------------------------------------------------------------------------------
bool i2c_rx_slave_byte( char *rxChar)
{
	// Espera con timeout recibir un byte del slave
	
	uint8_t timeout = 5;

	while(timeout-- > 0) {
		if ( (TWI_RIF_bm & TWI1.MSTATUS) != 0 ) {
			*rxChar = TWI1.MDATA;
            vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
			if ( i2c_debug_flag )
				xprintf("i2c_rx_slave_byte: RXBYTE=0x%02x\r\n", *rxChar );
			return(true);
		}
	}
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	if ( i2c_debug_flag )
		xprintf("i2c_rx_slave_byte: TO. STATUS=0x%02x\r\n", TWI1.MSTATUS );
	return(false);
}
//------------------------------------------------------------------------------
bool i2c_tx_slave_byte( char txChar)
{
	// Transmite un byte del slave
	
	uint8_t timeout = 5;

	while(timeout-- > 0) {
		if ( (TWI_WIF_bm & TWI1.MSTATUS) != 0 ) {
			TWI1.MDATA = txChar;
            vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
			if ( i2c_debug_flag )
                xprintf("i2c_tx_slave_byte: TXBYTE=0x%02x\r\n", txChar );
			return(true);
		}
	}
	
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	if ( i2c_debug_flag )
		xprintf("i2c_tx_slave_byte: TO. STATUS=0x%02x\r\n", TWI1.MSTATUS );
	
	return(false);
}
//------------------------------------------------------------------------------
void i2c_setACKAction(void)
{
	TWI1.MCTRLB &= !TWI_ACKACT_bm;
}
//------------------------------------------------------------------------------
void i2c_setNACKAction(void)
{
	TWI1.MCTRLB |= TWI_ACKACT_bm;
}
//------------------------------------------------------------------------------
void i2c_sendMasterCommand(uint8_t newCommand)
{
	TWI1.MCTRLB |=  newCommand;
}
//------------------------------------------------------------------------------
