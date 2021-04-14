/*=====================(C) COPYRIGHT 2009 Insem Inc.=========================
program 		: KROAD ALARM CONTROLLER  
processor	: STM32F103xx
compiler		: IAR 4.41A Compiler 
program BY 	: YN.Kim
date 			: 2009.	  .
copy right 	: Insem Inc.
===========================================================================*/
void iexio_control_proc(void);

//==================================================================
#define l_data	GpioE->Bit.b7
#define l_clk	GpioE->Bit.b9
#define l_stb	GpioE->Bit.b8

#define i_data	GpioE->Bit.b4
#define i_clk	GpioE->Bit.b3
#define i_stb	GpioE->Bit.b2

//==================================================================
typedef union {
	u32 dword;
	struct {
		unsigned rl2 : 1;
		unsigned rl1 : 1;

		unsigned erxd : 1;
		unsigned etxd : 1;
		unsigned elink : 1;

		unsigned rxd : 1;
		unsigned txd : 1;

		unsigned indicator : 16;
		unsigned power : 1;
	}s;
}__led_struct;

//==================================================================
//==================================================================
#ifdef __IEXIO_H__
#define ioEX 

#else
#define ioEX extern

#endif

ioEX u16 keydb, keyd;
ioEX __led_struct led;
ioEX u8 lcnt, icnt;
ioEX u16 iwdata, idata;

//==================================================================
ioEX __bits iexio;
#define keyin_f	iexio.s.b0
#define keyst_f	iexio.s.b1
#define esyncd		iexio.s.b2
#define esync_f	iexio.s.b3
#define testmode_f	iexio.s.b4
#define esyncst_f	iexio.s.b5



//==================================================================


