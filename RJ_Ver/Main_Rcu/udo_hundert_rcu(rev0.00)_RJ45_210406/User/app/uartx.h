/*=====================(C) COPYRIGHT 2008 Insem Inc.=========================
program 		: HDPVR FRONT PANEL 
processor	: STM32F101xx
compiler		: IAR 4.41A Compiler 
program BY 	: YN.Kim
date 			: 2008.	  .
copy right 	: Insem Inc.
=======================================================================*/
#ifndef __UARTx__
#define __UARTx__

void PC2_control_proc(void);
void Uartx_initial(void);
void pc_sframe_proc(void);
u8 pcbyte_loading(void);

#define debug_mode_pc
#define pc_smaxsize	(256)

//=======================================================================
#define __USART2__

#if defined (__USART2__)
#define Serial_Port 					USART2
#define Serial_Port_IRQChannel	USART2_IRQChannel
#elif defined (__USART3__)
#define Serial_Port 					USART3
#define Serial_Port_IRQChannel	USART3_IRQChannel
#elif defined (__USART4__)
#define Serial_Port 					UART4
#define Serial_Port_IRQChannel	UART4_IRQChannel
#elif defined (__USART5__)
#define Serial_Port 					UART5
#define Serial_Port_IRQChannel	UART5_IRQChannel
#else
  #error "Serial POrt가 정의되지 않았습니다." 
#endif


#define eth_reset GpioD->Bit.b13
#define hwtrig GpioD->Bit.b15

//=======================================================================
typedef struct {
	u16 ldad;
	u16 svad;
	u8 buf[256];
}__pcring;

typedef struct {
	u8 size[pc_smaxsize];
}_pcsize;

typedef struct {
	u8 stx[2];
	u8 cmd;
	u16 len;
	u8 crc[2];
	u8 etx[2];
}_penq;

typedef struct {
	u8 stx[2];
	u8 ans;
	u16 len;
	u8 crc[2];
	u8 etx[2];
}_pans;
typedef struct {			// hyper
	u8 stx;
	u8 cmd[2];
	u8 sel;
	u8 index;
	u8 etx;
}_index;

typedef struct {
	u8 stx;
	u8 rsp;
	u8 etx;
}_rsp;

#define default_crcmake (sizeof(Pctx.scom.cmd)+sizeof(Pctx.scom.len)+sizeof(Pctx.scom.index))
#define default_sendlen (sizeof(u16) +default_crcmake +sizeof(u16)+sizeof(u16))

#pragma pack(1)
typedef struct {
	u8 stx[2];
	u8 cmd;
	u16 len;
	s16 index;
	union {
		u8 buf[pc_smaxsize];
	}s;
}_pccom;

#pragma pack(1)
typedef union {
	_pcsize s;
	_index i;
	_rsp rsp;

	_penq enq;						// tx cmd
	_pccom scom;
	_pans ans;						// rx cmd

	u8 buf[sizeof(_pcsize)];
}__pctrx;

//=======================================================================

#ifdef __UARTx_H__
#define uxEX

const char* eth_wcmd[] = {
	"<WX.0",
	"<WN.1",
	"<WD.2",
	"<WI.3",
	"<WS.4",
	"<WG.5",
	"<WP.6",
	"<WB.7",
	"<WM.8",
//	"<WT1>",
//	"<WC0>",
//	"<WR>",

	".",			// end point
};

const char* eth_rcmd[] = {
	"<RX>",			//0
	"<RN>",
	"<RD>",
	"<RI>",
	"<RS>",
	"<RG>",
	"<RP>",			
	"<RB>",
	"<RM>",
	"<Ra>",

	".",				// end point
};

#else
#define uxEX extern 

#endif

uxEX __pcring pcring;
uxEX __pctrx Pctx, Pcrx, bkPctx;
uxEX u8 pcdata;
uxEX u16 Pctx_max;

//=======================================================================
uxEX __bits uartx;
#define pcsend_f uartx.s.b0
#define pcstrt_f uartx.s.b1
#define ethmd_f uartx.s.b2




//=======================================================================
#endif

