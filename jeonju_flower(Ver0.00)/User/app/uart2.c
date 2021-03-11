/*=====================(C) COPYRIGHT 2008 Insem Inc.=========================
program 		:   
processor	  : STM32F103xx
compiler		: IAR 6.41A Compiler 
program BY 	: 울릉도 목업룸 (A800 MOLEX)
date 			  : 2013.	  .
copy right 	: Plus - H.
===========================================================================*/

#define __UART2_H__

#include "stm32f10x_lib.h"
#include "main.h"

//=======================================================================
/* UART1_initial	:								              */												
//=======================================================================
void UART2_initial(void){
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

  /* RS485 UART2 Direction port define */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
        
	/* Enable GPIOA and USART2 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	 /* Enable USART2 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  
  /* Configure USART2 Tx (PA9) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 Rx (PA10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

/* USART2 configuration ------------------------------------------------------*/
  /* USART2 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate = 19200;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//  USART_InitStructure.USART_Clock = USART_Clock_Disable;
//  USART_InitStructure.USART_CPOL = USART_CPOL_Low;
//  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
//  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

  /* Configure the USART1 */
  USART_Init(USART2, &USART_InitStructure);

/* Enable the USART Transmoit interrupt: this interrupt is generated when the 
   USART1 transmit data register is empty */  
//  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

/* Enable the USART Receive interrupt: this interrupt is generated when the 
   USART1 receive data register is not empty */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  /* Enable USART1 */
  USART_Cmd(USART2, ENABLE);

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



//=======================================================================
void uart2_frame_proc() 
{

  UART2_tx_check();
  UART2_rx_check();
  
}

/* UART1 data arrange routine */
void volume_jig_data_set(void)
{
  unsigned char crc = 0;
  unsigned char i = 0,j;
  
  P_UART2_DIR = 1;
  uart2_tx_data_buf[i++] = ' ';                               //Preamble
  uart2_tx_data_buf[i++] = '(';                               //STX
  ++i;
  uart2_tx_data_buf[i++] = 0x01;                           //Device bit
  uart2_tx_data_buf[i++] = 0xca;                           //Device ID
  uart2_tx_data_buf[i++] = room_data_buf[79];             //Data#1
  uart2_tx_data_buf[i++] = room_data_buf[80];             //Data#n
  uart2_tx_data_buf[2] = i+1;                               //Length
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                               //CRC
  uart2_tx_data_buf[i++] = ')';                               //ETX

  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;

  
 
}

//
//============ Thermo sensor data arrange routine =======================
//
void Thermo_sensor_data_set(unsigned char index, unsigned char index_bit)
{
  unsigned char crc = 0;
  unsigned char i=0,j;
  
  P_UART2_DIR = 1;
  ++i;                                                        //Length point(아래에서 처리)
  uart2_tx_data_buf[i++] = index+0x10;                        //Device ID
  
  if(index_bit & 0x01){
    if(f_ts_power_on[0]){
      uart2_tx_data_buf[i++] = room_data_buf[0];
    }
    else uart2_tx_data_buf[i++] = ts_air_set_temp_from_PC[0];
    uart2_tx_data_buf[i++] = ts_floor_set_temp_from_PC[0];
    uart2_tx_data_buf[i++] = ts_switch_toggle_bit[0];
    uart2_tx_data_buf[i++] = ts_status_read(0);
    uart2_tx_data_buf[i++] = ts_control_data_read(0);
  }
  if(index_bit & 0x02){
    if(f_ts_power_on[1])  uart2_tx_data_buf[i++] = room_data_buf[1];
    else uart2_tx_data_buf[i++] = ts_air_set_temp_from_PC[1];
    uart2_tx_data_buf[i++] = ts_floor_set_temp_from_PC[1];
    uart2_tx_data_buf[i++] = ts_switch_toggle_bit[1];
    uart2_tx_data_buf[i++] = ts_status_read(1);
    uart2_tx_data_buf[i++] = ts_control_data_read(1);
  }
  if(index_bit & 0x04){
    if(f_ts_power_on[2])  uart2_tx_data_buf[i++] = room_data_buf[2];
    else uart2_tx_data_buf[i++] = ts_air_set_temp_from_PC[2];
    uart2_tx_data_buf[i++] = ts_floor_set_temp_from_PC[2];
    uart2_tx_data_buf[i++] = ts_switch_toggle_bit[2];
    uart2_tx_data_buf[i++] = ts_status_read(2);
    uart2_tx_data_buf[i++] = ts_control_data_read(2);
  }
  if(index_bit & 0x08){
    if(f_ts_power_on[3])  uart2_tx_data_buf[i++] = room_data_buf[4];
    else uart2_tx_data_buf[i++] = ts_air_set_temp_from_PC[3];
    uart2_tx_data_buf[i++] = ts_floor_set_temp_from_PC[3];
    uart2_tx_data_buf[i++] = ts_switch_toggle_bit[3];
    uart2_tx_data_buf[i++] = ts_status_read(3);
    uart2_tx_data_buf[i++] = ts_control_data_read(3);
  }
  uart2_tx_data_buf[i++] = room_data_buf[28];               //대기 상.하한 온도
  uart2_tx_data_buf[i++] = room_data_buf[29];               //바닥 상.하한 온도
  uart2_tx_data_buf[i++] = room_data_buf[30];               //외기 온도
  
  uart2_tx_data_buf[i++] = bojung_temp[index];              //보정온도 h@20140704
  if(!f_temp_test_mode)
  {
    uart2_tx_data_buf[i++] = aircon_no_respones[index];   //에어컨 에러코드(1 = 통신 미연결 / 0 = 통신 연결)
  }
  else uart2_tx_data_buf[i++] = 0; //test mode Error code clear  
  
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                             //CRC
  uart2_tx_data_buf[0] = i;                                 //Length

  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
  
  if(++ts_set_temp_send_count[index] > 3){
    ts_set_temp_send_count[index] = 5;
    ts_air_set_temp_from_PC[index] = 0xff;
    ts_floor_set_temp_from_PC[index] = 0xff;
  }
  
  timer_cnt = 30;
}

unsigned char ts_status_read(unsigned char index)
{
  unsigned char return_data = 0;
  
  if(room_data_buf[40] & 0x01) return_data |= 0x01;         //Check IN 
  if(room_data_buf[40] & 0x02) return_data |= 0x02;         //Guest IN
  if(room_data_buf[20+index] & 0x04) return_data |= 0x04;   //Cool mode
  switch(room_data_buf[24+index] & 0x30)
  {
    case 0x00: return_data |= 0x00;    break;               //대기전용
    case 0x10: return_data |= 0x08;    break;               //바닥전용
    case 0x20: return_data |= 0x10;    break;               //대기/바닥 동시제어
  }
  if(room_data_buf[40] & 0x10) return_data |= 0x20;         //Windows OPEN
  if(room_data_buf[74] == 2) return_data |= 0x40;           //환절기 mode
  if(room_data_buf[41] & 0x08) return_data |= 0x80;         //온도표시방법 1=설정온도표시, 0=현재온도표시
  return(return_data);
}
unsigned char ts_control_data_read(unsigned char index)
{
  unsigned char return_data = 0;
  
  return_data = room_data_buf[16+index] & 0x03;             //Speed data
  if(room_data_buf[16+index] & 0x10) return_data |= 0x04;   //바닥밸브 open or close?
  if(room_data_buf[16+index] & 0x04) return_data |= 0x08;   //대기센서 스위치 ON/OFF?
  if(room_data_buf[16+index] & 0x20) return_data |= 0x10;   //바닥센서 스위치 ON/OFF?
  //  if(room_data_buf[24+index] & 0x08) return_data |= 0x20;   //화씨 or 도씨?       //h@20150910
  //if(room_data_buf[20+index] & 0x40) return_data |= 0x20;       //송풍모드          //h@20150910
  if(room_data_buf[20+index] & 0x08) return_data |= 0x40;   //Auto or Manual mode
  if(f_ts_power_on[index] == 1) return_data |= 0x80;   //Auto or Manual mode
  return(return_data);
}

//
//================ Night Table data arrange routine ===================
//
void night_table_data_set(unsigned char index, unsigned char index_bit)
{
  unsigned char crc = 0;
  unsigned char i=0,j;
  unsigned char led_temp = 0;
 
  P_UART2_DIR = 1;
  ++i;                                                        //Length point(아래에서 처리)
  uart2_tx_data_buf[i++] = index + 0x20;                      //Device ID
  
  if(index_bit & 0x01){
   switch(cb_mode)
   {
    case 0: //RCB - 1
      
            if(room_data_buf[45] & 0x02) led_temp |= 0x01; //ROOM 1
            if(room_data_buf[45] & 0x04) led_temp |= 0x08; //ROOM 2 
            if(room_data_buf[45] & 0x08) led_temp |= 0x02; //ROOM 3
            if(room_data_buf[45] & 0x10) led_temp |= 0x10; //ROOM 4
            if(room_data_buf[45] & 0x20) led_temp |= 0x04; //ROOM 5
            if(((room_data_buf[45] & 0xfe) == 0)&& ((room_data_buf[46] & 0x03) == 0)) led_temp |= 0x20; //MASTER
            
       break;     
   }
   
    uart2_tx_data_buf[i++] = led_temp;
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][0];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][1];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][2];
    uart2_tx_data_buf[i++] = nt_control_data_read(index);
  }
  if(index_bit & 0x02){
    
   switch(cb_mode)
   {
    case 1: //RCB - 2, RCB - 3
      if(room_data_buf[47] & 0x01) led_temp |= 0x01;
      if(room_data_buf[47] & 0x02) led_temp |= 0x02;
      if(room_data_buf[47] & 0x04) led_temp |= 0x04;
      if(room_data_buf[47] & 0x08) led_temp |= 0x08;
      if(room_data_buf[47] & 0x10) led_temp |= 0x10;
      if((room_data_buf[47] & 0x1f) == 0) led_temp |= 0x20;
      
      if(room_data_buf[40] & 0x04) led_temp |= 0x40;  //dnd
      if(room_data_buf[40] & 0x08) led_temp |= 0x80; //mur
      break;
   }    
    
    uart2_tx_data_buf[i++] = led_temp;
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][0];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][1];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][2];
    uart2_tx_data_buf[i++] = nt_control_data_read(index);
  }
  if(index_bit & 0x04){
    uart2_tx_data_buf[i++] = room_data_buf[index+50];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][0];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][1];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][2];
    uart2_tx_data_buf[i++] = nt_control_data_read(index);
  }
  if(index_bit & 0x08){
    uart2_tx_data_buf[i++] = room_data_buf[index+50];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][0];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][1];
    uart2_tx_data_buf[i++] = nt_switch_toggle_bit[index][2];
    uart2_tx_data_buf[i++] = nt_control_data_read(index);
  }
  uart2_tx_data_buf[i++] = nt_room_status();
  
  uart2_tx_data_buf[i++] = room_data_buf[90];   //year
