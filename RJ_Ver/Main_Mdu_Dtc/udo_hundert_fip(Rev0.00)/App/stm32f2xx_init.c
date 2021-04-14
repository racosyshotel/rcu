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
u8 ix;
static u16 tm1sec;

	if(tm1ms_f) { tm1ms_f =0;	//every 1msec 	 
		watchdog.s.b4 =1; 			// watchdog setup
    
    fip_led_drive();
    dip_switch_read();
    if(++led_scan_time > 250){ led_scan_time = 0; fip_led_move();  }

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

		if(++tm1sec >=1000 ) { tm1sec =0;
			if(!tm6fg) {	 if(!--timer_buf[6]) tm6fg =1;	 }
		}
		for(ix=0; ix<sizeof(extm.buf); ix++) { if(extm.buf[ix]) extm.buf[ix]--;
		}
    
    
    if(++uart3_call_timer > 20){
      uart3_call_timer = 0;
      f_uart3_send_time = 1;
      f_uart2_send_time = 1;
    }
    
    if(f_uart3_data_send){        //h@20150127 =>
      if(!f_uart3_delay_time){
        if(++tx_delay_timer > 1){
          tx_delay_timer = 0;
          f_uart3_delay_time = 1;
        }
      }
    }                             //<= h@20150127


    
	}
}
#if 0
timer0 : 
timer1 : uart1
timer2 : uart2
timer3 :  
timer4 : uart4
timer5 :  
timer6 : backlight
timer7 :  
	
timer8 : lcd oper
timer9 : lcd oper

timer14 :
timer15 :

#endif

  
  
void fip_led_drive(void)
{
  switch(led_scan_count){
    case 0: GPIO_Write(GPIOG, 0x0001);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 1: GPIO_Write(GPIOG, 0x0002);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 2: GPIO_Write(GPIOG, 0x0004);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 3: GPIO_Write(GPIOG, 0x0008);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 4: GPIO_Write(GPIOG, 0x0010);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 5: GPIO_Write(GPIOG, 0x0020);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 6: GPIO_Write(GPIOG, 0x0040);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 7: GPIO_Write(GPIOG, 0x0080);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    
    case 8: GPIO_Write(GPIOG, 0x0100);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 9: GPIO_Write(GPIOG, 0x0200);  GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 10: GPIO_Write(GPIOG, 0x0400); GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 11: GPIO_Write(GPIOG, 0x0800); GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 12: GPIO_Write(GPIOG, 0x1000); GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 13: GPIO_Write(GPIOG, 0x2000); GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 14: GPIO_Write(GPIOG, 0x4000); GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 15: GPIO_Write(GPIOG, 0x8000); GPIO_Write(GPIOE, led_y_buf[led_scan_count]); ++led_scan_count;  break;
    case 16: GPIO_Write(GPIOG, 0x0000); led_scan_count = 0;  break;
    default:	led_scan_count =0;	break;
  }
}

void fip_led_move(void)
{
	unsigned i,j;
        
	if(++led_shift_buf >= 4)	led_shift_buf = 0;
        
	for(i=0; i<16; ++i)
        {
		for(j=0; j<16; ++j)
                {
			if((led_kind_buf[i][j] >> led_shift_buf) & 0x01)	led_y_buf[i] |= (0x0001 << j);
			else led_y_buf[i] &= ((0x0001 << j) ^ 0xffff);
		}
	}  
}

