
#include "lora.h"


// ---------------------------------------------------------------
void LORA_init(void)
{
    LORA_RTS_CONFIG();
    LORA_CTS_CONFIG();
    LORA_RESET_CONFIG();
}
// ---------------------------------------------------------------
