
#include "pines.h"

// ---------------------------------------------------------------
void ACGEN_init(void)
{
	// Configura los pines como output
    
	AC0_PORT.DIR |= AC0_PIN_bm;	
	CLEAR_AC0();

	AC1_PORT.DIR |= AC1_PIN_bm;	
	CLEAR_AC1();
  
}
// ---------------------------------------------------------------
