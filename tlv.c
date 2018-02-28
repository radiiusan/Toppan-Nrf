#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "nrf_delay.h"
#include "basemodule.h"
#include "flash.h"
#include "main.h"
#include "tlv.h"

JACKET_FLASH_ADDCARD addcard_info[ADDCARDMAX];
uint8_t gResponseTLV[MAXAPDUSIZE];
int g_active_cards[ADDCARDMAX];
int g_ix_active_cards;
int16_t primary_index;
MOBILE TLV;

// Transmits the Tlv response to mobile app
void SendTLVResp(uint8_t *buffer, uint16_t len)
{
		printf("\r\nResponse to Mobile : ");
		Rad_UartWriteInHex(buffer, len);
		nrf_delay_ms(500);	
}

// Writes Tlv data into Flash
static int WriteTLVToFlash (void);
static int WriteTLVToFlash (void)
{
    uint32_t   pg_size;
    uint32_t   pg_num;		    
    pg_size = NRF_FICR->CODEPAGESIZE;
    pg_num  = NRF_FICR->CODESIZE - 1;  // Use last page in flash
		
		uint8_t i,ix;
		uint32_t * write_address;
		write_address = (uint32_t *)(pg_size * pg_num);		
		// Erase page:
		flash_page_erase((uint32_t*)write_address);		
		// Write data
		for(ix=0;ix<ADDCARDMAX;ix++)
		{
			for(i = 0; i < 8; i++)
			{
					flash_word_write(write_address, addcard_info[ix].storage.array[i]);
					++write_address;
			}		
		}
	
    return 1;
}

// DeleteAll cards from flash
static void DeleteAllCards(void);
static void DeleteAllCards(void)
{
    uint8_t ix;
    
    g_ix_active_cards=0;
    for(ix=0; ix<ADDCARDMAX; ix++)
				addcard_info[ix].storage.params.state=CARD_DELETED; 	

    WriteTLVToFlash();    
}

// Retrieves the card information from the Flash
static uint8_t GetCardInfo(void);
static uint8_t GetCardInfo(void)
{
    uint32_t   pg_size;
    uint32_t   pg_num;		    
    pg_size = NRF_FICR->CODEPAGESIZE;
    pg_num  = NRF_FICR->CODESIZE - 1;  // Use last page in flash	
		// Start address:
		uint8_t * read_address;
		read_address = (uint8_t *)(pg_size * pg_num);		
		FlashRead(read_address, sizeof(addcard_info), (uint8_t *)&addcard_info);	
	
		uint8_t ix,i=2,j;
		g_ix_active_cards=0;	
		for(ix=0 ;ix<ADDCARDMAX; ix++)
		{ 
			if((addcard_info[ix].storage.params.state == CARD_ACTIVE)||(addcard_info[ix].storage.params.state == CARD_SUSPEND))	
			{
				gResponseTLV[i++]=addcard_info[ix].storage.params.cid[0];
				gResponseTLV[i++]=addcard_info[ix].storage.params.cid[1];
				gResponseTLV[i++]=addcard_info[ix].storage.params.cid[2]; 
				gResponseTLV[i++]=addcard_info[ix].storage.params.state;             
				for(j=0;j<13;j++)
				{
						gResponseTLV[i++]=addcard_info[ix].storage.params.ui[j];                 
				}
				g_active_cards[g_ix_active_cards++]=ix;   				
				printf("\r\nIX %d",ix);
				printf("\r\nCID 	:");
				Rad_UartWriteInHex(addcard_info[ix].storage.params.cid, 3);				
				printf("\rUI 	:");
				Rad_UartWriteInHex(addcard_info[ix].storage.params.ui, 13);		
				printf("\rSTATE   :%d\r\n",addcard_info[ix].storage.params.state);
				printf("DEFAULT :%d\r\n",addcard_info[ix].storage.params.primary);	
				nrf_delay_ms(500);	
			}				
		}
		if(g_ix_active_cards)
			printf("Active cards : %d\r\n", g_ix_active_cards);
		else
			printf("No cards");
   
    gResponseTLV[1]=i-2; 	
    return 1;	
}
	