//  uart2_tx_data_buf[i++] = room_data_buf[91];   //month
  if(f_ts_power_on[index])  uart2_tx_data_buf[i++] = room_data_buf[0];  //TS set temp
  else uart2_tx_data_buf[i++] = ts_air_set_temp_from_PC[0];
//  uart2_tx_data_buf[i++] = room_data_buf[92];   //day
  //uart2_tx_data_buf[i++] = room_data_buf[5];    //TS2 check temp
  
  uart2_tx_data_buf[i++] = room_data_buf[93];   //hour
  uart2_tx_data_buf[i++] = room_data_buf[94];   //min
  
  uart2_tx_data_buf[i++] = room_data_buf[28];               //대기 상.하한 온도
  uart2_tx_data_buf[i++] = bojung_temp[0];              //보정온도 h@20140704
  
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  
  uart2_tx_data_buf[i++] = crc;                             //CRC
  uart2_tx_data_buf[0] = i;                                 //Length
  
  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
  if(++ts_set_temp_send_count[0] > 3){
    ts_set_temp_send_count[0] = 5;
    ts_air_set_temp_from_PC[0] = 0xff;
    ts_floor_set_temp_from_PC[0] = 0xff;
  }
  timer_cnt = 38;
}

unsigned char nt_control_data_read(unsigned char index)
{
  unsigned char return_data = 0;
/* 
  return_data = room_data_buf[16+index] & 0x03;             //Speed data
  if(room_data_buf[16+index] & 0x04) return_data |= 0x04;   //TS ON/OFF status
  if(room_data_buf[20+index] & 0x04) return_data |= 0x08;   //Cool or warm
  if(room_data_buf[36+index] & 0x08) return_data |= 0x10;   //Radio on?
  return_data |= ((room_data_buf[36+index] & 0x07) << 5);   //Radio channel
  return(return_data);
*/
    return_data = room_data_buf[16] & 0x03;             //Speed data
    if(room_data_buf[16] & 0x04) return_data |= 0x04;   //TS ON/OFF status
    if(room_data_buf[20] & 0x04) return_data |= 0x08;   //Cool or warm
    
    if(room_data_buf[16] & 0x20) return_data |= 0x10;   //TS ON/OFF status (11.13 바닥난방)
    if(room_data_buf[16] & 0x10) return_data |= 0x20;   //바닥난방 밸브제어
    
    //if(room_data_buf[36+index] & 0x08) return_data |= 0x10;   //Radio on?
    //return_data |= ((room_data_buf[36+index] & 0x07) << 5);   //Radio channel
  return(return_data);  
}

unsigned char nt_room_status(void)
{
  unsigned char return_data = 0;
  
  if(room_data_buf[40] & 0x01) return_data |= 0x01;         //Check IN 
  if(room_data_buf[40] & 0x02) return_data |= 0x02;         //Guest IN
  if(room_data_buf[20] & 0x04) return_data |= 0x04;         //Cool mode
  if(room_data_buf[40] & 0x04) return_data |= 0x08;         //DND
  if(room_data_buf[40] & 0x08) return_data |= 0x10;         //MUR
  if(room_data_buf[40] & 0x10) return_data |= 0x20;         //Window
  if(room_data_buf[40] & 0x20) return_data |= 0x40;         //Message
  if(room_data_buf[40] & 0x40) return_data |= 0x80;         //Door open
  return(return_data);
  
}
//
//================ Light switch data arrange routine ===================
//
void light_switch_data_set(unsigned char index, unsigned int index_bit)
{
  unsigned char crc = 0;
  unsigned char i=0,j;
  
  P_UART2_DIR = 1;                 
  ++i;                                                        //Length point(아래에서 처리)
  uart2_tx_data_buf[i++] = index + 0x30;                      //Device ID
  
  if(index_bit & 0x01) // 객실 스위치
  {
          uart2_tx_data_buf[i] = 0;
          
          switch(cb_mode)
          {
          case 0: //RCB - 1 (객실 1 6구)
            
            if(room_data_buf[45] & 0x02) uart2_tx_data_buf[i] |= 0x01; // LIGHT 1
            if(room_data_buf[45] & 0x04) uart2_tx_data_buf[i] |= 0x02; // LIGHT 2
            if(room_data_buf[45] & 0x08) uart2_tx_data_buf[i] |= 0x04; // LIGHT 3
            if(room_data_buf[45] & 0x10) uart2_tx_data_buf[i] |= 0x08; // LIGHT 4
            if(room_data_buf[45] & 0x20) uart2_tx_data_buf[i] |= 0x10; // LIGHT 5
               
            if((room_data_buf[45] & 0x3e)==0) uart2_tx_data_buf[i] |= 0x20; //MASTER
            
            break;
            
          case 1: //RCB - 2 (객실 1)
          
            if(room_data_buf[45] & 0x02) uart2_tx_data_buf[i] |= 0x01; // LIGHT 1
            if(room_data_buf[45] & 0x04) uart2_tx_data_buf[i] |= 0x02; // LIGHT 2
            if(room_data_buf[45] & 0x08) uart2_tx_data_buf[i] |= 0x08; // LIGHT 3
            if(room_data_buf[45] & 0x10) uart2_tx_data_buf[i] |= 0x10; // LIGHT 4
               
            if((room_data_buf[45] & 0x1e)==0) uart2_tx_data_buf[i] |= 0x04; //MASTER
             
            break;
          }
          ++i;
          uart2_tx_data_buf[i++] = light_switch_toggle_bit[index][0];
          uart2_tx_data_buf[i++] = ls_control_data_read(index);
  }
  if(index_bit & 0x02) // RCB - 2 (객실 2)
  {
    uart2_tx_data_buf[i] = 0;
    
     if(room_data_buf[47] & 0x01) uart2_tx_data_buf[i] |= 0x01; // LIGHT 1
     if(room_data_buf[47] & 0x02) uart2_tx_data_buf[i] |= 0x02; // LIGHT 2
     if(room_data_buf[47] & 0x04) uart2_tx_data_buf[i] |= 0x08; // LIGHT 3
     if(room_data_buf[47] & 0x08) uart2_tx_data_buf[i] |= 0x10; // LIGHT 4
               
     if((room_data_buf[47] & 0x0f)==0) uart2_tx_data_buf[i] |= 0x04; //MASTER
             
    
    ++i;
    uart2_tx_data_buf[i++] = light_switch_toggle_bit[index][0];
    uart2_tx_data_buf[i++] = ls_control_data_read(index);
  }
  if(index_bit & 0x04) 
  {
    uart2_tx_data_buf[i] = 0;
    

    ++i;
    uart2_tx_data_buf[i++] = light_switch_toggle_bit[index][0];
    uart2_tx_data_buf[i++] = ls_control_data_read(index);      
  }
  if(index_bit & 0x08) 
  {
    uart2_tx_data_buf[i] = 0;
    
    ++i;
    uart2_tx_data_buf[i++] = light_switch_toggle_bit[index][0];
    uart2_tx_data_buf[i++] = ls_control_data_read(index);      
  }  
  if(index_bit & 0x10) 
  {
    uart2_tx_data_buf[i] = 0;

      ++i;
      uart2_tx_data_buf[i++] = light_switch_toggle_bit[index][0];
      uart2_tx_data_buf[i++] = ls_control_data_read(index);            
  }
  
  uart2_tx_data_buf[i++] = cb_mode; //RCU MODE
  
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                             //CRC
  uart2_tx_data_buf[0] = i;                                 //Length
  
  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
   timer_cnt = 0;
}

unsigned char ls_control_data_read(unsigned char index)
{
  unsigned char return_data = 0;
  
  if(room_data_buf[40] & 0x01) return_data |= 0x01;         //Check IN 
  if(room_data_buf[40] & 0x02) return_data |= 0x02;         //Guest IN
 // if(room_data_buf[40] & 0x80) return_data |= 0x04;         //Please wait sign
  if(room_data_buf[40] & 0x04) return_data |= 0x08;         //DND
  if(room_data_buf[40] & 0x08) return_data |= 0x10;         //MUR
  if(room_data_buf[40] & 0x10) return_data |= 0x20;         //Window
  if(room_data_buf[40] & 0x20) return_data |= 0x40;         //Message
  if(room_data_buf[40] & 0x40) return_data |= 0x80;         //Door open
  return(return_data);
}


//
//========== Key sensor data arrange =============
//
void key_sendor_data_set(unsigned char index,unsigned char index_bit)
{
  unsigned char crc = 0;
  unsigned char i=0,j;
 
  P_UART2_DIR = 1;
  ++i;                                                        //Length point(아래에서 처리)
  uart2_tx_data_buf[i++] = index + 0x60;                      //Device ID
  
  if(index_bit & 0x01){
    uart2_tx_data_buf[i++] = ks_switch_toggle_bit[index];
    uart2_tx_data_buf[i++] = ks_control_data_read(index);
  }
  if(index_bit & 0x02){
    uart2_tx_data_buf[i++] = ks_switch_toggle_bit[index];
    uart2_tx_data_buf[i++] = ks_control_data_read(index);
  }

  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                             //CRC
  uart2_tx_data_buf[0] = i;                                 //Length
  
  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
   timer_cnt = 0;
  
}

unsigned char ks_control_data_read(unsigned char index)
{
  unsigned char return_data = 0;
  
  if(room_data_buf[40] & 0x01) return_data |= 0x01;         //Check IN 
  if(room_data_buf[40] & 0x02) return_data |= 0x02;         //Guest IN
  if(room_data_buf[40] & 0x04) return_data |= 0x10;         //DND
  if(room_data_buf[40] & 0x08) return_data |= 0x20;         //MUR
  
  //if(room_data_buf[46] & 0x04) return_data |= 0x10;         //전열교환기 상태
  //if(room_data_buf[45] & 0x02) return_data |= 0x20;         //입구등 상태
  return(return_data);
}

