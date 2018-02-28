#ifndef __FLASH_H__
#define __FLASH_H__

void flash_word_write(uint32_t * address, uint32_t value);
void flash_page_erase(uint32_t * page_address);
uint32_t FlashWrite(uint32_t address, uint16_t length, uint8_t *writeBuf);
uint32_t FlashRead(uint8_t *address, uint16_t length, uint8_t *readBuf);
void Flash_Test(void);
void FormTLV(uint8_t* BLEPkt, uint16_t size);
#endif /* __FLASH_H__ */

