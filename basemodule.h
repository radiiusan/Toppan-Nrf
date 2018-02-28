#ifndef __BASE_MODULES_H__
#define __BASE_MODULES_H__

#include <stdint.h>
#include "main.h"

void Led_Test(void);
void Console_Test(void);

uint8_t Rad_PrintHexVal(uint32_t value);
uint16_t  Rad_UartWriteInHex(unsigned char *dbuff, uint16_t len);

#endif /* __BASE_MODULES_H__*/
