/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/
#define __IEXEC_C__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bit_define.h"
#include "stm32f2xx.h"
#include "stm32f2xx_init.h"

#include "comtask.h"
#include "uart1.h"
#include "uart3.h"
#include "iexec.h"


/******************************************************************************************************/
/******************************************************************************************************/
u8 iexecctrl =7;
void iexec_control_proc(void) {

	switch(iexecctrl) {
		case 0:
         if(tm15fg) { TimerSetup(15,500);
            P_led8++;
         }
			break;

			
// powerup start
		case 7:
			

		default:	iexecctrl =0;	break;
	}
}

/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/
