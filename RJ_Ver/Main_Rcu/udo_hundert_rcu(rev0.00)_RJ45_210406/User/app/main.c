/*=====================(C) COPYRIGHT 2014 PLUS-H.=========================
program		    : CONTROL BOX(RCS-300-RCU)
processor 	  : STM32F103xx
compiler		  : IAR 6.2A Compiler 
program BY	  : H.H.Hwang
date			    : 2014.	 .
copy right	  : P L U S  -  H. 
============================================================================*/
#define __MAIN_H__  

#include "stm32f10x_lib.h"   
#include "main.h"


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : Nonet
* Return         : None
*******************************************************************************/
u8 proctrl =7;
int main(void) {
#ifdef DEBUG
  debug();
#endif
	CORTEX_initial();      
	while(1)
        {
 		tcrst =~tcrst;   
		switch(proctrl){
			case 0:
				UART1_control_proc();				
        UART2_control_proc();
      	proctrl++;       
        break; 
        
			case 1:   
        Internal_timer_Proc(); 				
        internal_exec_proc();
				proctrl++;       
        break; 

			case 2:
        UART3_control_proc();
				proctrl=0;		    
        break; 

        default:		proctrl =0;		break;              
		}  
      
	}
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1) { 
  }
}
#endif

/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/