void dimmer_data_set(void)
{
  unsigned char crc = 0;
  unsigned char i = 0,j;
  
  P_UART2_DIR = 1;
  uart2_tx_data_buf[i++] = ' ';                               //Preamble
  uart2_tx_data_buf[i++] = '(';                               //STX
  ++i;
  uart2_tx_data_buf[i++] = 0x56;  //0x01;                     //Device bit
  uart2_tx_data_buf[i++] = 0x40;                              //Device ID
  uart2_tx_data_buf[i++] = dimmer_level[0];                   //Data#1
  uart2_tx_data_buf[i++] = dimmer_level[2];                   //Data#2
  uart2_tx_data_buf[i++] = dimmer_level[1];                   //Data#3 
  uart2_tx_data_buf[i++] = dimmer_level[3];                   //Data#$ 
  uart2_tx_data_buf[2] = i+1;                                 //Length
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                               //CRC
  uart2_tx_data_buf[i++] = ')';                               //ETX

  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
  
}

void audio_data_set(void)
{
  unsigned char crc = 0;
  unsigned char i = 0,j;
  unsigned char temp=0;
  
  P_UART2_DIR = 1;
  uart2_tx_data_buf[i++] = ' ';                               //Preamble
  uart2_tx_data_buf[i++] = '(';                               //STX
  ++i;
  uart2_tx_data_buf[i++] = 0x55;  //0x01;                           //Device bit
  uart2_tx_data_buf[i++] = 0x70;                           //Device ID
  if(room_data_buf[40] & 0x02) temp |= 0x01;              //Guest IN
  if(f_chime_keep) temp |= 0x02;                //Chime ON
  if(f_alram_on)   temp |= 0x04;                //Alram ON
  if(room_data_buf[36] & 0x08) temp |= 0x08;              //Audio ON
  if(room_data_buf[40] & 0x04) temp |= 0x10;              //DND ON
  if(room_data_buf[40] & 0x08) temp |= 0x20;              //MUR ON
  if(room_data_buf[36] & 0x40) temp |= 0x40;              //MUTE ON
  
  uart2_tx_data_buf[i++] = temp;
  uart2_tx_data_buf[i++] = room_data_buf[36] & 0x07;      //Radio channel
  
  uart2_tx_data_buf[i++] = audio_volume[0];             //Audio volume
  uart2_tx_data_buf[i++] = room_data_buf[79];           //Chime volume
  uart2_tx_data_buf[i++] = room_data_buf[80];           //Alram volume
  uart2_tx_data_buf[2] = i+1;                               //Length
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                               //CRC
  uart2_tx_data_buf[i++] = ')';                               //ETX

  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
  
}

void chime_ind_data_set(void)
{
  unsigned char crc = 0;
  unsigned char i=0,j;
  
  P_UART2_DIR = 1;
  ++i;                                                        //Length point(아래에서 처리)
  uart2_tx_data_buf[i++] = 0x50;                              //Device ID
  
  uart2_tx_data_buf[i++] = 0;
  uart2_tx_data_buf[i++] = chime_ind_toggle_bit[0];
  uart2_tx_data_buf[i++] = ls_control_data_read(0);
  
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                             //CRC
  uart2_tx_data_buf[0] = i;                                 //Length
  
  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
   timer_cnt = 0;
}

void common_data_set(void)
{
  unsigned char crc = 0;
  unsigned char i = 0,j;
  unsigned char temp=0;
    
  P_UART2_DIR = 1;
  ++i;
  uart2_tx_data_buf[i++] = 0xff;                        //Device ID
  uart2_tx_data_buf[i++] = 0xff;                        //Device ID
  
  if(room_data_buf[40] & 0x01) temp |= 0x01;            //Check IN 
  if(room_data_buf[40] & 0x02) temp |= 0x02;            //Guest IN
  if(room_data_buf[40] & 0x10) temp |= 0x04;            //Window
  if(room_data_buf[20] & 0x04) temp |= 0x08;            //Cool mode
  if(room_data_buf[40] & 0x04) temp |= 0x10;            //DND
  if(room_data_buf[40] & 0x08) temp |= 0x20;            //MUR
  if(room_data_buf[82] & 0x08) temp |= 0x80;            //현재조도상태 1=밝게하라(밝다),0=어둡게하라(어둡다)
  uart2_tx_data_buf[i++] = temp;
  uart2_tx_data_buf[i++] = room_data_buf[82];           //조도 조절법, 복귀시간 및 현재 상태
  
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                             //CRC
  uart2_tx_data_buf[0] = i;                                 //Length
  
  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
   timer_cnt = 0;
}

void ex_relay_data_set(unsigned char index, unsigned char index_bit)
{
  unsigned char crc = 0;
  unsigned char i=0,j;
  
  P_UART2_DIR = 1;
  ++i;                                                        //Length point(아래에서 처리)
  uart2_tx_data_buf[i++] = 0x90;                              //Device ID
  
  if(index_bit & 0x01)
  {
    uart2_tx_data_buf[i] = 0;
    
    if(curtain_sw_buf[0] & 0x01) uart2_tx_data_buf[i] |= 0x80; //curtain 1 open    ( ex relay port 1 )
    else uart2_tx_data_buf[i] &= 0x7f;
      
    if(curtain_sw_buf[0] & 0x02) uart2_tx_data_buf[i] |= 0x40; //curtain 1 stop    ( ex relay port 2 )
    else uart2_tx_data_buf[i] &= 0xbf;
    
    if(curtain_sw_buf[0] & 0x04) 
    {
      uart2_tx_data_buf[i] |= 0x20; //curtain 1 close   ( ex relay port 3 )
    }
    else uart2_tx_data_buf[i] &= 0xdf;     
    
    if(curtain_sw_buf[1] & 0x01) uart2_tx_data_buf[i] |= 0x10; //curtain 2 open    ( ex relay port 4 )
    else uart2_tx_data_buf[i] &= 0xef;    
    
    if(curtain_sw_buf[1] & 0x02) uart2_tx_data_buf[i] |= 0x08; //curtain 2 stop    ( ex relay port 5 )
    else uart2_tx_data_buf[i] &= 0xf7;    
    
    if(curtain_sw_buf[1] & 0x04) 
    {
      uart2_tx_data_buf[i] |= 0x04; //curtain 2 close   ( ex relay port 6 )
    }
    else uart2_tx_data_buf[i] &= 0xfb;      
    
    uart2_tx_data_buf[i] &= 0xfc;
    
    ++i;
  }
  
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                             //CRC
  uart2_tx_data_buf[0] = i;                                 //Length
  
  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
  timer_cnt = 0;
    
}

/* UART2 data send check routine */
void UART2_tx_check(void)
{
  if(f_uart2_data_send){
    if(++timer_cnt > 45)
    {
      timer_cnt = 1001;
      if(USART_GetFlagStatus(USART2, USART_FLAG_TC) !=RESET) {
      if(uart2_tx_length == 0){
        USART2->DR = ((uart2_tx_data_buf[uart2_tx_length++] & (u16)0x01FF) | (u16)0x0100);
      }
      else{
        if(uart2_tx_length < uart2_tx_backup){
          USART2->DR = (uart2_tx_data_buf[uart2_tx_length++] & (u16)0x00FF);
        }
        else{
          P_UART2_DIR = 0;
          f_uart2_data_send = 0;
          sub_call_timer = 18;
        }
      }
      }
    }
  }
}

void giga_gini(void)
{
  unsigned char crc = 0;
  unsigned char i=0,j;
  
  P_UART2_DIR = 1;
  ++i;
  uart2_tx_data_buf[i++] = 0x70;
  
  uart2_tx_data_buf[i++] = room_status();
  
  uart2_tx_data_buf[i++] = (room_data_buf[0] & 0x7f); //TS1 SET TEMP (정수부)
  if(room_data_buf[0] & 0x80) uart2_tx_data_buf[i++] = 0x05; //TS1 SET TEMP (소수부)
  else uart2_tx_data_buf[i++] = 0; 
  
  uart2_tx_data_buf[i++] = (room_data_buf[4] & 0x7f); //TS1 CHECK TEMP (정수부)
  if(room_data_buf[4] & 0x80) uart2_tx_data_buf[i++] = 0x05; //TS1 CHECK TEMP (소수부)
  else uart2_tx_data_buf[i++] = 0;
  
  uart2_tx_data_buf[i++] = ts_status(0); //TS STATUS  
  
  uart2_tx_data_buf[i++] = light_status(); //LIGHT STATUS
  
  uart2_tx_data_buf[i++] = curtain_status(); // Curtain
  
  uart2_tx_data_buf[i++] = 0x07; //한국어 사용    
  
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                             //CRC
  uart2_tx_data_buf[0] = i;                                 //Length
  
  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;
  timer_cnt = 0;
}

unsigned char room_status(void)
{
  unsigned char temp = 0;
  
  switch(room_data_buf[49] & 0x03) //key-Tag
  {
    case 1: temp |= 0x02; break; //Maid key
    case 3: temp |= 0x01; break; //Guest Key
  }
  
  if(room_data_buf[40] & 0x01) temp |= 0x04; //Check IN
  if(room_data_buf[40] & 0x04) temp |= 0x08; //DND
  if(room_data_buf[40] & 0x08) temp |= 0x10; //MUR
  if(room_data_buf[42] & 0x01) temp |= 0x20; //EMR
  
  return temp;
}

unsigned char ts_status(unsigned char index)
{
  unsigned char ts_status_value = 0;
  
  if(room_data_buf[20+index] & 0x04) ts_status_value |= 0x02; //Cooling mode
  else ts_status_value |= 0x01;                               //Heating mode
  
  if(room_data_buf[16+index] & 0x04) ts_status_value |= 0x04; //TS power on,off
  if((room_data_buf[20+index] & 0x08) == 0) ts_status_value |= 0x08; //Auto,Manual Mode
  
  switch(room_data_buf[16] & 0x03)
  {
    case 1: ts_status_value |= 0x10; break; //L
    case 2: ts_status_value |= 0x20; break; //M
    case 3: ts_status_value |= 0x30; break; //H
  }
  
  if(room_data_buf[16+index] & 0x10) ts_status_value |= 0x40; //TS VALVE
  ts_status_value &= 0x7f; //섭씨
  
  return ts_status_value;
}

unsigned char light_status(void)
{
    unsigned char light_status = 0;
    
    if(room_data_buf[45] & 0x02) light_status |= 0x01;
    if(room_data_buf[45] & 0x04) light_status |= 0x02;
    
    return light_status;
}

unsigned char curtain_status(void)
{
    unsigned char curtain_status = 0;
    
    if(curtain_state[0] & 0x01) curtain_status |= 0x01; //curtain 1 open
    if(curtain_state[0] & 0x02) curtain_status |= 0x02; //curtain 1 close
    if(curtain_state[0] & 0x04) curtain_status |= 0x03; //curtain 1 stop
    
    return curtain_status;
}

