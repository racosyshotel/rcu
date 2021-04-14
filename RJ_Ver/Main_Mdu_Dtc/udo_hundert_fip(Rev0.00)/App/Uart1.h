/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/
#ifndef __UART1_H__
#define __UART1_H__


//=======================================================================

void UART1_control_proc(void);
void UART1_initial(void);

u8 u1_byteload(void);
void __lineclr(u8 len);
void screen_clear(void);
void line_clr(u8 col);
void __printf(char *str);
void __xcprintf(const char *str);
void _xdprintf(char *str);
void __xdprintf(char *str);
void factory_initial(void);
void hyper_clear(void) ;
void __linefeed(void);
//=======================================================================


typedef enum {
	black =0x30,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	white
}_foreground;

typedef enum {
	_black =0x40,
	_red,
	_green,
	_yellow,
	_blue,
	_magenta,
	_cyan,
	_white
}_background;

typedef enum {
	NUL,
	SOH,
	STX,
	ETX,
	EOT,
	ENQ,
	ACK,
	BEL,
	BS,
	TAB,
	LF,
	VT,
	FF,
	CR,
	SO,
	SI,
	DLE,
	DC1,
	DC2,
	DC3,
	DC4,
	NAK,
	SYN,
	ETB,
	XCAN,
	EM,
	SUB,
	ESC,
	FS,
	GS,
	RS,
	US,
	SPACE,
}__control_code;

#define debug_sp1

//=======================================================================
//=======================================================================
typedef struct {
	u16 ldad;
	u16 ldad1;
	u16 svad;
	u8 buf[64];
}__ring;

typedef struct {
	u16 ldad;
	u16 svad;
	u8 buf[2*1024];
}__trx;

typedef struct {
	u8 slen;
	u8 msel;
	u8 menumax;
	u8 input[20];
}__zin;

typedef enum {
	_password =0x31,
	_beep,
	_debug,
	_factory_initial,
	_title_input,
	_memorydump,	
}_select;

//=======================================================================

#ifdef __UART1_C__
#define u1EX 
u1EX char xdebug[349];

const char* menu[] = {
	"1. Password change\r\n",
	"2. Beep sound\r\n",
	"3. Debug mode\r\n",
	"4. Factory Initial\r\n",
	"5. Title Input\r\n",
	"6. Memory Dump\r\n",

	".",			// end point
};
	
const u8 selno[] = {
	_password,
	_beep,
	_debug,
	_factory_initial,
	_title_input,
	_memorydump,

};

const char* Lname[] = {
	"대표이사",
};
const char* Gname[] = {
	"정문앞",
};
	
extern u16 wdtcnt;

#else
#define u1EX extern
u1EX char xdebug[349];

#endif

u1EX __trx dtx;
u1EX __ring ring;
u1EX u8 spdata, rcnt, rdinx, esctc;
u1EX __zin zin;
u1EX char dspbuf[32];

//=======================================================================
u1EX __wbits uart1;
#define fsend_f		  uart1.s.b0
#define u1_init_f		uart1.s.b1
#define firstdp_f 	uart1.s.b2
#define firstdp1_f 	uart1.s.b3
#define wdtdis_f	  uart1.s.b4
#define name_f		  uart1.s.b5
#define upc_f		    uart1.s.b6
#define downc_f	    uart1.s.b7
#define modec_f 	  uart1.s.b8
#define Ntitle_f	  uart1.s.b9

#endif

//=======================================================================
//=======================================================================

