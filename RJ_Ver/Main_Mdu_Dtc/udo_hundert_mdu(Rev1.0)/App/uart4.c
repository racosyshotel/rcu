/*=====================(C) COPYRIGHT 2008 Insem Inc.=========================
program 		:   
processor	: STM32F101xx
compiler		: IAR 4.41A Compiler 
program by 	: YN.Kim
date 			: 2008.	  .
copy right 	: Insem Inc.
===========================================================================*/

#define __UART4_C__

#include "stm32f2xx.h"
#include "bit_define.h"
#include "stm32f2xx_init.h"

#include "comtask.h"
#include "uart1.h"
#include "iexec.h"
#include "eeprom.h"

#include "rtos.h"
#include "rtosinit_stm32f10.h"
#include "stm32f10x_it.h"
#include "lcd_oper.h"

//========================================================================
//========================================================================
u8 u4ctrl =7;
void UART4_Frame_Proc(void) {
static u8 u4txinx;

	switch(u4ctrl) {
		case 0:
			if(F4Send_f) { u4txinx =0;
				extm.s.u4c =3, Trxe4 =1;						// TX mode setup
				u4ctrl++;
			}
			watchdog.s.b5 =1; 			// watchdog setup
			break;

		case 1:	if(extm.s.u4c ==0) u4ctrl++;		break;
		case 2:
			UART4->DR =tx4.buf[u4txinx] &(u16)0x01FF;
			extm.s.u4c =10;	u4ctrl++;
			break;

		case 3:
			if(USART_GetFlagStatus(UART4, USART_FLAG_TC) !=RESET) {
				if(++u4txinx >=Pctxmax4) { 
					extm.s.u4c =2;	u4ctrl++;
				}
				else u4ctrl--;
			}
			else if(extm.s.u4c ==0) u4ctrl =7;
			break;

		case 4:
			if(extm.s.u4c ==0) {
				F4Send_f =0, Trxe4 =0;						// RX mode setup
				u4ctrl =0;
			}
         break;

// powerup start
		case 7:
			F4Send_f =0;
			UART4_initial();						// uart0 initial

			if(zin.msel &0x80) __xcprintf("Uart4 Initial !!!\r\n");

		default:		u4ctrl =0;		break;
 	}
}

//========================================================================
/* UART4_initial	:								              */												
//========================================================================
void UART4_initial(void) {
//NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

	 /* Enable the UART4 Pins Partial Software Remapping */
//	GPIO_PinRemapConfig(GPIO_Remap_UART4, DISABLE);  

	/* Enable GPIOB and UART4 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	 /* Enable UART4 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	/* Configure UART4 Tx (PC10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure UART4 Rx (PC11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

/* UART4 configuration ------------------------------------------------------*/
  /* UART4 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (rm_rts and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//  USART_InitStructure.USART_Clock = USART_Clock_Disable;
	// USART_InitStructure.USART_CPOL = USART_CPOL_Low;
	//  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
	//  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

	/* Configure the UART4 */
	USART_Init(UART4, &USART_InitStructure);

	/* Enable the USART Transmoit interrupt: this interrupt is generated when the 
	USART1 transmit data register is empty */  
	//  USART_ITConfig(UART4, USART_IT_TXE, ENABLE);

	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	UART4 receive data register is not empty */
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

	/* Enable UART4 */
	USART_Cmd(UART4, ENABLE);

	OS_ARM_InstallISRHandler(_ISR_ID_USART4, (OS_ISR_HANDLER*)UART4_IsrHandler);
	OS_ARM_ISRSetPrio(_ISR_ID_USART4, 100);
	OS_ARM_EnableISR(_ISR_ID_USART4); 	  
}

//========================================================================
/* u4_byte_loading	:																											*/												
//========================================================================
u8 u4_ByteLoad() {

	if(Ring4.ldad !=Ring4.svad) { TimerSetup(4, rm_timeout);   
		spdata4 =Ring4.buf[Ring4.ldad];
		if(++Ring4.ldad >=sizeof(Ring4.buf)) Ring4.ldad =0;

#ifdef debug_mode_u4x
		if(zin.msel &0x80) { 
			sprintf(xdebug, "%c.", spdata4); mm;
		}
#endif		
		return 1;
	}
	return 0;  
}

u8 chksum_make(void) {
unsigned char ix, checksum;

 	if(tx4.buf[1] && tx4.buf[2]) {
 		checksum =ix =0;
 		while (tx4.buf[ix] !=rm_dev) checksum ^=tx4.buf[ix++];
		checksum ^=tx4.buf[ix++];	
 		tx4.buf[ix++] =checksum &0x0f |0x70;	tx4.buf[ix++] =rm_etx;  tx4.buf[ix] =0;
 	}
	return strlen((char*)tx4.buf);
}
unsigned char chksum_check(void) {
u8 ix, checksum;

 	checksum =ix =0;
	while(rx4.buf[ix] !=rm_dev &&ix !=sizeof(_u4size)) checksum ^=rx4.buf[ix++];
	if(ix ==sizeof(_u4size)) return 0;
	checksum ^=rx4.buf[ix++];
	
 	if(rx4.buf[ix] ==(checksum &0x0f |0x70)) return 1;
	else return 0;
}

