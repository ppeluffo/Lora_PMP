/*
 * l_printf.c
 *
 *  Created on: 13 jul. 2018
 *      Author: pablo
 */


#include "xprintf.h"

#define PRINTF_BUFFER_SIZE        128U

static uint8_t stdout_buff[PRINTF_BUFFER_SIZE];

SemaphoreHandle_t sem_STDOUT;
StaticSemaphore_t STDOUT_xMutexBuffer;

//------------------------------------------------------------------------------
void xprintf_init(void)
{
	sem_STDOUT = xSemaphoreCreateMutexStatic( &STDOUT_xMutexBuffer );
}
//------------------------------------------------------------------------------
void xputChar( char c)
{
	// Funcion intermedia necesaria por cmdline para escribir de a 1 caracter en consola
	// El tema es que el prototipo de funcion que requiere cmdlineSetOutputFunc no se ajusta
	// al de FreeRTOS_UART_write, por lo que defino esta funcion intermedia.

char cChar;

	cChar = c;
	xnprint( &cChar, sizeof(char));
}
//------------------------------------------------------------------------------
int xnprint( const char *pvBuffer, const uint16_t xBytes )
{
	/* Imprime en fdTERM sin formatear
	   No uso stdout_buff por lo tanto no requeriria semaforo pero igual
	   lo uso para evitar colisiones. De este modo todo el acceso al uart queda
	   siempre controlado por el semaforo
	*/

int bytes2wr = 0;

	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	bytes2wr = frtos_write( fdTERM, pvBuffer, xBytes );

	xSemaphoreGive( sem_STDOUT );
	return(bytes2wr);

}
//------------------------------------------------------------------------------
void xprintf_cmd( char *s )
{
	// Esta funcion es con la que incializo el FRTOS_cmd para mostrar strings.
	xprintf(s);

}
//------------------------------------------------------------------------------
int xprintf( const char *fmt, ...)
{
	// Imprime formateando en el uart fd.usando el buffer stdout_buff.
	// Como se controla con semaforo, nos permite ahorrar los buffers de c/tarea.
	// Si bien vsnprintf no es thread safe, al usarla aqui con el semaforo del buffer
	// queda thread safe !!!

va_list args;
int i;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	vsnprintf( (char *)stdout_buff,sizeof(stdout_buff),fmt,args);
    va_end(args);
    
	i = frtos_write(fdTERM, (char *)stdout_buff, strlen((char *)stdout_buff) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//------------------------------------------------------------------------------
int xfprintf( uint8_t fd, const char *fmt, ...)
{
	// Imprime formateando en el uart fd.usando el buffer stdout_buff.
	// Como se controla con semaforo, nos permite ahorrar los buffers de c/tarea.
	// Si bien vsnprintf no es thread safe, al usarla aqui con el semaforo del buffer
	// queda thread safe !!!

va_list args;
int i;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);   
    vsnprintf( (char *)stdout_buff,sizeof(stdout_buff),fmt,args);
    va_end(args);
    
	i = frtos_write(fd, (char *)stdout_buff, strlen((char *)stdout_buff) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//-------------------------------------------------------------------------------
int xprintf_P( PGM_P fmt, ...)
{
	// Imprime formateando en el uart fd.usando el buffer stdout_buff.
	// Como se controla con semaforo, nos permite ahorrar los buffers de c/tarea.
	// Si bien vsnprintf no es thread safe, al usarla aqui con el semaforo del buffer
	// queda thread safe !!!
	// Al usar esta funcion no es necesario controlar el tamaño de los buffers ya que
	// los limita a PRINTF_BUFFER_SIZE

va_list args;
int i;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	vsnprintf_P( (char *)stdout_buff, sizeof(stdout_buff),fmt, args);
    va_end(args);
	i = frtos_write(fdTERM, (char *)stdout_buff, strlen((char *)stdout_buff) );
	// Espero que se vacie el buffer 10ms.
	vTaskDelay( (TickType_t)( 10 / portTICK_PERIOD_MS ) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//------------------------------------------------------------------------------