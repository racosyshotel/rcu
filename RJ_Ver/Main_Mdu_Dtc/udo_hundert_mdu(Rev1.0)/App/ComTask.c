/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : EWARM Compiler								    */
/* program by	  : JK.Won									    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : Insem Inc.									    */
/****************************************************************************************************/
#define __GP_COMTASK__

#include "bit_define.h"
#include "stm32f2xx_init.h"

#include "ComTask.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "main.h"
#include "iexec.h"

/******************************************************************************************************/
/******************************************************************************************************/

/*
u8 utaskctrl;
void Common_Task(void){

	while(1){
//		Iwdog_initial(5000);
		switch(utaskctrl) {
      case 0:
//				UART1_control_proc();   
				Internal_timer_Proc();	
				utaskctrl++;	break;

			case 1:
         iexec_control_proc();
         uart3_frame_proc();
         uart2_frame_proc();
				utaskctrl =0;	break;


			default:  	utaskctrl =0;		break;
		}
	}
}
*/


/************************************** COPYRIGHT 2007 Insem Inc.*************************************/

