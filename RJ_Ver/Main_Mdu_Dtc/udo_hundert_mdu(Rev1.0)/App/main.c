/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang						(DCU)                                                    */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/
#define __GP_MAIN__  

/* Includes ------------------------------------------------------------------*/
#include "bit_define.h"
#include "stm32f2xx.h"
#include "stm32f2xx_init.h"  
#include "Uart1.h"
#include "Uart2.h"
#include "Uart3.h"
#include "main.h"
#include "comtask.h"
#include "stm32f2xx_conf.h"

//#include "iexec.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


int main(void){
  /*!< At this stage the microcontroller clock setting is already configured,
  this is done through SystemInit() function which is called fropm startup
  file (startup_stm32fxxx_xx.s) before to branch to applicatiosdfsdfn main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32fxxx.c file   
  */

  Stm32f2xx_initial();             // initialize Hardware for STM32F20x.  
   
   
  while(1){
    Iwdog_initial(3000);
    switch(utaskctrl) {
      case 0:
        Internal_timer_Proc();	
        utaskctrl = 1;	
        break;

      case 1:
        uart2_frame_proc();
        utaskctrl = 2;	 
        break;

      case 2:
        uart3_frame_proc();
        utaskctrl = 0;	
        break;

      default:  	
        utaskctrl =0;		
        break;
    }
  }
}  
/*******************************************************************************
* Function Name  : USE_FULL_ASSERT
* Description    : Debug
*******************************************************************************/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
