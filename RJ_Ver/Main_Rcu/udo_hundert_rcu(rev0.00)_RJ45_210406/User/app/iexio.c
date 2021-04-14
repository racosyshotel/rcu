/*=====================(C) COPYRIGHT 2009 Insem Inc.=========================
program 		: KROAD ALARM CONTROLLER  
processor	: STM32F103xx
compiler		: IAR 4.41A Compiler 
program BY 	: YN.Kim
date 			: 2009.	  .
copy right 	: Insem Inc.
===========================================================================*/
#define __IEXIO_H__

#include "stm32f10x_lib.h"
#include "main.h"

//==================================================================
#define c_period 10
//==================================================================
void iexio_control_proc(void) {
static u8 exioctrl =7;
__bits regs;
static u8 esync, testc;

	switch(exioctrl) {
		case 0:
			if(testmode_f) {
				if(tm4fg) { Timer_setup(4, 300);	
					if(++testc >=6) testmode_f =0;
				}
				l_data =testc &0x01 ? 0:1;
			}
			else {
				if((led.dword>>lcnt) &0x01) l_data =0;
				else l_data =1;
			}
			
			w8 =c_period; 	while(w8--);
			l_clk =1;	w8 =c_period; while(w8--);	l_clk =0;
			if(++lcnt >=24) { lcnt =0;
				l_stb =1;	w8 =c_period; while(w8--);	l_stb =0;
			}
			led.s.rl1 =rly1, led.s.rl2 =rly2;

			if(!testmode_f) exioctrl++;
			break;

		case 1:
			iwdata >>=1;
			if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)) iwdata |=0x8000;
				
			i_clk =1;	w8 =10; while(w8--);	i_clk =0;

			if(++icnt >=16) { icnt =0;
				idata =iwdata;
				i_stb =0;	w8 =c_period; while(w8--);	i_stb =1;
				
			}
			exioctrl++;
			break;

		case 2:
			if(idata ==keyd &&keyst_f);
			else if(idata ==keydb) {
				if(Extm.s.keyc ==0) { keyst_f =1, keyd =idata;					
					led.s.indicator =idata;
				}
			}
			else keydb =idata, Extm.s.keyc =100;
			exioctrl++;
			break;
			
// eth_sync
		case 3:
			regs.s.b0 =GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14);
			if(regs.s.b0 !=esyncd) { esyncd =regs.s.b0;
				esync++;
			}
			if(tm4fg) { Timer_setup(4, 250);
				if(esync >=3) {
					esync_f =1;
					esyncst_f =1;
				}
				else {
					esync_f =0;
					if(esyncst_f) { esyncst_f =0;
						ethmd_f =1;
					}
				}
				esync =0;
				led.s.elink =esync_f;
			}
			exioctrl++;
			break;
			
			
// powerup start
		case 7:
// led
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_8 | GPIO_Pin_9;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOE, &GPIO_InitStructure);
// key
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 |GPIO_Pin_3;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOE, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOE, &GPIO_InitStructure);
			
// eth_sync
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOD, &GPIO_InitStructure);

			i_stb =0;	w8 =10; while(w8--);
			i_stb =1;
			keyst_f =0;
			testmode_f =1;
			
		default:		exioctrl =0;		break;
	}
}

//==================================================================