//Stores the updatecard information into flash memory
static uint8_t UpdateCardInfo(void);
static uint8_t UpdateCardInfo(void)
{
    uint8_t ix,i,j=0;

		for(ix=0; ix<ADDCARDMAX; ix++)
		{
			if(ix == TLV.value[2])	
			{
				addcard_info[ix].storage.params.cid[0]=TLV.value[j++];
				addcard_info[ix].storage.params.cid[1]=TLV.value[j++];
				addcard_info[ix].storage.params.cid[2]=TLV.value[j++];
				if(TLV.len > 5)
				{
						for(i=0;i<13;i++)
						{
								addcard_info[ix].storage.params.ui[i]=TLV.value[j++];
						}     
				}
				addcard_info[ix].storage.params.state=TLV.value[j++];    
//				addcard_info[ix].storage.params.primary=TLV.value[j++]; value not send in the packet
				printf("\rIX %d",ix);
				printf("\r\nCID 	:");
				Rad_UartWriteInHex(addcard_info[ix].storage.params.cid, 3);				
				printf("\rUI 	:");
				Rad_UartWriteInHex(addcard_info[ix].storage.params.ui, 13);		
				printf("\rSTATE   :%d\r\n",addcard_info[ix].storage.params.state);
				printf("DEFAULT :%d\r\n",addcard_info[ix].storage.params.primary);	
				break;	
			}		
			else
			{
				printf("Cid not found!\r\n"); 
				// send response to mobile
				return 0;				
			}
		}

    if(WriteTLVToFlash())        
    {
        gResponseTLV[1]=0x03;           
        gResponseTLV[2]=addcard_info[ix].storage.params.cid[0]; 
        gResponseTLV[3]=addcard_info[ix].storage.params.cid[1];         
        gResponseTLV[4]=addcard_info[ix].storage.params.cid[2];  
        return 1; 
    }
    else
    {
        return 0;
    }
}

//Stores the addcard information into flash memory
static uint8_t StoreCardInfo(void);
static uint8_t StoreCardInfo(void)
{
    uint8_t ix,i,j=0,incid;
   
		incid = TLV.value[2];
		for(ix=0; ix<ADDCARDMAX; ix++)
		{
			if(incid == addcard_info[ix].storage.params.cid[2])	
			{
				printf("Duplicate CID\r\n"); 
				// send response to mobile
				return 0;	
			}				
		}
		for(ix=0; ix<ADDCARDMAX; ix++)
		{
			if((addcard_info[ix].storage.params.state == CARD_EMPTY)||(addcard_info[ix].storage.params.state == CARD_DELETED))	
			{
				break;	
			}				
		}
		if(ix==ADDCARDMAX)
		{
			printf("Max Cards Reached, Please delete and add\n"); 
			// send response to mobile
			return 0;			
		}		
		else
		{
			addcard_info[ix].storage.params.cid[0]=TLV.value[j++];
			addcard_info[ix].storage.params.cid[1]=TLV.value[j++];
			addcard_info[ix].storage.params.cid[2]=TLV.value[j++];
			if(TLV.len > 5)
			{
					for(i=0;i<13;i++)
					{
							addcard_info[ix].storage.params.ui[i]=TLV.value[j++];
					}     
			}
			addcard_info[ix].storage.params.state=TLV.value[j++];    
			addcard_info[ix].storage.params.primary=TLV.value[j++]; 
			printf("\rIX %d",ix);
			printf("\r\nCID 	:");
			Rad_UartWriteInHex(addcard_info[ix].storage.params.cid, 3);				
			printf("\rUI 	:");
			Rad_UartWriteInHex(addcard_info[ix].storage.params.ui, 13);		
			printf("\rSTATE   :%d\r\n",addcard_info[ix].storage.params.state);
			printf("DEFAULT :%d\r\n",addcard_info[ix].storage.params.primary);

			if(WriteTLVToFlash())        
			{
					gResponseTLV[1]=0x03;           
					gResponseTLV[2]=addcard_info[ix].storage.params.cid[0]; 
					gResponseTLV[3]=addcard_info[ix].storage.params.cid[1];         
					gResponseTLV[4]=addcard_info[ix].storage.params.cid[2];  
					return 1; 
			}
			else
			{
					return 0;
			}
		}
}

