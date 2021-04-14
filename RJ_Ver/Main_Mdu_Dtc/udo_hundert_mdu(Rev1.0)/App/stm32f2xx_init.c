/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/

#define __stm32f2xx_init__

/* Includes ------------------------------------------------------------------*/
#include "bit_define.h"
#include "stm32f2xx.h"
#include "stm32f2xx_init.h"
#include "uart2.h"
#include "uart3.h"






/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/******************************************************************************************************/
/* Internal_timer_Proc : 평상시 tmxfg는 항상 "1" 이다	(TimerSetup 시 tmxg를 "0"으로 만든다)	      */ 											
/******************************************************************************************************/
void Internal_timer_Proc() {				
//u8 ix;
static u16 tm1sec;

	if(tm1ms_f) { tm1ms_f =0;	//every 1msec 	 
		watchdog.s.b4 =1; 			// watchdog setup
    

/*    
		if(!tm0fg) {	 if(!--timer_buf[0]) tm0fg =1;	 } 	 
		if(!tm1fg) {	 if(!--timer_buf[1]) tm1fg =1;	 }
		if(!tm2fg) {	 if(!--timer_buf[2]) tm2fg =1;	 }
		if(!tm3fg) {	 if(!--timer_buf[3]) tm3fg =1;	 }
		if(!tm4fg) {	 if(!--timer_buf[4]) tm4fg =1;	 }
		if(!tm5fg) {	 if(!--timer_buf[5]) tm5fg =1;	 }
		if(!tm7fg) {	 if(!--timer_buf[7]) tm7fg =1;	 } 

		if(!tm8fg) {	 if(!--timer_buf[8]) tm8fg =1;	 } 
		if(!tm9fg) {	 if(!--timer_buf[9]) tm9fg =1;	 } 

		if(!tm14fg) {  if(!--timer_buf[14]) tm14fg =1;  } 
		if(!tm15fg) {  if(!--timer_buf[15]) tm15fg =1;  } 
		if(!tm6fg) {	 if(!--timer_buf[6]) tm6fg =1;	 }
		for(ix=0; ix<sizeof(extm.buf); ix++) { if(extm.buf[ix]) extm.buf[ix]--;
		}
*/
    
		if(++tm1sec >= 500 ) { tm1sec =0; P_led2 ^= 1;
		}
    
    
    if(++uart3_call_timer > 50){
      uart3_call_timer = 0;
      f_uart3_send_time = 1;
    }

    if(++sign_led_pc_tx > 10){
      sign_led_pc_tx = 0;
      f_sign_pc_tx = 0;
    }
    if(++sign_led_pc_rx > 10){
      sign_led_pc_rx = 0;
      f_sign_pc_rx = 0;
    }
    if(++sign_led_sys_tx > 10){
      sign_led_sys_tx = 0;
      f_sign_sys_tx = 0;
    }
    if(++sign_led_sys_rx > 10){
      sign_led_sys_rx = 0;
      f_sign_sys_rx = 0;
    }
    sign_led_drive();
    

    
	}
}


//
//======== Sign LED Display routine =========
//
void sign_led_drive(void)
{
  GPIO_Write(GPIOG, 0xc000);  
  if(f_sign_pc_tx) P_led_y0 = 1;
  else P_led_y0 = 0;
  if(f_sign_pc_rx) P_led_y1 = 1;
  else P_led_y1 = 0;
  
  if(f_sign_sys_tx) P_led_y2 = 1;
  else P_led_y2 = 0;
  if(f_sign_sys_rx) P_led_y3 = 1;
  else P_led_y3 = 0;
  
  f_sign_ex_tx = 0;
  if(f_sign_ex_tx) P_led_y4 = 1;
  else P_led_y4 = 0;
  f_sign_ex_rx = 0;
  if(f_sign_ex_rx) P_led_y5 = 1;
  else P_led_y5 = 0;
  
  P_led_y6 ^= 1;
}


//void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);

/******************************************************************************************************/
/* TimerSetup : 해당 tmxfg를 "0"으로 만든 후 원하는 timer 값을 처리한다.		              */ 											
/******************************************************************************************************/
void TimerSetup(uchar tmno, u16 timer) 
{

  Clrbit(&tm.dword, tmno);
	timer_buf[tmno] =timer;
}

