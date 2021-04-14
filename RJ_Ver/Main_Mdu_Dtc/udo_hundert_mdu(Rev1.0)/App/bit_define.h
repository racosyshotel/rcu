/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/


#include "stm32f2xx.h"
#include "string.h"
#include "stdio.h"

typedef unsigned char 	uchar;
typedef unsigned short 	ushort;
typedef unsigned int  	uint;
typedef unsigned long  	ulong;

typedef unsigned long long u64;
typedef signed long long s64;

typedef union {
  vu8 DATA;
  struct {
          volatile unsigned b0:1;
          volatile unsigned b1:1;
          volatile unsigned b2:1;
          volatile unsigned b3:1;
          volatile unsigned b4:1;
          volatile unsigned b5:1;
          volatile unsigned b6:1;
          volatile unsigned b7:1;
  } Bit;
} SFR8;

typedef union {
  vu16 DATA;
  struct {
          volatile unsigned b0:1;
          volatile unsigned b1:1;
          volatile unsigned b2:1;
          volatile unsigned b3:1;
          volatile unsigned b4:1;
          volatile unsigned b5:1;
          volatile unsigned b6:1;
          volatile unsigned b7:1;
          volatile unsigned b8:1;
          volatile unsigned b9:1;
          volatile unsigned b10:1;
          volatile unsigned b11:1;
          volatile unsigned b12:1;
          volatile unsigned b13:1;
          volatile unsigned b14:1;
          volatile unsigned b15:1;
  } Bit;
} SFR16;

typedef union {
  vu32 DATA;
  struct {
          volatile unsigned b0:1; 
          volatile unsigned b1:1; 
          volatile unsigned b2:1; 
          volatile unsigned b3:1; 
          volatile unsigned b4:1; 
          volatile unsigned b5:1; 
          volatile unsigned b6:1; 
          volatile unsigned b7:1; 
          volatile unsigned b8:1; 
          volatile unsigned b9:1; 
          volatile unsigned b10:1;
          volatile unsigned b11:1;
          volatile unsigned b12:1;
          volatile unsigned b13:1;
          volatile unsigned b14:1;
          volatile unsigned b15:1;
          volatile unsigned b16:1; 
          volatile unsigned b17:1; 
          volatile unsigned b18:1; 
          volatile unsigned b19:1; 
          volatile unsigned b20:1; 
          volatile unsigned b21:1; 
          volatile unsigned b22:1; 
          volatile unsigned b23:1; 
          volatile unsigned b24:1; 
          volatile unsigned b25:1; 
          volatile unsigned b26:1;
          volatile unsigned b27:1;
          volatile unsigned b28:1;
          volatile unsigned b29:1;
          volatile unsigned b30:1;
          volatile unsigned b31:1;
  } Bit;
} SFR32;

typedef union {
  u32 DATA;
  struct {
          unsigned b0:1; 
          unsigned b1:1; 
          unsigned b2:1; 
          unsigned b3:1; 
          unsigned b4:1; 
          unsigned b5:1; 
          unsigned b6:1; 
          unsigned b7:1; 
          unsigned b8:1; 
          unsigned b9:1; 
          unsigned b10:1;
          unsigned b11:1;
          unsigned b12:1;
          unsigned b13:1;
          unsigned b14:1;
          unsigned b15:1;
          unsigned b16:1; 
          unsigned b17:1; 
          unsigned b18:1; 
          unsigned b19:1; 
          unsigned b20:1; 
          unsigned b21:1; 
          unsigned b22:1; 
          unsigned b23:1; 
          unsigned b24:1; 
          unsigned b25:1; 
          unsigned b26:1;
          unsigned b27:1;
          unsigned b28:1;
          unsigned b29:1;
          unsigned b30:1;
          unsigned b31:1;
  } Bit;
} FLAG;

/***********************************************************************************************/

typedef union {
	u32 dword;
	struct {
		unsigned  _tm0fg	:1;	
		unsigned  _tm1fg	:1;	
		unsigned  _tm2fg	:1;	
		unsigned  _tm3fg	:1;	
		unsigned  _tm4fg	:1;	
		unsigned  _tm5fg	:1;	
		unsigned  _tm6fg	:1;	
		unsigned  _tm7fg	:1;	

		unsigned  _tm8fg	:1;	
		unsigned  _tm9fg	:1;	
		unsigned  _tm10fg	:1;	
		unsigned  _tm11fg	:1;	
		unsigned  _tm12fg	:1;	
		unsigned  _tm13fg	:1;	
		unsigned  _tm14fg	:1;	
		unsigned  _tm15fg	:1;	
	} s;
}__tm;
	
