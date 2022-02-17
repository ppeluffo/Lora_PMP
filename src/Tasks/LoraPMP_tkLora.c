/*
 * File:   tkTerm.c
 * Author: pablo
 *
 * Created on 25 de octubre de 2021, 12:50 PM
 */


#include "LoraPMP.h"

#define LORA_RX_BUFFER_SIZE 64
char lora_rx_buffer[LORA_RX_BUFFER_SIZE];
lBuffer_s lora_rx_sdata;

//------------------------------------------------------------------------------
void LoraPMP_tkLora(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;
uint8_t c = 0;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
   
	xprintf( "\r\n\r\nstarting tkLora..\r\n" );
    lBchar_CreateStatic(&lora_rx_sdata, (char *)&lora_rx_buffer, LORA_RX_BUFFER_SIZE );
            
	// loop
	for( ;; )
	{
		c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.
		while ( frtos_read( fdLORA, (char *)&c, 1 ) == 1 ) {
            lBchar_Poke(&lora_rx_sdata,  (char *)&c);
        }
	}   
}
//------------------------------------------------------------------------------
void printLoraResponse(void)
{
    
char *p;
uint16_t xBytes = 0;

    p = lBchar_get_buffer(&lora_rx_sdata);
    xBytes = strlen(p);
    xprintf("lbuff[%d]\r\n", xBytes);
    
}
//------------------------------------------------------------------------------
void clearLoraRxBuffer(void)
{
    lBchar_Flush(&lora_rx_sdata);
}
//------------------------------------------------------------------------------
