#ifndef __TLV_H__
#define __TLV_H__

#define TRUE 1
#define FALSE 0
#define MAX_PAYLOAD 247
#define MAXTRACKDATA 228
#define MAXVALSIZE 248
#define MAXPKTSIZE 255
#define MAXAPDUSIZE 254
#define HEADERSIZE 5
#define PROLOG_SIZE 3
#define EPILOG_SIZE 1
#define ADDCARDMAX 4

//JACKET GENERATED TAGS
#define SELECT                  0x00
#define GETVERSION_JACKET_FW    0x01
#define GETCARD_UI              0x03
#define GETCARD_PSE_PPSE        0x04
#define GETCARD_DMS             0x05
#define VALIDATE0               0x06
#define VALIDATE1               0x07
#define VALIDATE2               0x08
#define PUT_TEMPLATE            0x09
#define CAPLOAD                 0x0B
#define PUTDATA_JACKET          0x40
#define PUTDATA_CARD            0x41
#define BAUDRATE                0x45
#define EXTENDED                0x46

//MOBILE GENERATED TAGS
#define GETCID                  0x02
#define GETSTATE                0x20
#define PAIR                    0x21
#define VERIFYPIN               0x22
#define CHANGEPIN               0x23
#define CREATECARD              0x24
#define UPDATECARD              0x25
#define DELETECARD              0x26
#define GETCARD_MOBILE          0x27
#define UNBLOCKPIN              0x28
#define LOCKCARD                0x05
#define USERPREFERENCE          0x34
#define REGISTERFP              0x35
#define SETSTATE                0x36
#define MCUADDCARD1             0x37 
#define MCUADDCARD2             0x38 
#define MCUDELETE1              0x39 
#define MCUDELETE2              0x40
#define GETDATA_JACKET          0x42
#define GETDATA_CARD            0x43

#define BLE_CONNECT             0xE0
#define FOTA_START              0xF0
#define FOTA_DATA               0xF1
#define FOTA_END                0xF2
#define REACTIVATE              0xFD
#define DELETEALLCARDS          0xFE
#define FOTA_ACK                0x00 
#define FOTA_NACK               0x01
#define SEND_APDU               0xE1

/*  CARD STATUS VALUES  */
#define CARD_EMPTY   		    		0xFF  /*Card not active yet*/
#define CARD_INACTIVE        		0x01  /*Card activation initiated*/
#define CARD_ACTIVE         		0x02  /*Card has been activated or resumed*/
#define CARD_DELETED           	0x03  /*Card has been deleted*/
#define CARD_SUSPEND			    	0x04  /*Card has been marked for suspend*/


/*  JACKET STATE VALUES  */
#define JACKETSTATE_OPERATIONAL		0x00
#define JACKETSTATE_INACTIVE    	0x01
#define JACKETSTATE_PINCHANGEREQ	0x02
#define JACKETSTATE_FPFIRSTSCAN		0x03
#define JACKETSTATE_FPSSUCCESS		0x05
#define JACKETSTATE_DEFAULTVALUE	0xFF


typedef struct jacket_flash_addcard
{
		union 
		{
				uint32_t array[8];
				struct 
				{
						uint8_t  cid[3];
						uint8_t  ui[13];    
						uint8_t  state;    
						uint8_t  primary;
						uint8_t  pad[14]; 
				}params;	 //32 bytes 
		}storage;
}JACKET_FLASH_ADDCARD;

typedef struct mobile {
	unsigned char tag;
	uint16_t len;
  unsigned char value[MAXVALSIZE];
} MOBILE;

typedef struct apdu {
	unsigned short len;
	unsigned char byteimage[MAXAPDUSIZE + PROLOG_SIZE + EPILOG_SIZE];
} APDU;

void FormTLV(uint8_t* BLEPkt, uint16_t size);
void ProcessTLV(void);

#endif /* __TLV_H__ */
