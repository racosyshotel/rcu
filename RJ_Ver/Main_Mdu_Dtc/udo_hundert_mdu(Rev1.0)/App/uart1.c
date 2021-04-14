/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/

#define __UART1_C__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bit_define.h"

#include "stm32f2xx_init.h"
#include "stm32f2xx_it.h"
#include "uart1.h"
#include "main.h"
//#include "eeprom.h"
//#include "pc2uart.h"
//#include "iexio.h"
//#include "buzzer.h"



/******************************************************************************************************/
/* UART1_initial :                                                                                    */ 											
/******************************************************************************************************/
void UART1_initial(void){
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
    
    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
     
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART1, &USART_InitStructure);

      /* Enable USART */
    USART_Cmd(USART1, ENABLE);    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    //
    // Install USART1 Handler with preemtion level one above lowest level to ensure communication
    //
    // Enable the USART1 Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
}

//=======================================================================
//=======================================================================


//=======================================================================
//=======================================================================

