#include "boards.h"
#include "nrf_delay.h"

const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;	
// Function to On and Off each of the four leds sequenciallly
void Led_Test(void)
{
		//LED's
		// Configure LED-pins as outputs.
  	 LEDS_CONFIGURE(LEDS_MASK);
		// Toggle LEDs 1-4
		LEDS_ON(1 << leds_list[0]);
		nrf_delay_ms(500);	
		LEDS_OFF(1 << leds_list[0]);
	  LEDS_ON(1 << leds_list[1]);
		nrf_delay_ms(500);
		LEDS_OFF(1 << leds_list[1]);	
		LEDS_ON(1 << leds_list[2]);
		nrf_delay_ms(500);
		LEDS_OFF(1 << leds_list[2]);	
		LEDS_ON(1 << leds_list[3]);
		nrf_delay_ms(500);
		LEDS_OFF(1 << leds_list[3]);		
		nrf_delay_ms(500);
}
