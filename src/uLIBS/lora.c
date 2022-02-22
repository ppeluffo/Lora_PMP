
#include "lora.h"


// -----------------------------------------------------------------------------
void LORA_init(void)
{
    LORA_RTS_CONFIG();
    LORA_CTS_CONFIG();
    LORA_RESET_CONFIG();
    
    //lBchar_CreateStatic(&lora_rx_sdata, (char *)&lora_rx_buffer, LORA_RX_BUFFER_SIZE );
}
// -----------------------------------------------------------------------------
void lora_flush_RxBuffer(void)
{
    //lBchar_Flush(&lora_rx_sdata);
    //memset( &lora_rx_buffer, '\0', LORA_RX_BUFFER_SIZE);
    //lora_rx_sdata.ptr = 0;
    memset( lora_rx_data.rx_buffer, '\0', LORA_RX_BUFFER_SIZE );
    lora_rx_data.ptr = 0;
    
}
// -----------------------------------------------------------------------------
void lora_flash_led(void)
{
    xprintf( "Lora test led.\r\n");

    xfprintf( fdLORA, "sys set pindig GPIO5 1\r\n");
    vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
    xfprintf( fdLORA, "sys set pindig GPIO5 0\r\n");
}
// -----------------------------------------------------------------------------
void lora_print_RxBuffer(void)
{
    // xprintf( "[%s]\r\n", lora_rx_sdata.buff  );
    xprintf( "[%s]\r\n", lora_rx_data.rx_buffer  );
}
//------------------------------------------------------------------------------
void lora_push_RxBuffer( char *c)
{
    //lBchar_Poke(&lora_rx_sdata,  (char *)&c);
    if ( lora_rx_data.ptr < LORA_RX_BUFFER_SIZE ) {
        lora_rx_data.rx_buffer[lora_rx_data.ptr++] = *c;
        
    }
}
//------------------------------------------------------------------------------
void lora_print_RxBuffer_stats(void)
{
    
//uint8_t i;
    
 /*    xprintf("lRxBuff_ptr [%d]\r\n", lora_rx_sdata.ptr );
     xprintf("lRxBuff_size[%d]\r\n", lora_rx_sdata.size );
     xprintf("lRxBuff_data[%s]\r\n", lora_rx_sdata.buff );
     xprintf("DEBUG[");
     for (i=0; i < lora_rx_sdata.ptr; i++) {
        xprintf("%c", lora_rx_sdata.buff[i] );
     }
     xprintf("]\r\n");
*/
     xprintf("ptr [%d]\r\n", lora_rx_data.ptr );
     xprintf("[%s]\r\n", lora_rx_data.rx_buffer );
     /*
     xprintf("DEBUG[");
     for (i=0; i < lora_rx_data.ptr; i++) {
        xprintf("%c", lora_rx_data.rx_buffer[i] );
     }
     xprintf("]\r\n");
     
     xprintf( "[%s]\r\n", lora_rx_data.rx_buffer );
     //xprintf( "lRxBuff_data[%s]\r\n", lora_rx_data.rx_buffer );
      */
}
//------------------------------------------------------------------------------
