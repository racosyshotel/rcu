/*=====================(C) COPYRIGHT 2008 Insem Inc.=========================
program 		:   
processor	: STM32F101xx
compiler		: IAR 4.41A Compiler 
program BY 	: YN.Kim
date 			: 2008.	  .
copy right 	: Insem Inc.
===========================================================================*/

#define __UARTx_H__

#include "stm32f10x_lib.h"
#include "bit_define.h"
#include "uart1.h"
#include "uartx.h"
#include "stm32f10x_init.h"
#include "eeprom.h"
//#include "iexio.h"

//========================================================================
//========================================================================
u8 pc2ctrl =7;
void pc_sframe_proc(void) {
static u16 ux_txinx;

	switch(pc2ctrl) {
		case 0:
			if(pcsend_f) { ux_txinx =0;
				Extm.s.pc =3;
				pc2ctrl++;
			}
			break;
	
		case 1:
			if(Extm.s.pc) return;
			
			Serial_Port->DR =Pctx.buf[ux_txinx] &(u16)0x01FF;
			Extm.s.pc =10;	pc2ctrl++;
			break;

		case 2:
			if(USART_GetFlagStatus(Serial_Port, USART_FLAG_TC) !=RESET) {
				if(++ux_txinx >=Pctx_max) { pcsend_f =0;
					pc2ctrl =0;
				}
				else Extm.s.pc =0, pc2ctrl--;
			}
			else if(!Extm.s.pc) pc2ctrl =7;
			break;

// powerup start
		case 7:
			pcsend_f =0;
			Uartx_initial();						// uart0 initial

			if(zin.msel &0x80) __xcprintf("Uartx Initial !!!\r\n");

		default:		pc2ctrl =0;		break;
 	}
}

