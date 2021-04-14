/*=====================(C) COPYRIGHT 2014 PLUS-H.=========================
program		    : CONTROL BOX(RCS-300-RCU)
processor 	  : STM32F103xx
compiler		  : IAR 6.2A Compiler 
program BY	  : H.H.Hwang
date			    : 2014.	 .
copy right	  : P L U S  -  H. 
============================================================================*/
void eeprom_control_proc(void);
void eepdata_display(void);
void eeprom_write_go(u16 time);
void eepretproc(u8 setaddr, u8 retaddr);

//========================================================================

#pragma pack(1)
typedef struct {
	u16 bzfrq;	
	u8 wdata[16][32];			// ethernet data
		
	u16 id_code;
	u16 checksum;
}_xmem;

typedef union {
	_xmem s;
	u8 buf[sizeof(_xmem)];
}__mem;

//========================================================================
#define Half_Address			(Full_Address/2)
#define Eep_Waittime 		7000

//========================================================================
#ifdef __EEPROM_H__
#define eEX 

eEX u8 eepctrl =15;
#else
#define eEX extern

eEX u8 eepctrl;
#endif

eEX __mem mem, _mem;
eEX __mem eepmem;

eEX u8 eepans, eepercnt, Volumec;
eEX u16 rw_index;
eEX u16 eep_address, rw_address;
eEX u8 reteepctrl;

//========================================================================
eEX __bits eep;
#define eepwrite_f	eep.s.b0
#define eepread_f		eep.s.b1
#define eepload_f		eep.s.b2
#define eepfail_f		eep.s.b3
#define eepdisp_f		eep.s.b6
#define xsave_f		eep.s.b7

//========================================================================
//========================================================================


//========================================================================

