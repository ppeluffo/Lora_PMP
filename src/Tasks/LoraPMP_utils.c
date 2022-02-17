/* 
 * File:   frtos20_utils.c
 * Author: pablo
 *
 * Created on 22 de diciembre de 2021, 07:34 AM
 */

#include "LoraPMP.h"

void SYSTEM_init();
int8_t WDT_init(void);
int8_t CLKCTRL_init(void);

uint8_t calc_checksum( uint8_t *s, uint16_t size );

//-----------------------------------------------------------------------------
void system_init()
{
//	mcu_init();

	CLKCTRL_init();
    WDT_init();
    LED_init();
    LORA_init();
    FLASH_0_init();
    
}
//-----------------------------------------------------------------------------
int8_t WDT_init(void)
{
	/* 8K cycles (8.2s) */
	/* Off */
	ccp_write_io((void *)&(WDT.CTRLA), WDT_PERIOD_8KCLK_gc | WDT_WINDOW_OFF_gc );  
	return 0;
}
//-----------------------------------------------------------------------------
int8_t CLKCTRL_init(void)
{
	// Configuro el clock para 24Mhz
	
	ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA), CLKCTRL_FREQSEL_24M_gc         /* 24 */
	| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable: disabled */
	| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby: disabled */);

	// ccp_write_io((void*)&(CLKCTRL.MCLKCTRLA),CLKCTRL_CLKSEL_OSCHF_gc /* Internal high-frequency oscillator */
	//		 | 0 << CLKCTRL_CLKOUT_bp /* System clock out: disabled */);

	// ccp_write_io((void*)&(CLKCTRL.MCLKLOCK),0 << CLKCTRL_LOCKEN_bp /* lock enable: disabled */);

	return 0;
}
//-----------------------------------------------------------------------------
void reset(void)
{
	/* Issue a Software Reset to initilize the CPU */
	//ccp_write_io( (void *)&(RSTCTRL.SWRR), RSTCTRL_SWRF_bm );
    
    // Habilito la fuente de software reset
    RSTCTRL.RSTFR |= RSTCTRL_SWRF_bm;
    // Genero el reset
    ccp_write_io( (void *)&(RSTCTRL.SWRR),  RSTCTRL_SWRST_bm ); 
}
//-----------------------------------------------------------------------------
void load_defaults(void)
{
    // Carga en el systemVars la configuracion por defecto
    systemVars.dac_value = 100;
    
}
//-----------------------------------------------------------------------------
uint8_t calc_checksum( uint8_t *s, uint16_t size )
{
	/*
	 * Recibe un puntero a una estructura y un tamaño.
	 * Recorre la estructura en forma lineal y calcula el checksum
	 */

uint8_t *p = NULL;
uint8_t checksum = 0;
uint16_t i = 0;

	checksum = 0;
	p = s;
	for ( i = 0; i < size ; i++) {
		checksum += p[i];
	}
	checksum = ~checksum;
	return(checksum);
}
//------------------------------------------------------------------------------
void save_params_in_NVMEE(void)
{

uint8_t checksum = 0;
uint16_t ee_wr_addr;

	// SystemVars.
	// Guardo systemVars en la EE
	ee_wr_addr = 0x00;
    FLASH_0_write_eeprom_block( ee_wr_addr, (uint8_t *)&systemVars, sizeof(systemVars));
        
	ee_wr_addr += sizeof(systemVars);
	checksum = calc_checksum( (uint8_t *)&systemVars, sizeof(systemVars) );
    FLASH_0_write_eeprom_byte(ee_wr_addr, checksum);

}
//------------------------------------------------------------------------------
bool load_params_from_NVMEE(void)
{
	/*
	 * Leo el systemVars desde la EE.
	 * Calculo el checksum. Si no coincide es que hubo algun
	 * error por lo que cargo el default.
	 * Hago el proceso inverso de save
	 */

uint8_t stored_checksum;
uint8_t calculated_checksum;
uint16_t ee_rd_addr;

	// systemVars.
	ee_rd_addr = 0x00;
	FLASH_0_read_eeprom_block(ee_rd_addr, (char *)&systemVars, sizeof(systemVars));
	calculated_checksum = calc_checksum( (uint8_t *)&systemVars, sizeof(systemVars) );
	ee_rd_addr += sizeof(systemVars);
	stored_checksum = FLASH_0_read_eeprom_byte(ee_rd_addr);


	if ( calculated_checksum != stored_checksum ) {
		xprintf_P( PSTR("ERROR: Checksum systemVars failed: calc[0x%0x], sto[0x%0x]\r\n"), calculated_checksum, stored_checksum );
		return(false);
	}

	return(true);
}
//------------------------------------------------------------------------------