void Uart_port_initial(void) {
GPIO_InitTypeDef GPIO_InitStructure;

#if defined ( __USART2__ )
	 /* Enable the USART3 Pins Partial Software Remapping */
//	GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);  

	/* Enable GPIOA and USART2 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	 /* Enable USART2 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Configure USART2 Tx (PA2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Rx (PA3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

#elif defined ( __USART3__)
	/* Enable GPIOB and USART3 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	 /* Enable USART3 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* Configure USART3 Tx (PB10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART3 Rx (PB11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

#elif defined ( __USART4__)
	/* Enable GPIOC and USART4 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	 /* Enable USART4 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	/* Configure USART4 Tx (PB10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure USART4 Rx (PB11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

#elif defined ( __USART5__)
	/* Enable GPIOC/GPIOD and USART5 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
	 /* Enable USART5 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

	/* Configure USART5 Tx (PC12) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure USART5 Rx (PD2) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

#else
#error "Serial POrt가 정의되지 않았습니다." 
#endif

}

//========================================================================
/* Uartx_initial	:								              */												
//========================================================================
void Uartx_initial(void) {
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;

	Uart_port_initial();					// uart GPIO initial

/* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follow:
		  - BaudRate = 115200 baud  
		  - Word Length = 8 Bits
		  - one Stop Bit
		  - No parity
		  - Hardware flow control disabled (RTS and CTS signals)
		  - Receive and transmit enabled
		  - USART Clock disabled
		  - USART CPOL: Clock is active low
		  - USART CPHA: Data is captured on the second edge 
		  - USART LastBit: The clock pulse of the last data bit is not output to 
								 the SCLK pin
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//  USART_InitStructure.USART_Clock = USART_Clock_Disable;
// USART_InitStructure.USART_CPOL = USART_CPOL_Low;
//  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
//  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

  /* Configure the USARTx */
  USART_Init(Serial_Port, &USART_InitStructure);

 /* Enable the USART Transmoit interrupt: this interrupt is generated when the 
	USARTx transmit data register is empty */  
//  USART_ITConfig(Serial_Port, USART_IT_TXE, ENABLE);

/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USARTx receive data register is not empty */
  USART_ITConfig(Serial_Port, USART_IT_RXNE, ENABLE);

  /* Enable USARTx */
  USART_Cmd(Serial_Port, ENABLE);

  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = Serial_Port_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

//========================================================================
/* byte_loading	:																											*/												
//========================================================================
u8 pcbyte_loading() {

	if(pcring.ldad !=pcring.svad) {  Extm.s.erxc=3;
		pcdata =pcring.buf[pcring.ldad];
		if(++pcring.ldad >=sizeof(pcring.buf)) pcring.ldad =0;

#ifdef debug_mode_pc
		if(zin.msel &0x80) { 
			sprintf(xdebug, "%c", pcdata); mm;
		}
#endif		
		return 1;
	}
	return 0;  
}
/*
u8 framedata_load(void) {
static u8 exsp1_inx;

	if(pcbyte_loading()) {
		if(pcdata =='<') pcstrt_f =1, exsp1_inx =0;
		else if(pcdata =='>') { pcstrt_f =0;
			Pcrx.buf[exsp1_inx++] =pcdata, Pcrx.buf[exsp1_inx] =0;
			return 1;
		}
		if(pcstrt_f) {
			Pcrx.buf[exsp1_inx] =pcdata;
			if(++exsp1_inx >=sizeof(_pcsize)) pcstrt_f =0;
		}
	}
	return 0;
}
*/
void hw_trig(u8 sel) {

	hwtrig =sel;
}

//========================================================================
#define exu1_timeout	2000

//========================================================================
u8 u3test;
u8 uartxctrl =7;
void PC2_control_proc(void) {
	switch(uartxctrl) {
		case 0:
			if(eth_download_f) { eth_download_f =0;
				hw_trig(0);
				eth_reset =0; 	w8=10; while(w8--);	eth_reset =1;	// ethernet reset
				
				complete_f =0, Timer_setup(11, 1000);
				uartxctrl =3;
			}
			else if(eth_upload_f) { eth_upload_f =0;
				hw_trig(0);
				eth_reset =0; 	w8=10; while(w8--);	eth_reset =1;	// ethernet reset
				complete_f =0, Timer_setup(11, 1000);
				uartxctrl =4;
			}

			pcbyte_loading();			// 
			if(tm2fg) { Timer_setup(2, 1000);
				memset(Pctx.buf, u3test++, sizeof(_pcsize));
				pcsend_f =1; 
            Pctx_max =sizeof(_pcsize);
				if(u3test <0x20 ||u3test >=0x80) u3test =0x20;
			}

			if(ethmd_f) { ethmd_f =0;			// ethernet 초기화 
				uartxctrl =8;
			}
			break;

// ethernet download
		case 3: if(tm11fg) esc_f =ethcnt =0, uartxctrl =1; 	break;
		case 1: 
			strcpy((char*)Pctx.buf, (char*)eth_wcmd[ethcnt]);
			if(Pctx.i.stx =='.') { uartxctrl =0;			// end point
				complete_f =1;
				hw_trig(1);
			}
			else if(Pctx.i.sel =='.') {
				if(mem.s.wdata[2][0] =='0') { 		// static ip
ex2p: 			strcpy((char*)&Pctx.i.sel, (char*)mem.s.wdata[Pctx.i.index &0x0f]);
					Pctx_max =strlen((char*)Pctx.buf);
					Pctx.buf[Pctx_max++] ='>';
				
ex1p: 			pcsend_f =1, Timer_setup(11, exu1_timeout);
					uartxctrl++;
					for(w8=0; w8<Pctx_max; w8++) sprintf(xdebug, "%c", Pctx.buf[w8]), mm;
				}
				else {
					if(Pctx.i.index >='3' &&Pctx.i.index <='5') ethcnt++;
					else goto ex2p;
			
				}
			}
			else {
				Pctx_max =strlen((char*)Pctx.buf);
				goto ex1p;
			}
			break;

		case 2:
			if(framedata_load()) {
				if(Pcrx.rsp.rsp =='S') {	uartxctrl--;
					ethcnt++;
					__xcprintf(" _Ok_\r\n");
				}
				else {
					sprintf(xdebug, "Fail> '%c'\r\n", Pcrx.rsp.rsp); mm;
				}
			}
			else if(zin.msel &0x80) goto exsp2c1p;
			else if(tm11fg) { uartxctrl--;
				Timer_setup(12, 15000);
				__xcprintf(" Time out !!! ");
			}
			else if(esc_f) { esc_f =0; 				// exit
exsp2c1p:	hw_trig(1);
				complete_f =1;
				uartxctrl =0;
			}
			break;

// ethernet upload
		case 4: if(tm11fg) ethcnt =0, uartxctrl++;		break;
		case 5: 
			strcpy((char*)Pctx.buf, (char*)eth_rcmd[ethcnt]);
			if(Pctx.i.stx =='.') { uartxctrl =0;			// end point
				complete_f =1;
				hw_trig(1);
			}
			else {
				Pctx_max =strlen((char*)Pctx.buf);
				pcsend_f =1, Timer_setup(11, exu1_timeout);
				uartxctrl++;
			}
			break;

		case 6:
			if(framedata_load()) { uartxctrl--;
				strcpy(xdebug, (char*)Pcrx.buf); mm;	__linefeed();

				Pcrx.buf[strlen(xdebug)-1] =0;
				strcpy((char*)update[ethcnt], (char*)Pcrx.buf+2); 	// receive data backup
				ethcnt++;
			}
			else if(zin.msel &0x80) goto exsp6c1p;
			else if(tm11fg) { uartxctrl--;
				Timer_setup(12, 15000);
				__xcprintf(" Time out !!! ");
			}
			else if(esc_f) { esc_f =0; 				// exit
exsp6c1p:	hw_trig(1);
				complete_f =1;
				uartxctrl =0;
			}
			break;

// powerup start
		case 7:
/*			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOD, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOD, &GPIO_InitStructure);
*/
			uartxctrl++;
			break;
			
		case 8:			
			if(u1_init_f) { uartxctrl++;
				if(zin.msel &0x80) __xcprintf("Ethernet Reset !!!\r\n");

				eth_reset =1;
				hw_trig(0);
			}
			break;
		
		case 9:
			eth_reset =0;
			Timer_setup(11, 100);
			uartxctrl++;
			break;
		case 10:
			if(tm11fg) { eth_reset =1, uartxctrl++;
				hw_trig(1);
			}
			break;

		default:		uartxctrl =0;		break;
	}
	pc_sframe_proc();

}
//========================================================================
//========================================================================