// According to the Tlv tag performs the appropriate processing.
static void ProcessTLV(void);
static void ProcessTLV(void)
{
    uint8_t cmd;
    
    cmd=TLV.tag;
    gResponseTLV[0]=cmd;
    gResponseTLV[1]=0x02;      
    gResponseTLV[2]=0x00; 
    gResponseTLV[3]=0x00; 
    gResponseTLV[4]=0x00;    

    switch(cmd)
    {
    case CREATECARD: 
				StoreCardInfo();
        SendTLVResp(gResponseTLV, gResponseTLV[1]+2);   
    break;	
		
    case UPDATECARD:			
    case DELETECARD:			
				UpdateCardInfo();
        SendTLVResp(gResponseTLV, gResponseTLV[1]+2);   
    break;		
		
    case GETCARD_MOBILE:
        GetCardInfo();           
        SendTLVResp(gResponseTLV, gResponseTLV[1]+2);  
    break;	
		
    case PAIR:
        gResponseTLV[2]=0x90; 
        gResponseTLV[3]=0x00;            
        SendTLVResp(gResponseTLV, gResponseTLV[1]+2);  
    break;
		
    case SEND_APDU:
#ifdef ENABLE_DEDUG_PRINTS         
/*        Rad_PrintStr("\r\n TLV Data \t");   
        uint8 i;
        for(i=0; i<TLV.len; ++i){
            Rad_PrintHexVal(TLV.value[i]);   
            Rad_PrintStr("\t");
        }
        Rad_PrintStr("\r\n");*/
#endif
        gResponseTLV[2] = TLV.value[0]; 
        gResponseTLV[3] = TLV.value[1]; 
        gResponseTLV[4] = TLV.value[2];  
        SendTLVResp(gResponseTLV, gResponseTLV[1]+2);   
    break;
    
    case LOCKCARD:
        gResponseTLV[2]=0x90; 
        gResponseTLV[3]=0x00;            
        SendTLVResp(gResponseTLV, gResponseTLV[1]+2);  
    break;  

    case REACTIVATE:
        gResponseTLV[2]=0x90;  
        gResponseTLV[3]=0x00;          
        SendTLVResp(gResponseTLV, gResponseTLV[1]+2);      
    break;  
    
    case DELETEALLCARDS:
				DeleteAllCards();
        gResponseTLV[2]=0x90; 
        gResponseTLV[3]=0x00;            
        SendTLVResp(gResponseTLV, gResponseTLV[1]+2);             
    break; 
    }
}

// Forms the TLV packet from the Ble data recieved and calls ProcessTLV
void FormTLV(uint8_t* BLEPkt, uint16_t size)
{
		static bool begining = TRUE;
		static uint8_t len, TLVinx = 0;
    int readinx = 0;
    uint16_t count = 0;

		Rad_UartWriteInHex(BLEPkt, size);
		nrf_delay_ms(500);

    if(begining)
    {
        begining = FALSE;
        TLV.tag = BLEPkt[readinx++];
        count++;  
				count++; readinx++;
        TLV.len = BLEPkt[readinx++];
        count++;
        len = TLV.len;
				if (TLV.tag == FOTA_DATA)
					len &= 0x7F;
    }  
    while (count<size) {
		TLV.value[TLVinx++] = BLEPkt[readinx++];
        count++;
				len--;
		}
    if(len==0)
    {
        begining = TRUE;
        TLVinx = 0;
/*				printf("\nTLV \t"); 
				Rad_PrintHexVal(TLV.tag);
				printf("\t");    
				Rad_PrintHexVal(TLV.len);
				printf("\n");	*/		
				ProcessTLV();
//		TlvFormed = TRUE;
    }	
}
