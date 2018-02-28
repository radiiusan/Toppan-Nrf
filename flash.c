#include "nrf.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "basemodule.h"


// Function for writing a page in flash with data.
void flash_word_write(uint32_t * address, uint32_t value)
{
    // Turn on flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    *address = value;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // Turn off flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}

// Function for erasing a page in flash.
void flash_page_erase(uint32_t * page_address)
{
    // Turn on flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // Erase page:
    NRF_NVMC->ERASEPAGE = (uint32_t)page_address;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // Turn off flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}
#if 0
uint32_t FlashWrite(uint32_t address, uint16_t length, uint8_t *writeBuf)
{
		// Erase page:
		flash_page_erase((uint32_t *)address);
		
		uint32_t value = 0x00000000;
	
		value = *(uint32_t *)writeBuf;
		printf("%x %x %x %x %x\r\n",*writeBuf, *(writeBuf+1),*(writeBuf+2),*(writeBuf+3), value );
 		value = ( value >> 24 ) | (( value << 8) & 0x00ff0000 )| ((value >> 8) & 0x0000ff00) | ( value << 24)  ;
		printf("%x ",value);

		flash_word_write(&address, value);
		printf(" Flash Write Done \n\r");
		nrf_delay_ms(500);
		
    return NRF_SUCCESS;
}
#endif

// Function for read length bytes of values from the address into readBuf
uint32_t FlashRead(uint8_t *address, uint16_t length, uint8_t *readBuf)
{ 
		uint8_t *p;
		uint8_t i;
		p = address;
		for(i = 0; i < length ; i++) 
				*readBuf++ = *p++;
	
		return NRF_SUCCESS;
}

// Flash api for testing the write and read functionalities
void Flash_Test(void)
{
		nrf_delay_ms(500);
    uint32_t   pg_size;
    uint32_t   pg_num;		    
    pg_size = NRF_FICR->CODEPAGESIZE;
    pg_num  = NRF_FICR->CODESIZE - 1;  // Use last page in flash
		
		uint32_t * write_address;
		uint32_t byte1 = 0x00FF00FF, byte2 = 0xFF00FF00;
		write_address = (uint32_t *)(pg_size * pg_num);		
		// Erase page:
		flash_page_erase((uint32_t*)write_address);		
		// Write data
		flash_word_write(write_address, byte1);
		write_address++;
		flash_word_write(write_address, byte2);	
		printf("Flash write data\r\n%x%x\n\r",byte1,byte2);
	
		// Start address:
		uint8_t * read_address;
	  uint8_t rbuff[8];
		read_address = (uint8_t *)(pg_size * pg_num);	
		FlashRead(read_address, sizeof(rbuff), (uint8_t*)rbuff);		
    printf("Flash read data\r\n");
		Rad_UartWriteInHex(rbuff, 8);
		nrf_delay_ms(500);					
}