void dip_switch_read(void)
{
  unsigned char temp_dip = 0;
  
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)) temp_dip |= 0x01;
  
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)) temp_dip |= 0x02;
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)) temp_dip |= 0x04; 
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)) temp_dip |= 0x08; 
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)) temp_dip |= 0x10;
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)) temp_dip |= 0x20;
  
  my_device_no = temp_dip;
  
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)) dip_function &= 0xfe;     //Dip switch 2번에 7번 on=청소지시 시간 처리
  else dip_function |= 0x01;
  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)) dip_function &= 0xfd;    //Dip switch 2번에 8번 on=FIP LED ( 객실(DND,MUR)상태 or 통신상태 LED 모드 전환)
  else dip_function |= 0x02;
  
  if((my_device_no >= 0) && (my_device_no <= 16))
  {
      dong_code = 0x01;
  }
  else dong_code = 0x02;
  
  temp_dip = 0;
  if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_0)) dip_function_2 &= 0xfe;  //Dip switch 1번의 1번 : 재실시, 화장실 전등 ON/OFF
  else dip_function_2 |= 0x01;
  if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_1)) dip_function_2 &= 0xfd;  //Dip switch 1번의 2번 : 기구물 후면부 LED  밝기조절
  else dip_function_2 |= 0x02;
  
  if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_2)) temp_dip |= 0x04; 
  if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_3)) temp_dip |= 0x08; 
  if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_4)) temp_dip |= 0x10; 
  if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_5)) temp_dip |= 0x20; 
  if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_6)) temp_dip |= 0x40; 
  if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7)) temp_dip |= 0x80; 
  dip_switch_buf_2 = temp_dip;
  
/*  if(!f_illumination_ok){
    common_data_buf[23] = dip_switch_buf_2;
    if(dip_switch_buf_2 != dip_switch_buf_2_back){
      dip_switch_buf_2_back = dip_switch_buf_2;
      cb_first_send_save(0xfe,'P');
    }
  }*/
  
//dip switch function
//Dip switch #1 
//No  1, 2, 3, 4, 5, 6  ==>   Device ID
//No  7
//No  8   ON  = check temp priority send Enable
//        OFF = check temp priority send Disable
  
//Dip switch #2(Function)
//No  1     2
//    On    On    No action
//    Off   On    Illumination always ON
//    On    Off   Illumination touch high after auto low
//    Off   Off   Illumination CDS value
//No  3     ON = High Light, OFF = Low light
//No  4     N.C
//No  5, 6, 7, 8 = High light keep time(sec)
  
 

  
}

//void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);

