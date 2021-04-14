/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/
#ifndef __MAIN_H__
#define __MAIN_H__

/******************************** include ***********************************************/


/******************************** Function ************************************************/
void Common_Task(void);
void TFTLCD_Task(void);
void Stm32f2xx_initial(void);


typedef enum {
	Pri_Common_Task =100,
	Pri_Task1,
	
	Pri_EEP_Task,
	Pri_VolumeTask,
	Pri_TFTLCD_Task,

	Pri_MP3_Task,
	Pri_BZTask,

}__Priority;

/******************************** global parameter ****************************************/
#ifdef __GP_MAIN__
#define maEX   
#else
#define maEX extern
#endif





#endif


/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/

