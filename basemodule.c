#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "nrf.h"
#include "main.h"

// Converts in Hexadecimal value and sends to uart
uint8_t Rad_PrintHexVal(uint32_t value)
{
    #define BUFFER_SIZE1 16          	// Buffer size required to hold maximum value 
    
    uint8_t  i = BUFFER_SIZE1;     		// Loop counter 
#ifdef ENABLE_DEDUG_PRINTS
		uint32_t remainder = value;   		// Remaining value to send 
    int temp;
    char   buffer[BUFFER_SIZE1]; 			// Buffer for ASCII string 

		// Ensure the string is correctly terminated 
    buffer[--i] = '\0';
    // Loop at least once and until the whole value has been converted 
    do
    {
        // Convert the unit value into ASCII and store in the buffer 
        temp = (remainder % 16);
        
        if( temp < 10)
           temp =temp + 48;
        else
          temp = temp + 55;

        buffer[--i] = temp;
        // Shift the value right one decimal 
        remainder /= 16;
    } while (remainder > 0);
    
    buffer[--i] = 'x';
    buffer[--i] = '0';

    // Send the string to the UART 
		printf("%s",buffer+i);
#endif  
       
    /* Return length of ASCII string sent to UART */
    return (BUFFER_SIZE1 - 1) - i;
}

//Sends a Hexdecimal value of a string to uart
uint16_t  Rad_UartWriteInHex(unsigned char *dbuff, uint16_t len)
{
    uint16_t ix=0;
#ifdef ENABLE_DEDUG_PRINTS      
    for(ix=0; ix<len; ix++)
    {
        if(ix && (ix%16)==0) printf("\r\n");
        Rad_PrintHexVal(dbuff[ix]);
        printf(" ");
    }
    printf("\r\n");
#endif       
    return ix;
}
