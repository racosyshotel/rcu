/**
  ******************************************************************************
  * @file    Project/STM32F2xx_StdPeriph_Template/stm32f2xx_it.c 
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "bit_define.h"
#include "stm32f2xx_it.h"
#include "Uart1.h"
#include "Uart2.h"
#include "Uart3.h"
#include "stm32f2xx_init.h"





/** @addtogroup Template_Project
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{

//}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 
	
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description	  : This function handles USART1 global interrupt request.
* Input			  : None
* Output 		  : None
* Return 		  : None
*******************************************************************************/
void USART1_IRQHandler(void) {


  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){ 	 // rx interrupt check
  
		ring.buf[ring.svad] =(u16)(USART1->DR & (u16)0x01FF);
		if(++ring.svad >=sizeof(ring.buf)) ring.svad =0;  
	  
		 /* Clear the USART1 Receive interrupt */
		 USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  
	 }
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description	  : This function handles USART2 global interrupt request.
* Input			  : None
* Output 		  : None
* Return 		  : None
*******************************************************************************/
void USART2_IRQHandler(void) {
  unsigned char uart2_sbuf;

  

  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){		// rx interrupt check
    uart2_sbuf = (u16)(USART2->DR & (u16)0x01FF);
    if(!f_uart2_com_start){
      if(uart2_sbuf == '('){
        f_uart2_com_start = 1;
        uart2_rxding_point = 0;
        uart2_rxding_buf[uart2_rxding_point++] = uart2_sbuf;
        
      }
    }
    else{
      if(uart2_rxding_point == 1){
        if((uart2_sbuf > 25) || (uart2_sbuf < 5)) f_uart2_com_start = 0;
      }
      if((uart2_sbuf == ')') && (uart2_rxding_point == uart2_rxding_buf[1])){
        f_uart2_com_start = 0;
        uart2_rxding_buf[uart2_rxding_point++] = uart2_sbuf;
        memcpy(&uart2_rxd_buf[0],&uart2_rxding_buf[0],uart2_rxding_point);
        uart2_rxding_point = 0;
        f_uart2_frame_rx_ok = 1;
      }
      else{
        if(uart2_rxding_point > 25) f_uart2_com_start = 0;
        else uart2_rxding_buf[uart2_rxding_point++] = uart2_sbuf;
      }
    }  
		/* Clear the USART1 Receive interrupt */
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  } 	
        
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description	  : This function handles USART3 global interrupt request.
* Input			  : None
* Output 		  : None
* Return 		  : None
*******************************************************************************/
void USART3_IRQHandler(void) {
  unsigned char uart3_sbuf;

  if(uart3_call_timer > 10){
    uart3_call_timer = 10;
  }
  
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){ 	 // rx interrupt check
    uart3_sbuf = (u16)(USART3->DR & (u16)0x01FF);
    if(!f_uart3_com_start){
      if(uart3_sbuf == '{'){
        f_uart3_com_start = 1;
        uart3_rxding_point = 0;
        uart3_rxding_buf[uart3_rxding_point++] = uart3_sbuf;
      }
    }
    else{
      if(uart3_rxding_point == 1) uart3_rx_length = uart3_sbuf;
      if((uart3_sbuf == '}') && (uart3_rxding_point == uart3_rx_length-1)){
        f_uart3_com_start = 0;
        uart3_rxding_buf[uart3_rxding_point++] = uart3_sbuf;
        memcpy(&uart3_rxd_buf[0],&uart3_rxding_buf[0],uart3_rxding_point);
        uart3_rxding_point = 0;
        f_uart3_frame_rx_ok = 1;

      }
      else{
        if(uart3_rxding_point > uart3_rx_length) f_uart3_com_start = 0;
        else if(uart3_rxding_point > 90) f_uart3_com_start = 0;
        else uart3_rxding_buf[uart3_rxding_point++] = uart3_sbuf;
      }
    }  
	  
		 /* Clear the USART1 Receive interrupt */
		 USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  
	}
}


/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void) {


	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET) { 
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			// interrupt clear  

		tm1ms_f =1;
		if(esctc) esctc--;

/*    
		if(!--Wdtcnt){ 
      Wdtcnt =3000;
			watchdog.word |=0xff00;
			                  
			if(watchdog.word !=0xffff){ 
        Wdtdis_f =1;
//#ifdef __WATCHDOG_ENABLE__ 
//				sprintf(xdebug, "Watchdog : %02x\r\n", watchdog.word); mmd;
//#endif
			}
			watchdog.word =0;
		}
*/
    
/*	
#ifdef __WATCHDOG_ENABLE__ 
		if(!Wdtdis_f) {
			if(!(Wdtcnt &0x03ff)) Iwdog_initial(3000);
		}
#endif
*/		
	}		
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

