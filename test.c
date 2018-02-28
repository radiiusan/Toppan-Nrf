#include <string.h>
#include "basemodule.h"
#include "nrf_delay.h"
#include "flash.h"
#include "app_uart.h"
#include "test.h"
#include "main.h"
#include "tlv.h"

// Display the menu options on Uart
static void display_menu(void);
static void display_menu(void)
{
    printf("\r\n---------CONSOLE TEST---------\n\r");
    printf(" x - Exit test\n\r");		
    printf(" 1 - Flash test\n\r");	
    printf(" 2 - Led test\n\r");		
    printf(" 3 - Add Card\n\r");			
    printf(" 4 - Get Cards\n\r");		
    printf(" 5 - Suspend\n\r");	
    printf(" 6 - Resume\n\r");		
    printf(" 7 - Delete\n\r");	
    printf(" 8 - DeleteAll\n\r");		
	  printf(" 9 - Menu\n\r");
		nrf_delay_ms(500);		
}

// Accepts the character from uart accroding to menu options displayed
// As per the input from the user performs the corresponding test
void Console_Test(void)
{
		uint32_t err_code;
    uint8_t  cmd;	
	
		display_menu();
	
		while(cmd != 'x')
		{
				if(cmd != 'x')
					 printf("\r\n\r\nEnter value to test ");
				// Read char from uart
				do
				{
					 err_code = app_uart_get(&cmd);
				}
				while(err_code == NRF_ERROR_NOT_FOUND);

				// Read from flash the last written data and send it back:
				printf("%c \n\r\n\r", cmd);
				switch (cmd)
				{ 
					case FLASH:
						Flash_Test();
					break;
					
					case LED:
						Led_Test();
						printf("Led Test Done");						
					break;
					
					case ADDCARD:
					{						
						uint8_t cid = 0;
						uint8_t buff[]= {0x24,0x0,0x12,0x1,0x1,0x0,0x44,0x46,0x53,0x34,0x31,0x31,0x31,0x31,0x32,0x30,0x30,0x31,0x30,0x2,0x01};
						printf("\r\nEnter cid to add 0,1,2,3 ");
						// Read char from uart, and write it to flash:
						do
						{
							 err_code = app_uart_get(&cid);
						}
						while(err_code == NRF_ERROR_NOT_FOUND);

						// Read from flash the last written data and send it back:
						printf("%c \n\r\n\r", cid);	
						switch (cid)
						{
							case '0':
								buff[5] = 0x0;
								buff[10] = 0x30;
								buff[11] = 0x30;
								buff[12] = 0x30;
								buff[13] = 0x30;							
							break;
							case '1':
								buff[5] = 0x1;
								buff[10] = 0x31;
								buff[11] = 0x31;
								buff[12] = 0x31;
								buff[13] = 0x31;							
							break;
							case '2':
								buff[5] = 0x2;
								buff[10] = 0x32;
								buff[11] = 0x32;
								buff[12] = 0x32;
								buff[13] = 0x32;							
							break;
							case '3':
								buff[5] = 0x3;
								buff[10] = 0x33;
								buff[11] = 0x33;
								buff[12] = 0x33;
								buff[13] = 0x33;							
							break;							
						}
						FormTLV((uint8_t *)buff, sizeof(buff));	
						printf("AddCard Done");						
					}
					break;				
					
					case GETCARDS:
					{
						uint8_t buff[]= {0x27,0x0};
						FormTLV((uint8_t *)buff, sizeof(buff));	
						printf("GetCards Done");							
					}
					break;
					
					case SUSPEND:
					{
						uint8_t buff[]= {0x25,0x0,0x04,0x1,0x1,0x0,0x04};
						FormTLV((uint8_t *)buff, sizeof(buff));	
						printf("Suspend Done");							
					}
					break;		

					case RESUME:
					{
						uint8_t buff[]= {0x25,0x0,0x04,0x1,0x1,0x0,0x02};
						FormTLV((uint8_t *)buff, sizeof(buff));	
						printf("Resume Done");							
					}
					break;	
					
					case DELETE:
					{
						uint8_t buff[]= {0x26,0x0,0x04,0x1,0x1,0x0,03};
						FormTLV((uint8_t *)buff, sizeof(buff));	
						printf("Delete Done");							
					}
					break;				

					case DELETEALL:
					{
						uint8_t buff[]= {0xFE,0x0,0x00};
						FormTLV((uint8_t *)buff, sizeof(buff));	
						printf("Delete All Done");							
					}
					break;		
					
					case MENU:
					{
						display_menu();
					}
					break;					
				}
		}	
}

