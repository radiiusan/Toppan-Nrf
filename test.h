#ifndef __TLV_H__
#define __TLV_H__

typedef enum  
{
	EXIT = '0',
	FLASH,
	LED,
  ADDCARD,
	GETCARDS,
  SUSPEND,
  RESUME,
  DELETE,
	DELETEALL,
	MENU,
  MAX_COMMANDS
} COMMANDS;


#endif /* __TLV_H__ */
