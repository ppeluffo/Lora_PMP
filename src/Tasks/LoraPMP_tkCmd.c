/*
 * File:   tkCmd.c
 * Author: pablo
 *
 * Created on 25 de octubre de 2021, 12:50 PM
 */


#include "LoraPMP.h"
#include "cmdline.h"

static void cmdClsFunction(void);
static void cmdHelpFunction(void);
static void cmdResetFunction(void);
static void cmdStatusFunction(void);
static void cmdWriteFunction(void);
static void cmdConfigFunction(void);
static void cmdReadFunction(void);

static void pv_snprintfP_OK(void );
static void pv_snprintfP_ERR(void );


#define MAX_NUM_ARGS    16
#define MAX_LENGTH 64
char *argv[MAX_NUM_ARGS];
char test_buffer[MAX_LENGTH];

//------------------------------------------------------------------------------
void LoraPMP_tkCmd(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );

uint8_t c = 0;
uint8_t ticks = 0;

	//FRTOS_CMD_init( xputChar, xprintf_cmd );

    cmdlineInit();
    
	// Registro los comandos y los mapeo al cmd string.
    cmdlineAddCommand( "cls", cmdClsFunction );
	cmdlineAddCommand( "help", cmdHelpFunction );
    cmdlineAddCommand( "reset", cmdResetFunction );
    cmdlineAddCommand( "status", cmdStatusFunction );
    cmdlineAddCommand( "write", cmdWriteFunction );
    cmdlineAddCommand( "config", cmdConfigFunction );
    cmdlineAddCommand( "read", cmdReadFunction );
    
	// Fijo el timeout del READ
	ticks = 5;
	frtos_ioctl( fdTERM, ioctl_SET_TIMEOUT, &ticks );

	xprintf( "\r\n\r\nstarting tkCmd..\r\n" );
    
	// loop
	for( ;; )
	{
		c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.
		while ( frtos_read( fdTERM, (char *)&c, 1 ) == 1 ) {
            cmdlineInputFunc(c);
            cmdlineMainLoop();
        }
        //vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	}    
}
//------------------------------------------------------------------------------
static void cmdHelpFunction(void)
{

    
	cmdlineMakeArgv();
        
    if ( strcmp( strupr(argv[1]), "WRITE") == 0 ) {
		xprintf_P( PSTR("-write:\r\n"));
        xprintf_P( PSTR("   lora cts,reset {on|off}\r\n"));
        xprintf_P( PSTR("   lora cmd\r\n"));
        xprintf_P( PSTR("   dac {val}\r\n"));
        
    }  else if ( strcmp( strupr(argv[1]), "CONFIG") == 0 ) {
		xprintf_P( PSTR("-config:\r\n"));
		xprintf_P( PSTR("   default,save\r\n"));

    }  else if ( strcmp( strupr(argv[1]), "READ") == 0 ) {
		xprintf_P( PSTR("-read:\r\n"));
		xprintf_P( PSTR("   eeprom\r\n"));
        xprintf_P( PSTR("   lora rsp\r\n"));
        
    }  else {
        // HELP GENERAL
        xprintf_P( PSTR("Available commands are:\r\n"));
        xprintf_P( PSTR("-cls\r\n"));
        xprintf_P( PSTR("-help\r\n"));
        xprintf_P( PSTR("-status\r\n"));
        xprintf_P( PSTR("-reset\r\n"));
        xprintf_P( PSTR("-write...\r\n"));
        xprintf_P( PSTR("-config...\r\n"));
        xprintf_P( PSTR("-read...\r\n"));

    }
   
	xprintf_P( PSTR("Exit help \r\n"));

}
//------------------------------------------------------------------------------
static void cmdClsFunction(void)
{
	// ESC [ 2 J
	xprintf_P( PSTR("\x1B[2J\0"));
}
//------------------------------------------------------------------------------
static void cmdResetFunction(void)
{
   
    reset();
}
//------------------------------------------------------------------------------
static void cmdStatusFunction(void)
{

	cmdlineMakeArgv();

	xprintf_P( PSTR("\r\nSpymovil %s %s %s %s \r\n"), HW_MODELO, FRTOS_VERSION, FW_REV, FW_DATE);
	xprintf_P( PSTR("Clock %d Mhz, Tick %d Hz\r\n"),SYSMAINCLK, configTICK_RATE_HZ );

    xprintf_P(PSTR("dac: %d\r\n"), systemVars.dac_value);
	// SIGNATURE ID
	//pv_cmd_status_ids();
    
    
}
//------------------------------------------------------------------------------
static void cmdWriteFunction(void)
{
   cmdlineMakeArgv();

   
    // DAC:
    // write dac {val}
    if ( strcmp( strupr(argv[1]),"DAC") == 0 ) {
        systemVars.dac_value = atoi(argv[2]);
        pv_snprintfP_OK();
        return;
    }
   
	// LORA
	// write lora cts,reset {on|off}
 
    if ( strcmp( strupr(argv[1]),"LORA") == 0 ) {
 
        if ( strcmp( strupr(argv[2]),"CTS") == 0 ) {
            if ( strcmp( strupr(argv[3]),"ON") == 0 ) {
                SET_LORA_CTS();
                pv_snprintfP_OK();
                return;
            }
            if ( strcmp( strupr(argv[3]),"OFF") == 0 ) {
                CLEAR_LORA_CTS();
                pv_snprintfP_OK();
                return;
            }
            pv_snprintfP_ERR();
            return;
        }
  
        if ( strcmp( strupr(argv[2]),"RESET") == 0 ) {
            if ( strcmp( strupr(argv[3]),"ON") == 0 ) {
                SET_LORA_RESET();
                pv_snprintfP_OK();
                return;
            }
            if ( strcmp( strupr(argv[3]),"OFF") == 0 ) {
                CLEAR_LORA_RESET();
                pv_snprintfP_OK();
                return;
            }
            pv_snprintfP_ERR();
            return;
        }
        
         pv_snprintfP_ERR();
        return;
    }

    // CMD NOT FOUND
	xprintf_P( PSTR("ERROR\r\nCMD NOT DEFINED\r\n\0"));
	return;
 
}
//------------------------------------------------------------------------------
static void cmdConfigFunction(void)
{
    
    cmdlineMakeArgv();
    	
    // DEFAULT
	// config default
	if (!strcmp( strupr(argv[1]), "DEFAULT")) {
		load_defaults();
		pv_snprintfP_OK();
		return;
	}

	// SAVE
	// config save
	if (!strcmp( strupr(argv[1]), "SAVE")) {
		save_params_in_NVMEE();
		pv_snprintfP_OK();
		return;
	}

    // CMD NOT FOUND
	xprintf_P( PSTR("ERROR\r\nCMD NOT DEFINED\r\n\0"));
	return;
    
}
//------------------------------------------------------------------------------
static void cmdReadFunction(void)
{
    cmdlineMakeArgv();

    // EEPROM:
    // read eeprom
    if ( strcmp( strupr(argv[1]),"EEPROM") == 0 ) {
        load_params_from_NVMEE();
        pv_snprintfP_OK();
        return;
    }
   
        // CMD NOT FOUND
	xprintf_P( PSTR("ERROR\r\nCMD NOT DEFINED\r\n\0"));
	return;

}
//------------------------------------------------------------------------------
static void pv_snprintfP_OK(void )
{
	xprintf_P( PSTR("ok\r\n\0"));
}
//------------------------------------------------------------------------------
static void pv_snprintfP_ERR(void)
{
	xprintf_P( PSTR("error\r\n\0"));
}
//------------------------------------------------------------------------------