void Senddata_load(void) {
unsigned char ix;

	ix =0;
	tx4.buf[ix++] =rm_stx;								// start code
	tx4.buf[ix++] =c_remsw;
	tx4.buf[ix++] =group_data ?'1':'0';
	if(locgrp_f) {
		tx4.buf[ix++] =group_data >>8 &0x0f | 0x30;
		tx4.buf[ix++] =(group_data >>4) &0x0f | 0x30;
		tx4.buf[ix++] =group_data &0x0f | 0x30;
	}
	else {
		tx4.buf[ix++] =local_data >>8 &0x0f | 0x30;
		tx4.buf[ix++] =(local_data >>4) &0x0f | 0x30;
		tx4.buf[ix++] =local_data &0x0f | 0x30;
	}
	tx4.buf[ix++] ='A';
	tx4.buf[ix++] =abc_f ? '1' :'0';
	tx4.buf[ix++] ='B';
	tx4.buf[ix++] =group_data ? '1' :'0';
	tx4.buf[ix++] =rm_dev;

	Pctxmax4 =chksum_make();
	TimerSetup(4, rm_timeout);
	F4Send_f =1;						// end data char.. / send flag setup
}


void Byte_out(u8 data) {

	tx4.buf[0] =data;	tx4.buf[1] =0;
	TimerSetup(4, rm_timeout);
	Pctxmax4 =1, F4Send_f =1;
}

u8 remmax_key;
void Receive_data_iexec(void) {
u8 remote_link, local_remote[4];
uchar ix, data, iz;
 
	ix =data =0;
 	while(data !=rm_dev && data !=rm_etx) {
 		if(data =='R') { 										// 'R' .. all function
			remote_link =rx4.buf[ix++] &0x0f;			// remote box use no.
			if(remote_link ==8 || selfid ==remote_link) all_lock_f =0;
			else all_lock_f =1;
		}
 		else if(data =='S') { 								// 'S' .. group function
			remote_link =rx4.buf[ix++] &0x0f;			// remote box use no.
			if(remote_link ==8 || selfid ==remote_link) group_lock_f =0;
			else group_lock_f =1;
		}
		else if(data =='L') {								// remote box pair priority
			for(iz=0; iz<4; iz++) local_remote[iz] =rx4.buf[ix++];
			iz =local_remote[selfid>>1];

			if(selfid &0x01) rboxlock_f =(iz=='1') ?1:0;	
			else rboxlock_f =(iz=='2') ?1:0;	
		}
		else if(data =='Q') {
			remmax_key =rx4.buf[ix++] &0x0f;
		}
		data =rx4.buf[ix++];
	}

}

//========================================================================
//========================================================================
u8 uart4ctrl =7;
void UART4_control_proc(void) {

	switch(uart4ctrl) {
// line link
		case 0:					
			if(u4_ByteLoad()) {												
				if(spdata4 ==rm_enq) {
					led1++;	srx_point =0, u4strt_f =1;
				}
				else if(u4strt_f) { u4strt_f =0;
					if(selfid ==(spdata4 &0x0f)) {
						if(xserial_st_f) { xserial_st_f =0; 	// main 3È¸ recall 
							Byte_out('+'+(selfid>>1));
						}
						else Byte_out(rm_ack), uart4ctrl++;					
//						rmstatc++, rmstatc &=0x1f;
					}
				}
			}
			else if(xserial_st_f &&!extm.s.u4ctmp) { xserial_st_f =0;
				Byte_out('+'+(selfid>>1));
				serial_ok_f =0; TimerSetup(10, resend_wtime);				 
			}

			else if(tm4fg &&u4strt_f) u4strt_f =0;				// byte timeout ???
			else if(tm10fg) { 
				if(!serial_ok_f) xserial_st_f =1, TimerSetup(10, resend_wtime);				 
			}
			watchdog.s.b6 =1; 			// watchdog setup
			break;
		
		case 1: 	if(!F4Send_f) uart4ctrl++;  break;
		case 2:
			if(u4_ByteLoad()) {
				if(spdata4 ==rm_stx) { 	srx_point =0, u4strt_f =1;
				 	rx4.buf[srx_point++] =spdata4;
				}
				else if(spdata4 ==rm_etx) { u4strt_f =0;
					if(chksum_check()) { 
						Receive_data_iexec();						// receive data Ã³¸®
						Byte_out(rm_ack), uart4ctrl++;
						rmstatc++, rmstatc &=0x1f;
					}
					else Byte_out(rm_nak);
				}
				else if(u4strt_f) rx4.buf[srx_point++] =spdata4;
			}
			else if(tm4fg) u4strt_f =0, uart4ctrl =0;
			break;

		case 3:	if(!F4Send_f) uart4ctrl++;  break;
		case 4:
			if(u4_ByteLoad()) {										// 1byte read
				if(spdata4 ==rm_rts) retry =rm_loopc, Senddata_load(), uart4ctrl++;
				else if(spdata4==rm_eot) uart4ctrl =0;
			}
			else if(tm4fg) uart4ctrl =0; 							// timeout checking ???
			break;

// text send
		case 5:	if(!F4Send_f) uart4ctrl++;  break;			// data send
		case 6:
			if(u4_ByteLoad()) {	 
				if(spdata4==rm_ack || spdata4==rm_eot) uart4ctrl =0, serial_ok_f =1;
				else if(spdata4==rm_nak) uart4ctrl--;			// resend data
				else if(!--retry) uart4ctrl =0;
			}
			else if(tm4fg) uart4ctrl =0;
			break;


// powerup start
		case 7:
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOD, &GPIO_InitStructure);
			Trxe4 =0;						// RX mode setup

			group_lock_f =all_lock_f =0;
			remmax_key =12;	
			uart4ctrl++;

		case 8:
			if(eepload_f) uart4ctrl++;
			break;
			
		default:		uart4ctrl =0;		break;
	}

	UART4_Frame_Proc();
	key_lock_f =(group_lock_f ||all_lock_f) ? 1:0;
}
//========================================================================
//========================================================================