#define tm0fg	tm.s._tm0fg
#define tm1fg	tm.s._tm1fg
#define tm2fg	tm.s._tm2fg
#define tm3fg	tm.s._tm3fg
#define tm4fg	tm.s._tm4fg
#define tm5fg	tm.s._tm5fg
#define tm6fg	tm.s._tm6fg
#define tm7fg	tm.s._tm7fg

#define tm8fg	tm.s._tm8fg
#define tm9fg	tm.s._tm9fg
#define tm10fg	tm.s._tm10fg
#define tm11fg	tm.s._tm11fg
#define tm12fg	tm.s._tm12fg
#define tm13fg	tm.s._tm13fg
#define tm14fg	tm.s._tm14fg
#define tm15fg	tm.s._tm15fg

//=======================================================================
typedef union {
	u8 byte;
	struct {
		unsigned  b0	:1;	
		unsigned  b1	:1;	
		unsigned  b2	:1;	
		unsigned  b3	:1;	
		unsigned  b4	:1;	
		unsigned  b5	:1;	
		unsigned  b6	:1;	
		unsigned  b7	:1;
	} s;
	struct {
		unsigned  b0	:1;	
		unsigned  b1	:1;	
		unsigned  b2	:1;	
		unsigned  b3	:1;	
		unsigned  b4	:1;	
		unsigned  b5	:1;	
		unsigned  b6	:1;	
		unsigned  b7	:1;
	} bit;
}__bits; 

typedef union {
	u16 word;
	struct {
		volatile unsigned b0:1;
		volatile unsigned b1:1;
		volatile unsigned b2:1;
		volatile unsigned b3:1;
		volatile unsigned b4:1;
		volatile unsigned b5:1;
		volatile unsigned b6:1;
		volatile unsigned b7:1;
		volatile unsigned b8:1;
		volatile unsigned b9:1;
		volatile unsigned b10:1;
		volatile unsigned b11:1;
		volatile unsigned b12:1;
		volatile unsigned b13:1;
		volatile unsigned b14:1;
		volatile unsigned b15:1;
	} s;
}__wbits;

typedef union {
	u32 dword;
	struct {
		unsigned b0:1; 
		unsigned b1:1; 
		unsigned b2:1; 
		unsigned b3:1; 
		unsigned b4:1; 
		unsigned b5:1; 
		unsigned b6:1; 
		unsigned b7:1; 
		unsigned b8:1; 
		unsigned b9:1; 
		unsigned b10:1;
		unsigned b11:1;
		unsigned b12:1;
		unsigned b13:1;
		unsigned b14:1;
		unsigned b15:1;
		unsigned b16:1; 
		unsigned b17:1; 
		unsigned b18:1; 
		unsigned b19:1; 
		unsigned b20:1; 
		unsigned b21:1; 
		unsigned b22:1; 
		unsigned b23:1; 
		unsigned b24:1; 
		unsigned b25:1; 
		unsigned b26:1;
		unsigned b27:1;
		unsigned b28:1;
		unsigned b29:1;
		unsigned b30:1;
		unsigned b31:1;
	} s;
	struct {
		unsigned char byte0; 
		unsigned char byte1;
		unsigned char byte2; 
		unsigned char byte3; 
	} b;
} __dwbits;

#define mm __xdprintf(xdebug)
//#define mm mmd
#define mmd _xdprintf(xdebug)


/********************************Gpio Group bit contorl **************************************************/
#define GpioA	((SFR16*)(GPIOA_BASE+0x14))
#define GpioB	((SFR16*)(GPIOB_BASE+0x14))
#define GpioC	((SFR16*)(GPIOC_BASE+0x14))
#define GpioD	((SFR16*)(GPIOD_BASE+0x14))
#define GpioE	((SFR16*)(GPIOE_BASE+0x14))
#define GpioF	((SFR16*)(GPIOF_BASE+0x14))
#define GpioG	((SFR16*)(GPIOG_BASE+0x14))
#define GpioH	((SFR16*)(GPIOH_BASE+0x14))
#define GpioI	((SFR16*)(GPIOI_BASE+0x14))

/******************* (C) COPYRIGHT 2010 INSEM Inc ***************************************END OF FILE****/

