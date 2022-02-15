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
static void cmdTestFunction(void);

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
    cmdlineAddCommand( "test", cmdTestFunction );
    
	// Fijo el timeout del READ
	ticks = 5;
	frtos_ioctl( fdTERM, ioctl_SET_TIMEOUT, &ticks );

	xprintf_P( PSTR("\r\n\r\nstarting tkCmd..\r\n") );
    
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
        
    if (!strcmp( strupr(argv[1]), "WRITE")) {
		xprintf_P( PSTR("-write\r\n"));
        
    }  else if (!strcmp( strupr(argv[1]), "CONFIG")) {
		xprintf_P( PSTR("-config\r\n"));
		xprintf_P( PSTR("   default,save\r\n"));
        
    }  else {
        // HELP GENERAL
        xprintf_P( PSTR("Available commands are:\r\n"));
        xprintf_P( PSTR("-cls\r\n"));
        xprintf_P( PSTR("-help\r\n"));
        xprintf_P( PSTR("-status\r\n"));
        xprintf_P( PSTR("-reset\r\n"));
        xprintf_P( PSTR("-write...\r\n"));
        xprintf_P( PSTR("-config...\r\n"));
        xprintf_P( PSTR("-test\r\n"));
    
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

	//FRTOS_CMD_makeArgv();

	xprintf_P( PSTR("\r\nSpymovil %s %s %s %s \r\n"), HW_MODELO, FRTOS_VERSION, FW_REV, FW_DATE);
	xprintf_P( PSTR("Clock %d Mhz, Tick %d Hz\r\n"),SYSMAINCLK, configTICK_RATE_HZ );

	// SIGNATURE ID
	//pv_cmd_status_ids();
    
    
}
//------------------------------------------------------------------------------
static void cmdWriteFunction(void)
{
   

    cmdlineMakeArgv();

	// DAC
	// write dac {val}
	if ( strcmp( strupr(argv[1]),"DAC") == 0 ) {
        systemVars.dac_value = atoi(argv[2]);
		pv_snprintfP_OK();
		return;
	}

    if ( strcmp( strupr(argv[1]),"AC0") == 0 ) {
        if ( strcmp( strupr(argv[2]),"ON") == 0 ) {
            SET_AC0();
            pv_snprintfP_OK();
            return;
        }
        if ( strcmp( strupr(argv[2]),"OFF") == 0 ) {
            CLEAR_AC0();
            pv_snprintfP_OK();
            return;
        }
        pv_snprintfP_ERR();
        return;
    }

    if ( strcmp( strupr(argv[1]),"AC1") == 0 ) {
        if ( strcmp( strupr(argv[2]),"ON") == 0 ) {
            SET_AC1();
            pv_snprintfP_OK();
            return;
        }
        if ( strcmp( strupr(argv[2]),"OFF") == 0 ) {
            CLEAR_AC1();
            pv_snprintfP_OK();
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
		//u_save_params_in_NVMEE();
		pv_snprintfP_OK();
		return;
	}

    // CMD NOT FOUND
	xprintf_P( PSTR("ERROR\r\nCMD NOT DEFINED\r\n\0"));
	return;
    
}
//------------------------------------------------------------------------------
static void cmdTestFunction(void)
{
    
static uint8_t idx=0;
uint8_t* argptr;
char argument[12];

uint8_t nro_args;
uint8_t i;
uint8_t len;
char banner[] = "TEST FUNCTION 4";

 
    xprintf("%s\r\n", banner);

    nro_args = cmdlineMakeArgv();
    xprintf("Nro_args=%d\r\n", nro_args);
    for (i=0; i<nro_args; i++ ) {
        len = strlen(argv[i]);
        xprintf("ARG%d=[%d][%s]\r\n",i,len,argv[i]);
        
    }
    return;

    

    for (idx=0; idx<4; idx++) {
        argptr = cmdlineGetArgStr(idx);
        memset(argument,'\0', 12);
        strcpy(argptr, argument);
        
        xprintf("ARG=%d[%s]\r\n",idx, argument);
    }
    
    return;
    
    memset(test_buffer, '\0', sizeof(test_buffer) );
    strcpy(test_buffer, "  write dac prueba pablo spymovil");
    
    xprintf_P(PSTR("Test:[%s]\r\n"), test_buffer);
    
	// inicialmente todos los punteros deben apuntar a NULL.
	memset(argv, '\0', sizeof(argv) );

    // Primer argumento si comienza al inicio del buffer
    if ( test_buffer[0] != ' ') {
        argv[nro_args++] =  &test_buffer[0];
        xprintf_P(PSTR("Test: nro_args=0, idx=0, char=%c\r\n"), test_buffer[idx] );
    }
    
 	for ( idx=1; idx<(MAX_LENGTH - 1); idx++ ) {
        if ( (test_buffer[idx-1] == ' ') && (test_buffer[idx] != ' ')) {
            test_buffer[idx-1] = '\0';
            xprintf_P(PSTR("Test: nro_args=%d, idx=%d, char=%c\r\n"), nro_args,idx,test_buffer[idx] );
            argv[nro_args++] =  &test_buffer[idx];
            if (nro_args == MAX_NUM_ARGS) 
                goto quit;
        }
    }    
   
quit:
    xprintf_P(PSTR("Test End: Nro_args=%d\r\n"), nro_args);
    for(idx=0; idx < nro_args; idx++) {
        xprintf_P(PSTR("Test:[%d][%s]\r\n"), idx, argv[idx]);
    }
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