//=======================================================================
void UART2_control_proc(void) 
{
//u8 ix;
  unsigned char sub_device_kind;

  if(f_uart2_send_time){
    f_uart2_send_time = 0;
    
    if(f_volume_control)  volume_jig_data_set();
    else{
      if(f_common_send_time){ 
        common_data_set();  
        f_common_send_time = 0; 
      }
      else{
        if(b_sub_device_table[cb_mode][id_number] == 0xff) id_number = 0;
        sub_device_kind = b_sub_device_table[cb_mode][id_number++];
        switch(sub_device_kind){
          case B_KEY_SENSOR_0:
            key_sendor_data_set(0,0x01);
            if(++ks_device_falling_count[0]>15) ks_device_falling_count[0] = 20;                // 기구물 통신장애 추가 (20.11.12)
            break;  
          case B_TEMP_SENSOR_1:
            Thermo_sensor_data_set(0,0x01);
            break;  
          case B_TEMP_SENSOR_2:
            Thermo_sensor_data_set(1,0x02);
            break;  
          case B_TEMP_SENSOR_3:
            Thermo_sensor_data_set(2,0x04);
            break;  
          case B_TEMP_SENSOR_4:
            Thermo_sensor_data_set(3,0x08);
            break;  
          case B_NIGHT_TABLE_1:
            night_table_data_set(0,0x01);
            if(++nt_device_falling_count[0]>15) ks_device_falling_count[0] = 20;
            break;  
          case B_NIGHT_TABLE_2:
            night_table_data_set(1,0x02);
            if(++nt_device_falling_count[1]>15) ks_device_falling_count[1] = 20;
            break;  
          case B_LIGHT_SWITCH_1:
            light_switch_data_set(0,0x01);
            if(++light_switch_falling_count[0]>15) light_switch_falling_count[0] = 20;
            break;  
          case B_LIGHT_SWITCH_2:
            light_switch_data_set(1,0x02);
            if(++light_switch_falling_count[1]>15) light_switch_falling_count[1] = 20;
            break;  
          case B_LIGHT_SWITCH_3:
            light_switch_data_set(2,0x04);
            if(++light_switch_falling_count[2]>15) light_switch_falling_count[2] = 20;
            break;  
          case B_LIGHT_SWITCH_4:
            light_switch_data_set(3,0x08);
            if(++light_switch_falling_count[3]>15) light_switch_falling_count[3] = 20;
            break;  
          case B_LIGHT_SWITCH_5:
            light_switch_data_set(4,0x10);
            if(++light_switch_falling_count[4]>15) light_switch_falling_count[4] = 20;
            break; 
          case B_LIGHT_SWITCH_6:
            light_switch_data_set(5,0x20);
            if(++light_switch_falling_count[5]>15) light_switch_falling_count[5] = 20;
            break;
          case B_LIGHT_SWITCH_7:
            light_switch_data_set(6,0x40);
            if(++light_switch_falling_count[6]>15) light_switch_falling_count[6] = 20;
            break;
          case B_LIGHT_SWITCH_8:
            light_switch_data_set(7,0x80);
            if(++light_switch_falling_count[7]>15) light_switch_falling_count[7] = 20;
            break;
          case B_LIGHT_SWITCH_9:
            light_switch_data_set(8,0x100);
            if(++light_switch_falling_count[8]>15) light_switch_falling_count[8] = 20;
            break;
          case B_LIGHT_SWITCH_10:
            light_switch_data_set(9,0x101);
            if(++light_switch_falling_count[9]>15) light_switch_falling_count[9] = 20;
            break;
          case B_LIGHT_SWITCH_11:
            light_switch_data_set(10,0x102);
            if(++light_switch_falling_count[10]>15) light_switch_falling_count[10] = 20;
            break;
          case B_LIGHT_SWITCH_12:
            light_switch_data_set(11,0x104);
            if(++light_switch_falling_count[11]>15) light_switch_falling_count[11] = 20;
            break;
          case B_LIGHT_SWITCH_13:
            light_switch_data_set(12,0x108);
            if(++light_switch_falling_count[12]>15) light_switch_falling_count[12] = 20;
            break;
          case B_LIGHT_SWITCH_14:
            light_switch_data_set(13,0x110);
            if(++light_switch_falling_count[13]>15) light_switch_falling_count[13] = 20;
            break;
          case B_LIGHT_SWITCH_15:
            light_switch_data_set(14,0x120);
            if(++light_switch_falling_count[14]>15) light_switch_falling_count[14] = 20;
            break;
          case B_LIGHT_SWITCH_16:
            light_switch_data_set(15,0x140);
            if(++light_switch_falling_count[15]>15) light_switch_falling_count[15] = 20;
            break;        
            
          case B_DIMMER:
            dimmer_data_set();
            break;
            
          case B_AUDIO:
            audio_data_set();
            break;
            
          case B_CHIME_IND:
            chime_ind_data_set();
            if(++ci_device_falling_count[0]>15) ci_device_falling_count[0] = 20;
            break;
            
          case B_EXRELAY:
              ex_relay_data_set(0,0x01);
            break;            
            
          case B_GIGA:
             giga_gini();
            break;
        }
      }
    }
  }
	uart2_frame_proc();
  


}

void UART2_rx_check(void)
{
  unsigned char rx_crc=0;
  unsigned char i;
  
  if(f_uart2_frame_rx_ok){
    f_uart2_frame_rx_ok = 0;
      
    for(i=1; i<uart2_rxd_buf[0]-1; ++i) rx_crc += uart2_rxd_buf[i];
    rx_crc ^= 0x55;
    if(rx_crc == (uart2_rxd_buf[uart2_rxd_buf[0]-1])){
      switch(uart2_rxd_buf[1] & 0xf0){
        case 0x10:    //Thermo sensor
          led3 ^= 1;
          thermo_sensor_check(uart2_rxd_buf[1] & 0x03);
          break;
        case 0x20:    //Night Table
          night_table_check(uart2_rxd_buf[1] & 0x03);
          break;
        case 0x30:    //Light switch
          light_switch_check(uart2_rxd_buf[1] & 0x07);
          break;
        case 0x40:    //Dimmer module
          dimmer_check(uart2_rxd_buf[1] & 0x03);
          break;
        case 0x50:    //Service Plate
            chime_ind_check(uart2_rxd_buf[1] & 0x03);    
          break;
        case 0x60:    //Key sensor
          key_sensor_rx_check(uart2_rxd_buf[1] & 0x03);
          break;
        case 0x70: //receive 기가지니
          giga_gini_receive();
          break;
        case 0xc0:
          if(f_volume_control)
          {
            volume_data_check();
          }
          break;
      }
      sub_call_timer = 18;
   }

  }
}

