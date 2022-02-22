
#include "ina3221.h"

//------------------------------------------------------------------------------
void ina3221_config( uint16_t conf_reg_value )
{
char res[3] = { 0 };
int16_t xBytes = 0;

	res[0] = ( conf_reg_value & 0xFF00 ) >> 8;
	res[1] = ( conf_reg_value & 0x00FF );

	xBytes = ina3221_write( INA3231_CONF, res, 2 );
	if ( xBytes == -1 )
		xprintf_P( PSTR("ina3221_config: ERROR\r\n"));

}
//------------------------------------------------------------------------------------
int16_t ina3221_test_write ( char *rconf_val_str )
{

	// Escribe en el registro de configuracion de un INA ( 0, 1, 2)

uint16_t val = 0;
char data[3] = { 0 };
int16_t xBytes = 0;

	val = atoi( rconf_val_str);
	data[0] = ( val & 0xFF00 ) >> 8;
	data[1] = ( val & 0x00FF );
	xBytes = ina3221_write( INA3231_CONF, data, 2 );
	if ( xBytes == -1 )
		xprintf_P(PSTR("ina3221_test_write ERROR\r\n"));

	return (xBytes);
}
//------------------------------------------------------------------------------------
int16_t ina3221_test_read ( char *regs )
{

    // read ina id {conf|chxshv|chxbusv|mfid|dieid}
    
uint16_t val = 0;
char data[3] = { ' ' };
int16_t xBytes = 0;
char l_data[10] = { ' ' };

	memcpy(l_data, regs, sizeof(l_data));
	strupr(l_data);

	if (strcmp( l_data, "CONF") == 0 ) {
		xBytes = ina3221_read( INA3231_CONF, data, 2 );
	} else if (strcmp( l_data, "CH1SHV") == 0) {
		xBytes = ina3221_read( INA3221_CH1_SHV, data, 2 );
	} else if (strcmp( l_data, "CH1BUSV") == 0) {
		xBytes = ina3221_read( INA3221_CH1_BUSV, data, 2 );
	} else if (strcmp( l_data, "CH2SHV") == 0) {
		xBytes = ina3221_read( INA3221_CH2_SHV, data, 2 );
	} else if (strcmp( l_data, "CH2BUSV") == 0) {
		xBytes = ina3221_read( INA3221_CH2_BUSV, data, 2 );
	} else if (strcmp( l_data, "CH3SHV") == 0) {
		xBytes = ina3221_read( INA3221_CH3_SHV, data, 2 );
	} else if (strcmp( l_data, "CH3BUSV") == 0) {
		xBytes = ina3221_read( INA3221_CH3_BUSV, data, 2 );
	} else if (strcmp( l_data, "MFID") == 0) {
		xBytes = ina3221_read( INA3221_MFID, data, 2 );
	} else if (strcmp( l_data, "DIEID") == 0) {
		xBytes = ina3221_read( INA3221_DIEID, data, 2 );
	} else {
		xBytes = -1;
	}

	if ( xBytes == -1 ) {
		xprintf_P(PSTR("ina3221_test_read ERROR\r\n"));

	} else {

		val = ( data[0]<< 8 ) + data[1];
		xprintf_P( PSTR("VAL=0x%04x\r\n"), val);
//		xprintf_P( PSTR("out->DEBUG=[%02x][%02x]\r\n\0"), data[0], data[1]);
	}

	return(xBytes);

}
//------------------------------------------------------------------------------------
int16_t ina3221_read( uint16_t rdAddress, char *data, uint8_t length )
{

int16_t rcode = 0;
uint16_t i2c_args;

	//I2C_get_semaphore();
    i2c_args = INA3221_BUS_ADDRESS;
    frtos_ioctl(fdI2C, ioctl_I2C_SET_DEVADDRESS, &i2c_args );

    i2c_args = rdAddress;
    frtos_ioctl(fdI2C, ioctl_I2C_SET_DATAADDRESS, &i2c_args );
    
    i2c_args = 0x01;
    frtos_ioctl(fdI2C, ioctl_I2C_SET_DATAADDRESSLENGTH, &i2c_args );
    
    rcode = frtos_read( fdI2C, data, length );
	//I2C_release_semaphore();

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ina3221_read: recovering i2c bus\r\n"));
		i2c_args = 0x0;
        frtos_ioctl(fdI2C, ioctl_I2C_RESET, &i2c_args );
	}

	return( rcode );
}
//------------------------------------------------------------------------------------
int16_t ina3221_write( uint16_t wrAddress, char *data, uint8_t length )
{

int16_t rcode = 0;
uint16_t i2c_args;

//	xprintf_P(PSTR("DEBUG:INA_write: dev_id=0x%02x, wrAddress=0x%02x, length=0x%02x\r\n"), dev_id, wrAddress, length);
//	xprintf_P(PSTR("DEBUG:INA_write: dev_ussAddr=0x%02x\r\n"), INA_id2busaddr(dev_id));


	//I2C_get_semaphore();
    i2c_args = INA3221_BUS_ADDRESS;
    frtos_ioctl(fdI2C, ioctl_I2C_SET_DEVADDRESS, &i2c_args );

    i2c_args = wrAddress;
    frtos_ioctl(fdI2C, ioctl_I2C_SET_DATAADDRESS, &i2c_args );
    
    i2c_args = 0x01;
    frtos_ioctl(fdI2C, ioctl_I2C_SET_DATAADDRESSLENGTH, &i2c_args );
    
    rcode = frtos_write( fdI2C, data, length );
    
	//I2C_release_semaphore();
	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ina3221_write: recovering i2c bus\r\n"));
		i2c_args = 0x0;
        frtos_ioctl(fdI2C, ioctl_I2C_RESET, &i2c_args );
	}

	return( rcode );

}
//------------------------------------------------------------------------------------