void Tgbit(u32 *reg, u8 Bitno) {
  *reg ^=0x00000001<<Bitno;	
}
void Setbit(u32 *reg, u8 Bitno) {
  *reg |=0x00000001<<Bitno;	
}
void Clrbit(u32 *reg, u8 Bitno) { 
  *reg &=~(0x00000001<<Bitno);	
}
u8 Chkbit(u32 *reg, u8 Bitno) {	
	if(*reg & (0x00000001<<Bitno)) return 1;
	return 0;
}

/******************************************************************************************************/
/* timer base bit initial :		                                                              */																						
/******************************************************************************************************/
void Gp_initial(void){
u8 ix;

	tm.dword =(s32)-1;
	for(ix=0; ix<sizeof(timer_buf)/2; ix++) timer_buf[ix] =1;
	Wdtcnt =5000;
	
}

/******************************************************************************************************/
/* GPIO_port_initial	:								              */												
/******************************************************************************************************/
void GPIO_port_initial(void){
GPIO_InitTypeDef GPIO_InitStructure; 
  
    /* Enable GPIOF clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOB |RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD |RCC_AHB1Periph_GPIOE \
    		| RCC_AHB1Periph_GPIOF |RCC_AHB1Periph_GPIOG |RCC_AHB1Periph_GPIOH |RCC_AHB1Periph_GPIOI, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_1 | GPIO_Pin_8;    //U2dir, U1dir
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;    //U3dir, U4dir, U5dir
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
    
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;    //Dip switch 1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
    
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_13;    //TC Reset
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;    //Ex switch 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  //LED Y0~16
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |	GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;    //Dip switch 2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  //Sign LED
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  P_led1 = P_led2 = P_led3 = P_led4 = P_led5 = P_led6 = P_led7 = P_led8 = 1;

  //LED X0~16
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |	GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  
  P_u1dir = P_u2dir = P_u3dir = P_u4dir = P_u5dir = 0;
//  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_6;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);

}

/******************************************************************************************************/
/* MCO_initial	:								              */												
/******************************************************************************************************/
void MCO_initial(void){
    /* Enable GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    /* ConfigurePA.09 as output push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	 RCC_MCO1Config(RCC_MCO1Source_HSI, RCC_MCO1Div_2);
}

/******************************************************************************************************/
/* TIME2_initial	:								              */												
/******************************************************************************************************/
void TIME2_initial(void){
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* TIM2 clock enable */   //max - 60MHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 59;  				//TIM2 Clock / 60 
  TIM_TimeBaseStructure.TIM_Period = 1000;    				//1Mhz/1000=->1msec timer      
  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 
  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);

  // Enable the USART1 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	 
}

void Iwdog_initial(u16 wtime) {

  /* IWDG timeout equal to 350ms (the timeout may varies due to LSI frequency
     dispersion) -------------------------------------------------------------*/
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: 32KHz(LSI) / 32 = 1KHz */
  IWDG_SetPrescaler(IWDG_Prescaler_32);

  /* Set counter reload value to 349 */
  IWDG_SetReload(wtime-1);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();   
}



void NVIC_initial(void){
NVIC_InitTypeDef   NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
 /* Set the Vector Table base location at 0x20000000 */ 
 NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
 /* Set the Vector Table base location at 0x08000000 */ 

#ifdef __IAP_APP_USE__
 NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);   
#else
 NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);   
#endif
#endif

 /* Enable and configure RCC global IRQ channel */
 NVIC_InitStructure.NVIC_IRQChannel = RCC_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure); 

 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

/******************************************************************************************************/
/* CORTEX_initial	:		        						      */												
/******************************************************************************************************/
void Stm32f2xx_initial(void) {
//  	unsigned i,j;
  unsigned int i;

  NVIC_initial();
	Gp_initial();
 	GPIO_port_initial();
	TIME2_initial();
//	MCO_initial();
  UART2_initial();
  UART3_initial();
  
  for(i=0; i<cb_max_count; ++i){      //RAM Initail(0위치에 'R'이 있을때만 해당 data 전송한다)
    cb_rx_room_data_buf[i][0] = 0xff;
  }

  f_power_on = 0;
  bojung_flag = 0x00;   //h@20140704
  Iwdog_initial(3000);


}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