//================ i stay data check routine ==================
void giga_gini_receive(void)
{
  switch(uart2_rxd_buf[2])
  {
    case 0x0A: //알림버튼 제어
      
      if(uart2_rxd_buf[3] & 0x01) dnd_mur_execution('D','T'); //DND ON,OFF
      if(uart2_rxd_buf[3] & 0x02) dnd_mur_execution('N','T'); //MUR ON,OFF
      //EMR ??
      //WAIT ??
      
    break;
    
    case 0x14: //전체 전등 제어
      
      if(room_data_buf[40] & 0x02) //재실인가?
      {
        if(uart2_rxd_buf[3] & 0x01) 
        {
          room_data_buf[45] |= 0x1e; //객실 전등 ALL ON
          room_data_buf[46] |= 0x03; //화장실 전등 ALL ON
        }
        else if(uart2_rxd_buf[3] & 0x02) 
        {
          room_data_buf[45] &= 0xe1; //객실 전등 ALL OFF
          room_data_buf[46] &= 0xfc; //화장실 전등 ALL OFF
        }
      }
        
    break;
    
    case 0x15: //개별 전등 제어(그룹)
      
      if(room_data_buf[40] & 0x02) //재실인가?
      {      
        if(backup_data[1] != uart2_rxd_buf[3] & 0x3f) //data 변경시 적용
        {
          backup_data[1] = (uart2_rxd_buf[3] & 0x3f);
          
          if(uart2_rxd_buf[3] & 0x01) room_data_buf[45] |= 0x02; //전등 1
          else room_data_buf[45] &= 0xfd;
          
          if(uart2_rxd_buf[3] & 0x02) room_data_buf[45] |= 0x04; //전등 2
          else room_data_buf[45] &= 0xfb;       
          
          if(uart2_rxd_buf[3] & 0x04) room_data_buf[45] |= 0x08; //전등 3
          else room_data_buf[45] &= 0xf7;        
          
          if(uart2_rxd_buf[3] & 0x08) room_data_buf[45] |= 0x10; //전등 4
          else room_data_buf[45] &= 0xef;            
          
          if(uart2_rxd_buf[3] & 0x10) room_data_buf[46] |= 0x01; //화장실 전등 1
          else room_data_buf[46] &= 0xfe;   
          
          if(uart2_rxd_buf[3] & 0x20) room_data_buf[46] |= 0x02; //화장실 전등 2
          else room_data_buf[46] &= 0xfd;             
        }
      }
      
    break;
    
    case 0x17: //개별 전등 제어(개별)
      
      if(room_data_buf[40] & 0x02) //재실인가?
      {  
          switch(uart2_rxd_buf[4] & 0x07) //전등 번호 1~6
          {
          case 0: //1번 전등 ( 객실 L 1)
              
              if(uart2_rxd_buf[3] == 1) room_data_buf[45] |= 0x02;
              else room_data_buf[45] &= 0xfd;
            
            break;
            
          case 1: //2번 전등 ( 객실 L 2 )
              
              if(uart2_rxd_buf[3] == 1) room_data_buf[45] |= 0x04;
              else room_data_buf[45] &= 0xfb;
            
            break;
            
          case 2: //3번 전등 ( 객실 L 3 )
              
              if(uart2_rxd_buf[3] == 1) room_data_buf[45] |= 0x08;
              else room_data_buf[45] &= 0xf7;              
              
            break;
            
          case 3: //4번 전등 ( 객실 L 4 )
              
              if(uart2_rxd_buf[3] == 1) room_data_buf[45] |= 0x10;
              else room_data_buf[45] &= 0xef;
              
            break;
              
          case 4: //5번 전등 ( 화장실 L 1 )
            
              if(uart2_rxd_buf[3] == 1) room_data_buf[46] |= 0x01;
              else room_data_buf[46] &= 0xfe;              
            
            break;
            
          case 5: //6번 전등 ( 화장실 L 2 )
            
              if(uart2_rxd_buf[3] == 1) room_data_buf[46] |= 0x02;
              else room_data_buf[46] &= 0xfd;                 
            
            break;
          }
      }
      
    break;
    
    case 0x21: //전체 온도조절기 Valve 제어
          
          if((uart2_rxd_buf[3] & 0xfe) == 0) //TS POWER 제어 이외 동작 안될 시 작동
          {
            if((uart2_rxd_buf[3] & 0x01) && !(uart2_rxd_buf[4] & 0x04)) 
            {
              room_data_buf[16] |= 0x04; //TS POWER ON
              room_data_buf[16] |= 0x80;
              room_data_buf[99] |= 0x02;
              if(!(uart2_rxd_buf[3] & 0x38)) //fan speed control 제어 안할시
              {
                nt_switch_bit_check(K_fcu_auto_manual,0);
                room_data_buf[20] |= 0x08;    //Auto mode flag set
              }
            }
            else 
            {
              room_data_buf[16] &= 0xfb; //TS POWER OFF
              room_data_buf[16] |= 0x80;
              room_data_buf[99] |= 0x02;
            }
          }
          
          if(uart2_rxd_buf[3] & 0x02) //TS AUTO MODE
          {
            nt_switch_bit_check(K_fcu_auto_manual,0);
            room_data_buf[20] |= 0x08;    //Auto mode flag set
          }
          
          if(uart2_rxd_buf[3] & 0x04) room_data_buf[16] &= 0xfb; //TS SPEED STOP = TS POWER OFF
          
          if((room_data_buf[16] & 0x03) != 0) fan_speed_flag = 1;   
          else fan_speed_flag = 0;
          
          if((room_data_buf[16] & 0x04) && (fan_speed_flag != 0)) //온도조절기 ON 상태 일시
          {   
            if((uart2_rxd_buf[3] & 0x38) != 0) //fan (low , mid , high)
            {
                if(!(uart2_rxd_buf[3] & 0x20)) //fan high 제어시 ..
                  room_data_buf[54] = ((uart2_rxd_buf[3] & 0x18) >> 3);
                else
                  room_data_buf[54] = ((uart2_rxd_buf[3] & 0x30) >> 3) - 1;
                  
                room_data_buf[16] |= 0x80;
                room_data_buf[99] |= 0x02;    //Speed 변경시 온도 상태 전송
                room_data_buf[20] &= 0xf7;    //Manual mode flag set                      
            }
          }
      
    break;
    
    case 0x22: //전체 온도조절기 설정온도 제어 
          
          ts_air_set_temp_from_PC[0] = uart2_rxd_buf[3]; 
          if(uart2_rxd_buf[4] != 0) ts_air_set_temp_from_PC[0] |= 0x80;
          ts_set_temp_send_count[0] = 0;
          
    break;
    
    case 0x23: //개별 온도조절기 Valve 제어 
          
          ts_index = uart2_rxd_buf[4]; // 온도조절기 번호
          
          if((uart2_rxd_buf[3] & 0xfe) == 0) //TS POWER 제이 이외 동작 안될 시 작동
          {
            if((uart2_rxd_buf[3] & 0x01) && !(uart2_rxd_buf[3] & 0x04)) 
            {
              room_data_buf[16+ts_index] |= 0x04; //TS POWER ON
              room_data_buf[16+ts_index] |= 0x80;
              room_data_buf[99] |= 0x02;
              if(!(uart2_rxd_buf[3] & 0x38)) //fan speed control 제어 안할시
              {
                nt_switch_bit_check(K_fcu_auto_manual,ts_index);
                room_data_buf[20+ts_index] |= 0x08;    //Auto mode flag set
              }
            }
            else 
            {
              room_data_buf[16+ts_index] &= 0xfb; //TS POWER OFF
              room_data_buf[16+ts_index] |= 0x80;
              room_data_buf[99] |= 0x02;
            }
          }
          
          if(uart2_rxd_buf[3] & 0x02) //TS AUTO MODE
          {
            nt_switch_bit_check(K_fcu_auto_manual,ts_index);
            room_data_buf[20+ts_index] |= 0x08;    //Auto mode flag set
          }
          
          if(uart2_rxd_buf[3] & 0x04) room_data_buf[16+ts_index] &= 0xfb; //TS SPEED STOP = TS POWER OFF
          
          if((room_data_buf[16] & 0x03) != 0) fan_speed_flag = 1;   
          else fan_speed_flag = 0;          
          
          if((room_data_buf[16+ts_index] & 0x04) && (fan_speed_flag != 0)) //온도조절기 ON 상태 일시
          {   
            if((uart2_rxd_buf[3] & 0x38) != 0)
            {
                if(!(uart2_rxd_buf[3] & 0x20))
                  room_data_buf[54+ts_index] = ((uart2_rxd_buf[3] & 0x18) >> 3);
                else
                  room_data_buf[54+ts_index] = ((uart2_rxd_buf[3] & 0x30) >> 3) - 1;
                
                room_data_buf[16+ts_index] |= 0x80;
                room_data_buf[99] |= 0x02; //Speed 변경시 온도 상태 전송
                room_data_buf[20+ts_index] &= 0xf7;    //Manual mode flag set                      
            }
          }         
          
    break;
    
    case 0x24: //개별 온도조절기 설정온도 변경
          
          ts_set_index = uart2_rxd_buf[6];
          
          ts_air_set_temp_from_PC[0+ts_set_index] = uart2_rxd_buf[3]; 
          if(uart2_rxd_buf[4] != 0) ts_air_set_temp_from_PC[0+ts_set_index] |= 0x80;
          ts_set_temp_send_count[0+ts_set_index] = 0;
          
    break;
    
    case 0x32: //전체 커튼 제어
          
          curtain_control = 1; //기가 지니 커튼 제어 flag ( Light sw Led 동작 안시키게 하기 위해서 flag set )
          
          switch(uart2_rxd_buf[3])
          {
          case 1: //전체 커튼 열기
              
              curtain_sw_buf[0] = 0; //커튼 1 신호 clear
              curtain_sw_buf[1] = 0; //커튼 2 신호 clear
              
              curtain_sw_buf[0] |= 0x01; //커튼 1 열림
              curtain_sw_buf[1] |= 0x01; //커튼 2 열림
              
            break;
            
          case 2: //전체 커튼 닫기

              curtain_sw_buf[0] = 0; //커튼 1 신호 clear
              curtain_sw_buf[1] = 0; //커튼 2 신호 clear
              
              curtain_sw_buf[0] |= 0x04; //커튼 1 닫힘
              curtain_sw_buf[1] |= 0x04; //커튼 2 닫힘             
              
            break;
            
          case 4: //전체 커튼 멈춤
              
              curtain_sw_buf[0] = 0; //커튼 1 신호 clear
              curtain_sw_buf[1] = 0; //커튼 2 신호 clear
              
              curtain_sw_buf[0] |= 0x02; //커튼 1 멈춤
              curtain_sw_buf[1] |= 0x02; //커튼 2 멈춤                  
            
            break;
          }
    
    break;
    
  case 0x33: //커튼 개별 제어
    
          curtain_control = 1; //기가 지니 커튼 제어 flag ( Light sw Led 동작 안시키게 하기 위해서 flag set ) @@커튼 동시제어 올수있음 추가필요 //
          
        switch(uart2_rxd_buf[3])
          {
          case 1: //커튼 1 open
              
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x01;
              
            break;
            
          case 2: //커튼 1 close
              
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x04;
              
            break;
              
          case 3: //커튼 1 stop
               
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x02;
              
            break;
            
          case 4: //커튼 2 open
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x01;
              
            break;
            
          case 8: //커튼 2 close
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x04;
              
            break;
            
          case 12: //커튼 2 stop
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x02;              
              
            break;
            
          case 5: //커튼 1 open + 커튼 2 open
              
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x01;
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x01;              
            
            break;
            
          case 9: //커튼 1 open + 커튼 2 close
            
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x01;

              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x04;              
            
            break;
            
          case 13: //커튼 1 open + 커튼 2 stop

              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x01;
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x02;               
              
            break;
            
          case 6: //커튼 1 close + 커튼 2 open
              
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x04;
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x01;              
              
            break;
            
          case 10: //커튼 1 close + 커튼 2 close

              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x04;
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x04;              
            
            break;
            
          case 14: //커튼 1 close + 커튼 2 stop
            
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x04;
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x02;                  
            
            break;
            
          case 7: //커튼 1 stop + 커튼 2 open
            
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x02;

              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x01;              
            
            break;
            
          case 11: //커튼 1 stop + 커튼 2 close
            
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x02;
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x04;               
            
            break;
            
          case 15: //커튼 1 stop + 커튼 2 stop
            
              curtain_sw_buf[0] = 0;
              curtain_sw_buf[0] |= 0x02;
              
              curtain_sw_buf[1] = 0;
              curtain_sw_buf[1] |= 0x02;              
            
            break;
          }
    
    break;
  }    
}
//============= Volume set jig data check routine ===============
//
void volume_data_check(void)
{
  if(uart2_rxd_buf[2] == 0xca){
    if(uart2_rxd_buf[5] & 0x01){    //alram volume
      f_alram_on = 1;
      if(room_data_buf[80] != uart2_rxd_buf[4]){
        digital_volume_execution(1,room_data_buf[80]);
        room_data_buf[80] = uart2_rxd_buf[4];
      }
    }
    else{                           //chime volume
      f_alram_on = 0;
      if((jig_switch_toggle_bit & 0x01) != (uart2_rxd_buf[6] & 0x01)){
        room_data_buf[79] = uart2_rxd_buf[3];
        digital_volume_execution(0,room_data_buf[79]);
      }
    }
  }
}