/******************************************************************************************************/
/* TimerSetup : 해당 tmxfg를 "0"으로 만든 후 원하는 timer 값을 처리한다.		              */ 											
/******************************************************************************************************/
void TimerSetup(uchar tmno, u16 timer) {

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
  TIM_TimeBaseStructure.TIM_Prescaler = 59;  				//
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
  unsigned char i,j;
  unsigned int z;
  unsigned char comp_sram[10];
  unsigned char flag_comp;

  NVIC_initial();
	Gp_initial();
 	GPIO_port_initial();
	TIME2_initial();
//	MCO_initial();
  UART2_initial();
  UART3_initial();
  
	for(i=0; i<16; ++i){
		for(j=0; j<16; ++j){
			led_kind_buf[i][j] = 0;//0x0c;
		}
	}  
//  led_kind_buf[0][0] = 0x0f;

  f_illumination_ok = 0;
  f_power_on_start = 0;
  f_main_send_condition = 0;
  Iwdog_initial(3000);
  
  cb_max_count[0] = 28; //0번 DTC [해당 DTC1에 대한 ACU 개수]
  cb_max_count[1] = 20; //1번 DTC [해당 DTC2에 대한 ACU 개수]
  
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;      //This is to to enable access to PWR... 
  PWR-> CR |= PWR_CR_DBP;                 //This is to enable access to the BRE bit below... 
  PWR-> CSR |= PWR_CSR_BRE;               //This is to enable the SRAM in standby mode... 
  while((PWR->CSR & PWR_CSR_BRR)==0) ;    //Wait till the change becomes effective 
  RCC->AHB1ENR |= RCC_AHB1ENR_BKPSRAMEN;  //This is to enable SW access... 
   
  PWR->CR |= PWR_CR_CWUF; 
    
  flag_comp = 0;
  for(i=0; i<10; ++i){
    comp_sram[i] = Read_BKPSRAM(i);
    if(comp_sram[i] != i) flag_comp = 1;
  }
  flag_comp = 0;
  if(flag_comp){    //Factory setting
    for(z=1; z<0xff; ++z) Write_BKPSRAM(b_sram_check_io_base+(z*4),0);
    for(z=1; z<0xff; ++z) Write_BKPSRAM(b_sram_clean_base+(z*4),0);
    for(z=1; z<0xff; ++z) Write_BKPSRAM(b_sram_room_base+(z*4),0);
    for(z=0; z<cb_max_count[my_device_no]; ++z) cb_send_data_buf[z][2] = 0x01;    //check-in set
    for(z=0; z<cb_max_count[my_device_no]; ++z) cb_rx_room_data_buf[z][60] = 2;   //clean finish set
    for(z=0; z<cb_max_count[my_device_no]; ++z) cb_rx_room_data_buf[z][2] = 0;    //Going out set
    common_data_buf[1] = 0x33;    //외출온도차
    common_data_buf[2] = 0x50;    //이상온도
    common_data_buf[3] = 0x10;    //대기난방공실
    common_data_buf[4] = 0x30;    //대기냉방공실
    common_data_buf[5] = 0x10;    //바닥공실
    common_data_buf[6] = 0x55;    //대기상하한
    common_data_buf[7] = 0xaa;    //바닥상하한
    common_data_buf[8] = 0xff;    //년
    common_data_buf[9] = 0xff;    //원
    common_data_buf[10] = 0xff;   //일
    common_data_buf[11] = 0xff;   //시 
    common_data_buf[12] = 0xff;   //분
    common_data_buf[13] = 0x00;   //입구점등 
    common_data_buf[14] = 60;     //입구점등
    common_data_buf[15] = 0x12;   //청소지시 시
    common_data_buf[16] = 0x00;   //청소지시 분
    common_data_buf[17] = 0xff;   //대기일제전체 
    common_data_buf[18] = 0xff;   //바닥일제전체
    common_data_buf[19] = 0xff;   //대기일제공실 
    common_data_buf[20] = 0xff;   //바닥일제공실
    common_data_buf[21] = 10;     //차임볼륨
    common_data_buf[22] = 10;     //알람볼륨
    common_data_buf[23] = 0;      //조도조절
    common_data_buf[24] = 1;      //온도센서 표시방법
    for(z=1; z<25; ++z) Write_BKPSRAM(b_sram_global_data_base+z,common_data_buf[z]);
    
    P_led7 = 0;
  }
  else{             //Backup memory reload
      
      for(z=0; z<64; ++z){
        cb_send_data_buf[z][2] = Read_BKPSRAM(b_sram_check_io_base+(z*4));  //check-io read
        cb_send_data_buf[z][2] &= 0xf7; // 예냉예열 OFF
      }
      for(z=0; z<64; ++z){
        cb_rx_room_data_buf[z][60] = Read_BKPSRAM(b_sram_clean_base+(z*4)); //clean status read
      }
      for(z=0; z<64; ++z){
        cb_rx_room_data_buf[z][2] = Read_BKPSRAM(b_sram_room_base+(z*4));   //room(k/s) status read
      }
      for(z=1; z<30; ++z){
        common_data_buf[z] = Read_BKPSRAM(b_sram_global_data_base+(z*4));   //common data read
      }
     
  }
  
  for(i=0; i<10; ++i) Write_BKPSRAM(i,i);   //write code set
  
  bojung_flag = 0;    //h@20140704  
  common_send_count = 5;  //h@20150127
}

//LED1 = Main communication send
//LED2 = Timer operation sign
//LED3 = Sub communiction CRC error
//LED4 = Sub communiction send
//LED5 = Backup sram factiory setting
//LED6 = Main communication rx ok
//LED7 = Main common data rx
//LED8 = Sub communication rx ok


//LED1 = Main communication send
//LED2 = Main communication rx ok
//LED3 = Sub communiction send
//LED4 = Sub communication rx ok
//LED5 = 
//LED6 = Backu memory compareok
//LED7 = Backup sram factiory setting
//LED8 = Timer operation sign




/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

