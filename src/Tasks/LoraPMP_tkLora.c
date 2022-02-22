/*
 * File:   tkTerm.c
 * Author: pablo
 *
 * Created on 25 de octubre de 2021, 12:50 PM
 */


#include "LoraPMP.h"


//------------------------------------------------------------------------------
void LoraPMP_tkLora(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;
uint8_t c = 0;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
   
    lora_reset_on();
	xprintf( "\r\n\r\nstarting tkLora..\r\n" );
            
	// loop
	for( ;; )
	{
		c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.
		while ( frtos_read( fdLORA, (char *)&c, 1 ) == 1 ) {
            lora_push_RxBuffer( (char *)&c );
        }
	}   
}
//------------------------------------------------------------------------------