//
//============= Thermo sensor receive data check routine ==============
//
void thermo_sensor_check(unsigned char index)
{
  if((uart2_rxd_buf[8] & 0x80) == 0){
    f_ts_power_on[index] = 0;
    if((uart2_rxd_buf[1] & 0x7f) <= 70){    //Air set temp
      if(room_data_buf[0+index] != uart2_rxd_buf[2]){
        room_data_buf[16+index] |= 0x80;
        room_data_buf[0+index] = uart2_rxd_buf[2];
        room_data_buf[99] |= 0x02;
      }
    }
    if((uart2_rxd_buf[3] & 0x7f) <= 70){    //Air Check temp
      room_data_buf[4+index] = uart2_rxd_buf[3];
      f_check_temp_change = 1;
//      room_data_buf[99] |= 0x02;            //현재온도 변경시 온도 Data 전송
    }
    if((room_data_buf[4+index] & 0x7f) > room_data_buf[76]){   //이상온도 비교
      if(++em_temp_count[index] > 3){                 //이상온도 3회 이상 발생?
        em_temp_count[index] = 0;
        room_data_buf[42] |= (0x10 << index);         //이상 온도 발생 flag set
        //room_data_buf[45] |= 0x80;    //Fire sign ON
      }
    }
    else{
      //room_data_buf[42] &= (0xef << index);           //이상 온도 발생 flag clear
      switch(index)
      {
      case 0:
          room_data_buf[42] &= 0xef; //온도센서 1 em clear
        break;
      case 1:
          room_data_buf[42] &= 0xdf; //온도센서 2 em clear
        break;
      case 2:
          room_data_buf[42] &= 0xbf; //온도센서 3 em clear
        break;
      case 3:
          room_data_buf[42] &= 0x7f; //온도센서 4 em clear
        break;
      }
      em_temp_count[index] = 0;
      //room_data_buf[45] &= 0x7f;    //Fire sign OFF
    }
    
    if((uart2_rxd_buf[4] & 0x7f) <= 70){    //Floor set temp
      if(room_data_buf[8+index] != uart2_rxd_buf[4]){
        room_data_buf[20+index] |= 0x80;
        room_data_buf[8+index] = uart2_rxd_buf[4];
        room_data_buf[99] |= 0x04;
      }
    }
    if((uart2_rxd_buf[5] & 0x7f) <= 70){    //Floor Check temp
      room_data_buf[12+index] = uart2_rxd_buf[5];
      f_check_temp_change_2 = 1;
    }
    if((uart2_rxd_buf[6] & 0x7f) <= 99){    //Humidity
      room_data_buf[31+index] = uart2_rxd_buf[6];
    }
    
    if(ts_first_power_on[index]){ 
      if(room_data_buf[40] & 0x02)
      {
      //First power on than only bit save
        if(ts_switch_toggle_bit[index] != uart2_rxd_buf[7]){    //TS switch push ?
          if((ts_switch_toggle_bit[index] & 0x01) != (uart2_rxd_buf[7] & 0x01)){    //TS on/off switch change(key push)
            room_data_buf[16+index] ^= 0x04;    //TS Air on/off toggle
            nt_switch_bit_check(K_fcu_auto_manual,index);
            room_data_buf[99] |= 0x02;
            room_data_buf[20+index] |= 0x08;    //Auto mode flag set
            room_data_buf[16+index] |= 0x80;
          }
          if((ts_switch_toggle_bit[index] & 0x02) != (uart2_rxd_buf[7] & 0x02)){    //TS floor mode on/off switch change(key push)              /////
            room_data_buf[16+index] ^= 0x20;    //TS Floor on/off toggle
            room_data_buf[99] |= 0x04;
            room_data_buf[20+index] |= 0x80;
          }
       //   if((ts_switch_toggle_bit[index] & 0x10) != (uart2_rxd_buf[7] & 0x10)) room_data_buf[24+index] ^= 0x08;    //TS FaC, DoC
          if((ts_switch_toggle_bit[index] & 0x20) != (uart2_rxd_buf[7] & 0x20)) ts_speed_execution(index);          //TS local speed execution
          
          if((ts_switch_toggle_bit[index] & 0x40) != (uart2_rxd_buf[7] & 0x40)){  //Temp control mode(air,floor mode)
          /*  if(room_data_buf[74] == 2){   //환절기 모드일 경우만 적용한다.
              if(room_data_buf[83] >= 2) room_data_buf[83] = 0;   //0=auto, 1=warm, 2=cool
              else ++room_data_buf[83];
              room_data_buf[54+index] = 0;   //Auto mode
              room_data_buf[20+index] |= 0x80;
              switch(room_data_buf[83]){
                case 0: break;
                case 1:
                  room_data_buf[20] |= 0x04;    //냉방
                  room_data_buf[21] |= 0x04;    //냉방
                  room_data_buf[22] |= 0x04;    //냉방
                  room_data_buf[23] |= 0x04;    //냉방
                  if((room_data_buf[0] & 0x7f) >= (room_data_buf[4] & 0x7f)){
                    ts_air_set_temp_from_PC[0] = (room_data_buf[4] & 0x7f) - 2;
                    if(ts_air_set_temp_from_PC[0] < 18) ts_air_set_temp_from_PC[0] = 18;
                    ts_set_temp_send_count[0] = 0;
                  }
                  break;
                case 2:
                  room_data_buf[20] &= 0xfb;    //난방
                  room_data_buf[21] &= 0xfb;    //난방
                  room_data_buf[22] &= 0xfb;    //난방
                  room_data_buf[23] &= 0xfb;    //난방
                  
                  if((room_data_buf[0] & 0x7f) <= (room_data_buf[4] & 0x7f)){
                    ts_air_set_temp_from_PC[0] = (room_data_buf[4] & 0x7f) + 2;
                    if(ts_air_set_temp_from_PC[0] > 30) ts_air_set_temp_from_PC[0] = 30;
                    ts_set_temp_send_count[0] = 0;
                  }
                  break;
              }
              
              
            }*/
          }
          if(ts_switch_toggle_bit[index] & 0x80){               //Set temp CB execution?
            //if((ts_switch_toggle_bit[index] & 0x04) != (uart2_rxd_buf[8] & 0x04)) ts_temp_up_excution(index,uart2_rxd_buf[9]);
            //if((ts_switch_toggle_bit[index] & 0x08) != (uart2_rxd_buf[8] & 0x08)) ts_temp_down_excution(index,uart2_rxd_buf[9]);
          }
        }   
      }
    }
    ts_switch_toggle_bit[index] = uart2_rxd_buf[7];
    ts_first_power_on[index] = 1;
  }
  else f_ts_power_on[index] = 1;
  
}



void ts_speed_execution(unsigned char index)   
{
  if(room_data_buf[16+index] & 0x04){
    if(room_data_buf[54+index] != 0){
      if(++room_data_buf[54+index] >= 4) room_data_buf[54+index] = 1;
    }
    else{
      room_data_buf[54+index] = room_data_buf[16+index] & 0x03;
      if(++room_data_buf[54+index] >= 4) room_data_buf[54+index] = 1;
    }
    room_data_buf[16+index] |= 0x80;
    room_data_buf[20+index] &= 0xf7;    //Manual mode flag set
    room_data_buf[99] |= 0x02;          //Speed 변경시 온도 상태 전송
  }
  else room_data_buf[54+index] = 0;   //Auto mode
}

void ts_temp_up_excution(unsigned char index, unsigned char ts_mode)
{
  if(ts_mode & 0x01){   //Floor mode
    if(room_data_buf[8+index] & 0x80){
      room_data_buf[8+index] &= 0x7f;
      if((((room_data_buf[29] & 0xf0)>>4)+25) > (room_data_buf[8+index] & 0x80)) ++room_data_buf[8+index];
    }
    else{
      if((((room_data_buf[29] & 0xf0)>>4)+25) > (room_data_buf[8+index] & 0x80)) room_data_buf[8+index] |= 0x80;
    }
  }
  else{                 //Air mode
    if(room_data_buf[0+index] & 0x80){
      room_data_buf[0+index] &= 0x7f;
      if((((room_data_buf[28] & 0xf0)>>4)+25) > (room_data_buf[0+index] & 0x80)) ++room_data_buf[0+index];
    }
    else{
      if((((room_data_buf[28] & 0xf0)>>4)+25) > (room_data_buf[0+index] & 0x80)) room_data_buf[0+index] |= 0x80;
    }
  }
}

void ts_temp_down_excution(unsigned char index, unsigned char ts_mode)
{
  if(ts_mode & 0x01){   //Floor mode
    if(room_data_buf[8+index] & 0x80){
      room_data_buf[8+index] &= 0x7f;
    }
    else{
      if(((room_data_buf[29] & 0x0f)+25) < (room_data_buf[8+index] & 0x80)){
        --room_data_buf[8+index];
        room_data_buf[8+index] |= 0x80;
      }
    }
  }
  else{                 //Air mode
    if(room_data_buf[0+index] & 0x80){
      room_data_buf[0+index] &= 0x7f;
    }
    else{
      if(((room_data_buf[28] & 0x0f)+25) < (room_data_buf[0+index] & 0x80)){
        --room_data_buf[0+index];
        room_data_buf[0+index] |= 0x80;
      }
    }
  }
}

//
//================ Night Table receive data check routine ===============
//

