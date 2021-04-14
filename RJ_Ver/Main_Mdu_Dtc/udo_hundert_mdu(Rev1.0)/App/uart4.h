/*=====================(C) COPYRIGHT 2008 Insem Inc.=========================
program 		: HDPVR FRONT PANEL 
processor	: STM32F101xx
compiler		: IAR 4.41A Compiler 
program BY 	: YN.Kim
date 			: 2008.	  .
copy right 	: Insem Inc.
=======================================================================*/
#ifndef __UART4_H__
#define __UART4_H__


void UART4_control_proc(void);
void UART4_initial(void);
void UART4_Frame_Proc(void);

u8 u4_ByteLoad(void);

#define debug_mode_u4
#define Trxe4 GpioD->Bit.b3


#define	rm_stx	'('
#define	rm_etx	')'
#define	rm_eot	'#'
#define	rm_ack	'&'
#define	rm_nak	'!'
#define	rm_enq	'*'
#define	rm_dev	'$'
#define	rm_rts	'^'

#define	c_remsw	'S'
#define	c_remuse	'R'

#define rm_timeout	200		 
#define resend_wtime	3000
#define rm_loopc	3
#define selfid mem.s.device_id

//=======================================================================
typedef struct {
	u16 ldad;
	u16 svad;
	u8 buf[256];
}__Ring4;

typedef struct {
	u8 size[32];
}_u4size;


#pragma pack(1)
typedef union {
	_u4size s;

	u8 buf[sizeof(_u4size)];
}__Trx4;

//=======================================================================

#ifdef __UART4_C__
#define u4EX

#else
#define u4EX extern 

#endif


u4EX __Ring4 Ring4;
u4EX __Trx4 tx4, rx4;
u4EX u8 spdata4;
u4EX u16 Pctxmax4;
u4EX u8 retry, srx_point;

//=======================================================================
u4EX __wbits uart4;
#define F4Send_f 		uart4.s.b0
#define f4strt_f		uart4.s.b1
#define group_lock_f		uart4.s.b2
#define all_lock_f		uart4.s.b3
#define serial_ok_f	uart4.s.b4
#define u4strt_f		uart4.s.b5
#define xserial_st_f	uart4.s.b6
#define key_lock_f		uart4.s.b7
#define rboxlock_f	uart4.s.b8
#define grp_f			uart4.s.b9
#define abc_f			uart4.s.b10


//=======================================================================
//=======================================================================
#endif