void night_table_check(unsigned char index)
{
  if(f_first_nt_power_on[index]){
    if(room_data_buf[40] & 0x02){   //Guest IN?
      if((nt_switch_toggle_bit[index][0] & 0x01) != (uart2_rxd_buf[3] & 0x01)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][0],index);
      if((nt_switch_toggle_bit[index][0] & 0x02) != (uart2_rxd_buf[3] & 0x02)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][1],index);
      if((nt_switch_toggle_bit[index][0] & 0x04) != (uart2_rxd_buf[3] & 0x04)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][2],index);
      if((nt_switch_toggle_bit[index][0] & 0x08) != (uart2_rxd_buf[3] & 0x08)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][3],index);
      if((nt_switch_toggle_bit[index][0] & 0x10) != (uart2_rxd_buf[3] & 0x10)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][4],index);
      if((nt_switch_toggle_bit[index][0] & 0x20) != (uart2_rxd_buf[3] & 0x20)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][5],index);
      if((nt_switch_toggle_bit[index][0] & 0x40) != (uart2_rxd_buf[3] & 0x40)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][6],index);
      if((nt_switch_toggle_bit[index][0] & 0x80) != (uart2_rxd_buf[3] & 0x80)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][7],index);
      
      if((nt_switch_toggle_bit[index][1] & 0x01) != (uart2_rxd_buf[4] & 0x01)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][8],index);
      if((nt_switch_toggle_bit[index][1] & 0x02) != (uart2_rxd_buf[4] & 0x02)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][9],index);
      if((nt_switch_toggle_bit[index][1] & 0x04) != (uart2_rxd_buf[4] & 0x04)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][10],index);
      if((nt_switch_toggle_bit[index][1] & 0x08) != (uart2_rxd_buf[4] & 0x08)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][11],index);
      if((nt_switch_toggle_bit[index][1] & 0x10) != (uart2_rxd_buf[4] & 0x10)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][12],index);
      if((nt_switch_toggle_bit[index][1] & 0x20) != (uart2_rxd_buf[4] & 0x20)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][13],index);
      if((nt_switch_toggle_bit[index][1] & 0x40) != (uart2_rxd_buf[4] & 0x40)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][14],index);
      if((nt_switch_toggle_bit[index][1] & 0x80) != (uart2_rxd_buf[4] & 0x80)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][15],index);
       
      if((nt_switch_toggle_bit[index][2] & 0x01) != (uart2_rxd_buf[5] & 0x01)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][16],index);
      if((nt_switch_toggle_bit[index][2] & 0x02) != (uart2_rxd_buf[5] & 0x02)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][17],index);
      if((nt_switch_toggle_bit[index][2] & 0x04) != (uart2_rxd_buf[5] & 0x04)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][18],index);
      if((nt_switch_toggle_bit[index][2] & 0x08) != (uart2_rxd_buf[5] & 0x08)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][19],index);
      if((nt_switch_toggle_bit[index][2] & 0x10) != (uart2_rxd_buf[5] & 0x10)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][20],index);
      if((nt_switch_toggle_bit[index][2] & 0x20) != (uart2_rxd_buf[5] & 0x20)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][21],index);
      if((nt_switch_toggle_bit[index][2] & 0x40) != (uart2_rxd_buf[5] & 0x40)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][22],index);
      if((nt_switch_toggle_bit[index][2] & 0x80) != (uart2_rxd_buf[5] & 0x80)) nt_switch_bit_check(b_switch_kind_table[cb_mode][index][23],index);
    }
  }
  else f_first_nt_power_on[index] = 1;
  
  nt_switch_toggle_bit[index][0] = uart2_rxd_buf[3];
  nt_switch_toggle_bit[index][1] = uart2_rxd_buf[4];
  nt_switch_toggle_bit[index][2] = uart2_rxd_buf[5];
  
 /* nt_status[index] = uart2_rxd_buf[6];
  if(nt_status[index] & 0x10){
    if(!f_alram_on){
      f_alram_on = 1;
      digital_volume_execution(0,room_data_buf[80]);
    }
  }
  else f_alram_on = 0;
  
  if(index == 0){
    if(nt_status[0] & 0x80) room_data_buf[82] |= 0x08;
    else room_data_buf[82] &= 0xf7;
  }*/
  
//--- 부전동 호텔 나이트 테이블 온도센서 역할 처리 ---  
  
    if((uart2_rxd_buf[7] & 0x7f) <= 70){    //Air set temp
      if(room_data_buf[0] != uart2_rxd_buf[7]){
            room_data_buf[16] |= 0x80;
            room_data_buf[0] = uart2_rxd_buf[7];
            room_data_buf[99] |= 0x02;
            
            if(cb_mode == 1) // D, 장애인실 Type
            {
                  ts_air_set_temp_from_PC[0] = uart2_rxd_buf[7];
                  ts_set_temp_send_count[0] = 3;
            }
      }
    }
    
    if((uart2_rxd_buf[8] & 0x7f) <= 70){    //Air Check temp
      if(index == 0) //night table 1
      {
        room_data_buf[4] = uart2_rxd_buf[8];
        f_check_temp_change = 1;
  //      room_data_buf[99] |= 0x02;            //현재온도 변경시 온도 Data 전송
      }
    }
    if((uart2_rxd_buf[10] & 0x7f) <= 70){ //Floor set temp
      if(room_data_buf[8+index] != uart2_rxd_buf[10])
      {
        room_data_buf[20+index] |= 0x80;
        room_data_buf[8+index] = uart2_rxd_buf[10];
        room_data_buf[99] |= 0x04;
      }
    }
    
    if((uart2_rxd_buf[11] & 0x7f) <= 70){    //Floor Check temp
      
      room_data_buf[12+index] = uart2_rxd_buf[11];
      f_check_temp_change_2 = 1;
//      room_data_buf[99] |= 0x02;            //현재온도 변경시 온도 Data 전송
    }    
    
    if((room_data_buf[4+index] & 0x7f) > room_data_buf[76]){   //이상온도 비교
      if(++em_temp_count[index] > 3){                 //이상온도 3회 이상 발생?
        em_temp_count[index] = 0;
        room_data_buf[42] |= (0x10 << index);         //이상 온도 발생 flag set
        //room_data_buf[45] |= 0x80;    //Fire sign ON
      }
    }
    else{
      //room_data_buf[42] &= (0xef << index);           //이상 온도 발생 flag clear
      switch(index)
      {
      case 0:
          room_data_buf[42] &= 0xef; //온도센서 1 em clear
        break;
      case 1:
          room_data_buf[42] &= 0xdf; //온도센서 2 em clear
        break;
      case 2:
          room_data_buf[42] &= 0xbf; //온도센서 3 em clear
        break;
      case 3:
          room_data_buf[42] &= 0x7f; //온도센서 4 em clear
        break;
      }
      em_temp_count[index] = 0;
      //room_data_buf[45] &= 0x7f;    //Fire sign OFF
    }    
    
    if(room_data_buf[40] & 0x02){   //Guest IN?
      if(nt_switch_toggle_bit[index][3] != uart2_rxd_buf[9]){    //TS switch push ?
        
        if((nt_switch_toggle_bit[index][3] & 0x01) != (uart2_rxd_buf[9] & 0x01)){
          
          room_data_buf[16+index] ^= 0x04;    //TS Air on/off toggle
          nt_switch_bit_check(K_fcu_auto_manual,0);
          room_data_buf[99] |= 0x02;
          room_data_buf[20+index] |= 0x08;    //Auto mode flag set
          room_data_buf[16] |= 0x80;
          ///air mode
        }
        if((nt_switch_toggle_bit[index][3] & 0x02) != (uart2_rxd_buf[9] & 0x02))
        {
          room_data_buf[16+index] ^=0x20;
          room_data_buf[99] |= 0x04;            //send flag 
           room_data_buf[20+index] |= 0x80;
          
          //floor mode
        }
        
        //if((nt_switch_toggle_bit[index][3] & 0x10) != (uart2_rxd_buf[9] & 0x10)) room_data_buf[24] ^= 0x08;    //TS FaC, DoC
        if((nt_switch_toggle_bit[index][3] & 0x20) != (uart2_rxd_buf[9] & 0x20)) ts_speed_execution(0);          //TS local speed execution

      } 
    }  
  nt_switch_toggle_bit[index][3] = uart2_rxd_buf[9];
  
}

void nt_switch_bit_check(unsigned char key_code, unsigned char index)
{
  switch(key_code)
  {
    case K_stand_master: 
          
          switch(cb_mode)
          {
          case 0:  //RCB-1
      
            switch(index)
            {
              
            case 0:
              
              if(room_data_buf[45] & 0x3e)
              {
                room_data_buf[45] &= 0xc1;
              }
              else 
              {
                room_data_buf[45] |= 0x3e;
              }
              
            break;
            }
          
          break;
          
          case 1: //RCB-2
             
             switch(index)
             {
             case 0:
                
                if(room_data_buf[45] & 0x1e)
                {
                   room_data_buf[45] &= 0xe1;
                }
                else
                {
                   room_data_buf[45] |= 0x1e;
                }
                break;
                  
             case 1:
             
                if(room_data_buf[47] & 0x0f)
                {
                   room_data_buf[47] &= 0xf0;
                }
                else
                {
                   room_data_buf[47] |= 0x0f;
                }
                break;
             }
          }
          
      break;
      
  case K_stand_master_2: 
              
            
      break;
      
    case K_rest_master:
      
      
      break;
      
      
      case K_rest_master_2:
        
   
        
      break;
      
    case K_stand_1:   
              
         switch(cb_mode)
         {
          case 0: //RCB - 1
           
              switch(index)
              {
                case 0:
                  
                  if(room_data_buf[45] & 0x02) room_data_buf[45] &= 0xfd;
                  else room_data_buf[45] |= 0x02;
                
                break;
              }
              
              break;
              
            case 1: //RCB - 
            
                switch(index)
                {
                  case 0:
                    
                    if(room_data_buf[45] & 0x02) room_data_buf[45] &= 0xfd;
                    else room_data_buf[45] |= 0x02;
                  
                  break;
                  
                  case 1: 
                  
                    if(room_data_buf[47] & 0x01) room_data_buf[47] &= 0xfe;
                    else room_data_buf[47] |= 0x01;
                  
                  break;

                }
            
              break;
         }
              
      break;
      
    case K_stand_2:  
      
         switch(cb_mode)
         {
          case 0: //RCB - 1
           
              switch(index)
              {
                case 0:
                  
                  if(room_data_buf[45] & 0x04) room_data_buf[45] &= 0xfb;
                  else room_data_buf[45] |= 0x04;
                
                break;
              }
              
              break;
              
            case 1: //RCB - 
            
                switch(index)
                {
                  case 0:
                    
                    if(room_data_buf[45] & 0x04) room_data_buf[45] &= 0xfb;
                    else room_data_buf[45] |= 0x04;
                  
                  break;
                  
                  case 1: 
                  
                    if(room_data_buf[47] & 0x02) room_data_buf[47] &= 0xfd;
                    else room_data_buf[47] |= 0x02;
                  
                  break;

                }
            
              break;
         }
      
      break;
      
    case K_stand_3: 
                  
         switch(cb_mode)
         {
          case 0: //RCB - 1
           
              switch(index)
              {
                case 0:
                  
                  if(room_data_buf[45] & 0x08) room_data_buf[45] &= 0xf7;
                  else room_data_buf[45] |= 0x08;
                
                break;
              }
              
              break;
              
            case 1: //RCB - 
            
                switch(index)
                {
                  case 0:
                    
                    if(room_data_buf[45] & 0x08) room_data_buf[45] &= 0xf7;
                    else room_data_buf[45] |= 0x08;
                  
                  break;
                  
                  case 1: 
                  
                    if(room_data_buf[47] & 0x04) room_data_buf[47] &= 0xfb;
                    else room_data_buf[47] |= 0x04;
                  
                  break;

                }
            
              break;
         }
      
      break;
      
    case K_stand_4:   
      
        switch(cb_mode)
         {
          case 0: //RCB - 1
           
              switch(index)
              {
                case 0:
                  
                  if(room_data_buf[45] & 0x10) room_data_buf[45] &= 0xef;
                  else room_data_buf[45] |= 0x10;
                
                break;
              }
              
              break;
              
            case 1: //RCB - 
            
                switch(index)
                {
                  case 0:
                    
                    if(room_data_buf[45] & 0x10) room_data_buf[45] &= 0xef;
                    else room_data_buf[45] |= 0x10;
                  
                  break;
                  
                  case 1: 
                  
                    if(room_data_buf[47] & 0x08) room_data_buf[47] &= 0xf7;
                    else room_data_buf[47] |= 0x08;
                  
                  break;

                }
            
              break;
         }
      
      break;
      
    case K_stand_5:   
              
          switch(cb_mode)
         {
         case 0: //RCB - 1
           
              switch(index)
              {
                case 0:
                  
                if(room_data_buf[45] & 0x20) room_data_buf[45] &= 0xdf;
                else room_data_buf[45] |= 0x20;
                
                break;
              }
              
              break;
         }
      
      break;
                      
    case K_stand_6:   
      
      switch(index)
      {
              case 0:   //침실 1
              case 1:
                      if(room_data_buf[45] & 0x40) room_data_buf[45] &= 0xbf;
                      else room_data_buf[45] |= 0x40; 
                      
                 break;
               
             
              
              case 2:   //화장실
                
                      if(room_data_buf[46] & 0x01) room_data_buf[46] &= 0xfe;
                      else room_data_buf[46] |= 0x01;
                
                break;
              
        
      }
      
      break;
      
    case K_stand_7:   

       switch(index)
        {
                case 0:   //침실 1
                case 1:
                        if(room_data_buf[45] & 0x80) room_data_buf[45] &= 0x7f;
                        else room_data_buf[45] |= 0x80; 
                        
                   break;
                 
               
                
                case 2:   //화장실
                  
                        if(room_data_buf[46] & 0x02) room_data_buf[46] &= 0xfd;
                        else room_data_buf[46] |= 0x02;
                  
                  break;
                
          
        }
      
    case K_stand_8:
      
    
              if(room_data_buf[47] & 0x01) room_data_buf[47] &= 0xfe;
              else                         room_data_buf[47] |= 0x01;
      
      break;
      
    case K_stand_9:
      
              if(room_data_buf[47] & 0x02) room_data_buf[47] &= 0xfd;
              else                         room_data_buf[47] |= 0x02;
              
      break;
      
    case K_stand_10:

              if(room_data_buf[47] & 0x04) room_data_buf[47] &= 0xfb;
              else                         room_data_buf[47] |= 0x04;      
      
      break;
      
    case K_stand_11:
    
              if(room_data_buf[47] & 0x08) room_data_buf[47] &= 0xf7;
              else                         room_data_buf[47] |= 0x08;       
      
      break;      
      
  case K_stand_12 :
    
              if(room_data_buf[47] & 0x10) room_data_buf[47] &= 0xef;
              else                         room_data_buf[47] |= 0x10;        
    
    break;
 
  case K_stand_13 :
       
    break;    
    
    case K_bed_master:  

      break;
      
  case K_curtain_1_open:
    
          curtain_sw_buf[0] = 0;
          curtain_sw_buf[0] |= 0x01; //curtain 1 open
      
      break;
      
    case K_curtain_1_stop:
          
          curtain_sw_buf[0] = 0;
          curtain_sw_buf[0] |= 0x02; //curtain 1 stop          
          
      break;
    
    case K_curtain_1_close:
    
          curtain_sw_buf[0] = 0;
          curtain_sw_buf[0] |= 0x04; //curtain 1 close          
      
      break;      
      
    case K_curtain_2_open:
    
          curtain_sw_buf[1] = 0;
          curtain_sw_buf[1] |= 0x01; //curtain 2 open          
      
      break;
      
    case K_curtain_2_stop:
    
          curtain_sw_buf[1] = 0;
          curtain_sw_buf[1] |= 0x02; //curtain 2 stop             
      
      break;
    
    case K_curtain_2_close:
          
          curtain_sw_buf[1] = 0;
          curtain_sw_buf[1] |= 0x04; //curtain 2 close             
      
      break;
      
      
    case K_exchanger:   //전열교환기
      //if(room_data_buf[46] & 0x04) room_data_buf[46] &= 0xfb;
      //else room_data_buf[46] |= 0x04;
      break;
    case K_entry_lamp:    //입구등
      //if(room_data_buf[45] & 0x02) room_data_buf[45] &= 0xfd;
      //else{
      //  room_data_buf[45] |= 0x02;
       // timer_1sec_entry = 0;
      //  enterance_lamp_delay_timer = 0;
      //  f_enterance_lamp_delay = 0;
      //}
      break;
      
    case K_dim_step_up : 
          if(dimmer_level[index] >= 0x31) dimmer_level[index] = 0x31;
          else dimmer_level[index] += 10;
    break;
    
    case K_dim_step_down : 
          if((dimmer_level[index]) < 10) dimmer_level[index] = 0;
          else dimmer_level[index] -= 10;
    break;
    
      
  
    case K_fcu_onoff: room_data_buf[16+index] ^= 0x04;  room_data_buf[16+index] |= 0x80;  room_data_buf[54+index] &= 0xf8;   
      if(room_data_buf[16+index] & 0x04) room_data_buf[20+index] |= 0x08; //auto
      else room_data_buf[20+index] &= 0xf7; //manual
      break;
    case K_fcu_low:   room_data_buf[16+index] |= 0x04;  room_data_buf[16+index] |= 0x80;  room_data_buf[54+index] &= 0xf8;   room_data_buf[54+index] |= 0x01;  
      room_data_buf[20+index] &= 0xf7; //manual
      break;
    case K_fcu_mid:   room_data_buf[16+index] |= 0x04;  room_data_buf[16+index] |= 0x80;  room_data_buf[54+index] &= 0xf8;   room_data_buf[54+index] |= 0x02;  
      room_data_buf[20+index] &= 0xf7; //manual
      break;
    case K_fcu_high:  room_data_buf[16+index] |= 0x04;  room_data_buf[16+index] |= 0x80;  room_data_buf[54+index] &= 0xf8;   room_data_buf[54+index] |= 0x03;  
      room_data_buf[20+index] &= 0xf7; //manual
      break;
    case K_fcu_auto_manual: room_data_buf[54+index] &= 0xf8;  room_data_buf[16+index] |= 0x80; break;
  
    case K_dnd: dnd_mur_execution('D','T'); break;
    case K_mur: dnd_mur_execution('M','T'); break;
    case K_call_wait:
    case K_emergency:
    case K_chime_push:
      break;
      
      
      

  }
  
}

//
//=============== Light switch receive data check routine ==============================
//
void light_switch_check(unsigned char index)
{
  if(f_first_ls_power_on[index])
  {
      if(room_data_buf[40] & 0x02){   //Guest IN?
        if((light_switch_toggle_bit[index][0] & 0x01) != (uart2_rxd_buf[3] & 0x01)) nt_switch_bit_check(b_ls_switch_kind_table[cb_mode][index][0],index);
        if((light_switch_toggle_bit[index][0] & 0x02) != (uart2_rxd_buf[3] & 0x02)) nt_switch_bit_check(b_ls_switch_kind_table[cb_mode][index][1],index);
        if((light_switch_toggle_bit[index][0] & 0x04) != (uart2_rxd_buf[3] & 0x04)) nt_switch_bit_check(b_ls_switch_kind_table[cb_mode][index][2],index);
        if((light_switch_toggle_bit[index][0] & 0x08) != (uart2_rxd_buf[3] & 0x08)) nt_switch_bit_check(b_ls_switch_kind_table[cb_mode][index][3],index);
        if((light_switch_toggle_bit[index][0] & 0x10) != (uart2_rxd_buf[3] & 0x10)) nt_switch_bit_check(b_ls_switch_kind_table[cb_mode][index][4],index);
        if((light_switch_toggle_bit[index][0] & 0x20) != (uart2_rxd_buf[3] & 0x20)) nt_switch_bit_check(b_ls_switch_kind_table[cb_mode][index][5],index);
      }
  }
  else f_first_ls_power_on[index] = 1;
  
  light_switch_toggle_bit[index][0] = uart2_rxd_buf[3];
  
}
//========================= dimmer receive data check routine =============================
//
void dimmer_check(unsigned char index)
{
  if(f_first_dimmer_power_on[index])
  {
    
  }
  else f_first_dimmer_power_on[index] = 1;
     
  dimmer_switch_toggle_bit[index] = uart2_rxd_buf[4];
}
  
void chime_ind_check(unsigned char index)
{
  if(f_first_ci_power_on[index])
  {
    if((chime_ind_toggle_bit[index] & 0x01) != (uart2_rxd_buf[3] & 0x01))
   {
      if(!f_chime_keep)
      {
        f_chime_keep = 1;
        chime_sq = 0;
        f_chime_sq_timer = 1;
        digital_volume_execution(0,room_data_buf[79]);
        
        handy_sign_timer = 0;
        //if(room_data_buf[40] & 0x02) // 입실일때
        //{
          room_data_buf[48] |= 0x80;
          f_chime_sign = 1;
        //}
        
      }
    }
  }
  else f_first_ci_power_on[index] = 1;
   
  chime_ind_toggle_bit[index] = uart2_rxd_buf[3]; 
}

void chime_ex_sub_check(void)
{
      if(!f_chime_keep)
      {
        f_chime_keep = 1;
        chime_sq = 0;
        f_chime_sq_timer = 1;
        digital_volume_execution(0,room_data_buf[79]);
        
        handy_sign_timer = 0;
        if(room_data_buf[40] & 0x02) // 입실일때
        {
          room_data_buf[48] |= 0x80;
          f_chime_sign = 1;
        }
        
      } 
}

//
//=============== Key sensor receive data check routine =================
//
void key_sensor_rx_check(unsigned char index)
{
  if(f_first_ks_power_on[index])
  {
    if(room_data_buf[40] & 0x02) 
    {
      if((ks_switch_toggle_bit[index] & 0x01) != (uart2_rxd_buf[3] & 0x01))
      {
        nt_switch_bit_check(K_dnd, index);
      }
      if((ks_switch_toggle_bit[index] & 0x02) != (uart2_rxd_buf[3] & 0x02))
      {
        nt_switch_bit_check(K_mur, index);
      }
    }
  }
  else f_first_ks_power_on[index] = 1; 
  
  ks_switch_toggle_bit[index] = uart2_rxd_buf[3];
  
  if(dip_switch_buf[0] & 0x40){   //통신형 K/S 사용하는가?
    f_ks_rx_ok = 1;
    ks_rx_timer = 0;
    ks_key_status = uart2_rxd_buf[4];
    f_ks_error = 0;
  }
  
}

//=======================================================================
//=======================================================================

