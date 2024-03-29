
/*=====================(C) COPYRIGHT 2008 Insem Inc.=========================
program 		:   
processor	  : STM32F103xx
compiler		: IAR 6.41A Compiler 
program BY 	: H.H.Hwang
date 			  : 2013.	  .
copy right 	: Plus - H.
===========================================================================*/
#define __UART3_H__

#include "stm32f10x_lib.h"
#include "main.h"

//========================================================================
/* UART3_initial	:								              */												
//========================================================================
void UART3_initial(void) {
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

  /* RS485 UART3 Direction port define */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
        
	 /* Enable the USART3 Pins Partial Software Remapping */
//	GPIO_PinRemapConfig(GPIO_Remap_USART3, DISABLE);  
        
	/* Enable GPIOB and USART3 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	 /* Enable USART3 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        
	/* Configure USART3 Tx (PB10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART3 Rx (PB11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

/* USART3 configuration ------------------------------------------------------*/
  /* USART3 configured as follow:
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
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//  USART_InitStructure.USART_Clock = USART_Clock_Disable;
// USART_InitStructure.USART_CPOL = USART_CPOL_Low;
//  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
//  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
  
  /* Configure the USART3 */
  USART_Init(USART3, &USART_InitStructure);
  
 /* Enable the USART Transmoit interrupt: this interrupt is generated when the 
   USART1 transmit data register is empty */  
//  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  
/* Enable the USART Receive interrupt: this interrupt is generated when the 
   USART3 receive data register is not empty */
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
  /* Enable USART3 */
  USART_Cmd(USART3, ENABLE);
  
  /* Enable the USART3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

width CrcCompute(unsigned char *message, unsigned int nBytes)
{
    unsigned int CRC16 = 0xffff;
    int i;
    
    for(i = 0;i<nBytes;i++)
    {
        CRC16 = (CRC16>>8) ^crcTable[(CRC16^message[i])&0x00ff];        
    }
    
    return CRC16;
}

void UART3_control_proc(void)
{
    if(f_uart3_data_send_2)
    {
      f_uart3_data_send_2 = 0;
      
      if(cb_mode == 2) //배선도 3 (복층 타입)
      {
          if(aircon_count == 2) aircon_count = 0;
          else aircon_count = 2;
      }
      else
      {
        aircon_count = 0; //에어컨 1대 Address NO.1
      }
      //if(f_aircon_on[aircon_count])
      //{    
             if(f_lg_on_off[aircon_count]) // 에어컨 ON,OFF 상태
             {
                f_lg_on_off[aircon_count] = 0;
                lg_aircon_control_send(aircon_count+1,0x05,0x00,0x00,b_lg_on_off[aircon_count],0x00);    // 데이터 입력
             }
             else if(f_lg_mode[aircon_count]) // 에어컨 운전모드 상태
             {
                f_lg_mode[aircon_count] = 0;  
                
                //if(b_lg_on_off[aircon_count] == 0xff) //전원 상태가 ON 인가 ??
                lg_aircon_control_send(aircon_count+1,0x06,0x00,0x00,0x00,b_lg_mode[aircon_count]);   // 데이터 입력
             }             
             else if(f_lg_temp[aircon_count]) // 에어컨 희망온도 상태
             {
                f_lg_temp[aircon_count] = 0;
                
                if(b_lg_on_off[aircon_count] == 0xff) //전원 상태가 ON 인가 ??
                lg_aircon_control_send(aircon_count+1,0x06,0x00,0x01,b_lg_temp[aircon_count][0],b_lg_temp[aircon_count][1]);   // 데이터 입력
             }
             else if(f_lg_wind[aircon_count]) // 에어컨 바람세기 상태
             {
                f_lg_wind[aircon_count] = 0;
                
                if(b_lg_on_off[aircon_count] == 0xff) //전원 상태가 ON 인가 ??
                lg_aircon_control_send(aircon_count+1,0x06,0x00,0x0E,0x00,b_lg_wind[aircon_count]);  // 데이터 입력
             }             
             else if(!f_lg_on_off[aircon_count] && !f_lg_mode[aircon_count] && !f_lg_temp[aircon_count] && !f_lg_wind[aircon_count])    //데이터 입력할께 없으면 모니터링 상태로 넘어간다
             {
                  lg_aircon_monitor_send(aircon_count+1,aircon_m_flag,0,m_TX1,m_TX2,m_TX3);     
             }
    //  }
    }
            
           // if(lg_monitoring_ct > 1) // -- 운전모드 모니터링 데이터 -- //
            //{
              aircon_m_flag = 0x01; 
              
              m_TX1 = 0x00; //TX DATA 2
              m_TX2 = 0x00; //TX DATA 3
              m_TX3 = 0x01; //TX DATA                         
              
            //}
           /* else // -- 실내온도 모니터링 데이터 -- //  
            {
              aircon_m_flag = 0x04; //모니터링

              m_TX1 = 0x02; //TX DATA 2
              m_TX2 = 0x00; //TX DATA 3
              m_TX3 = 0x01; //TX DATA 4     
              
            }*/
            // --------------------//

          switch(data_count_delay)
          {
                  case 0: // 바람세기
                  
                        if((room_data_buf[16+aircon_count] & 0x03) != (room_data_buf[109+aircon_count] & 0x03)) //바람세기
                        {            
                                room_data_buf[109+aircon_count] = (room_data_buf[16+aircon_count] & 0x03); 
                                no_send_wind = 0;
                                
                                //c_TX2 = 0x00;    // TX DATA 3
                                if(room_data_buf[109+aircon_count] == 0x03) { b_lg_wind[aircon_count] = 0x03; }
                                else if(room_data_buf[109+aircon_count] == 0x02) {  b_lg_wind[aircon_count] = 0x02; }
                                else if(room_data_buf[109+aircon_count] == 0x01) {  b_lg_wind[aircon_count] = 0x01;}  
                                else no_send_wind = 1; //바람세기 0일때 data 미전송
                                
                                if(!no_send_wind) f_lg_wind[aircon_count] = 1;
                        }
                        
                ++data_count_delay; 
               break; 
               
                case 1: // 운전모드  
                
                 // if((room_data_buf[16+aircon_count] & 0x03) != 0) //바람세기 OFF가 아닐때만 동작
                //  {
                /*    if(!f_aircon_on[aircon_count]) 
                    {
                                    if((room_data_buf[20+aircon_count] & 0x04) == 0) //난방모드일때
                                    {
                                        room_data_buf[105+aircon_count] = 0x04; //초기값 설정을 위한 임시냉방모드 설정
                                    }
                                    else room_data_buf[105+aircon_count] = 0x00; //초기값 설정을 위한 임시난방모드 설정
                    }
                     */                 
                   // f_aircon_on[aircon_count] = 1;
                    
                    if((room_data_buf[20+aircon_count] & 0x04) != (room_data_buf[105+aircon_count] & 0x04)) 
                    { 
                        room_data_buf[105+aircon_count] = (room_data_buf[20+aircon_count] & 0x04);
                        
                        if(!room_data_buf[105+aircon_count]) mode_data[aircon_count] = 0x04; // 난방
                        else if(room_data_buf[105+aircon_count] & 0x04) mode_data[aircon_count] = 0x00; // 냉방
                        
                        b_lg_mode[aircon_count] = mode_data[aircon_count]; // data   
                        f_lg_mode[aircon_count] = 1; // 모드 flag set  
                    }
                //  }
                ++data_count_delay;
                
            break;    
            
          case 2: // 희망온도

                    if((room_data_buf[16+aircon_count] & 0x03) != 0)
                    {
                        //if((room_data_buf[0+aircon_count] & 0xff) != (room_data_buf[101+aircon_count] & 0xff))//온도센서 희망온도
                        if(on_off_data[aircon_count] != room_data_buf[101+aircon_count])
                        {
                              //data_flag = 1; //flag set
                              
                              //room_data_buf[101+aircon_count] = (room_data_buf[0+aircon_count] & 0xff);
                              
                              room_data_buf[101+aircon_count]  = on_off_data[aircon_count];
                              
                              if(mode_data[aircon_count] == 0x04) { ondo_set_temp_1 = 0x01; ondo_set_temp_2 = 0x2C; } //난방 30도 고정
                              else if(mode_data[aircon_count] == 0x00) { ondo_set_temp_1 = 0x00; ondo_set_temp_2 = 0xB4; } //냉방 18도 고정
                              
                             /* ondo_settemp_data = ((room_data_buf[0+aircon_count] & 0x7f) * 10);
                              if(room_data_buf[0+aircon_count] & 0x80) ondo_settemp_data += 5;
                              
                              if(ondo_settemp_data >=260)
                              {
                                  ondo_set_temp_1 = 0x01;
                                  if(ondo_settemp_data>=300)
                                    ondo_set_temp_2 = 0x2c;
                                  else
                                    ondo_set_temp_2 =ondo_settemp_data- 256;
                              }
                              else
                              {
                                  ondo_set_temp_1 = 0x00;
                                  if(ondo_settemp_data <=180)
                                    ondo_set_temp_2 = 0xb4;
                                  else
                                    ondo_set_temp_2 = ondo_settemp_data;
                              }*/
                                                          
                              b_lg_temp[aircon_count][0] = ondo_set_temp_1; // data3
                              b_lg_temp[aircon_count][1] = ondo_set_temp_2; // data4
                                
                              f_lg_temp[aircon_count] = 1;// 희망온도 flag set
                        }
                    }
                
                ++data_count_delay;
         break;     
            
            case 3: // ON/OFF
                    
                    if((room_data_buf[16+aircon_count] & 0x03) != (f_ondo_power_set[aircon_count])) 
                    {                                                                                                                                                                                               
                          f_ondo_power_set[aircon_count] = (room_data_buf[16+aircon_count] & 0x03);
                          if((room_data_buf[16+aircon_count] & 0x03) == 0) on_off_data[aircon_count] = 0x00;
                          else           on_off_data[aircon_count] = 0xff;
                          
                          if(b_lg_on_off[aircon_count] != on_off_data[aircon_count]) no_send_power = 1; //기존 데이터랑 새로운 데이터랑 같느나 ?? 다르면 전송해라
                          else no_send_power = 0; //다르지 않으면 전송하지 마라
                          
                          b_lg_on_off[aircon_count] = on_off_data[aircon_count]; // data2       
                          
                          if(no_send_power == 1)
                          {
                          f_lg_on_off[aircon_count] = 1; // 전원 flag set
                            if(on_off_data[aircon_count] == 0xff) //전원 ON일경우
                            {
                                      if((room_data_buf[20+aircon_count] & 0x04) == 0) //난방모드일때
                                      {
                                          room_data_buf[105+aircon_count] = 0x04; //초기값 설정을 위한 임시냉방모드 설정
                                      }
                                      else room_data_buf[105+aircon_count] = 0x00; //초기값 설정을 위한 임시난방모드 설정      
                                      
                                      room_data_buf[101+aircon_count] = 0; //ON일 경우 희망온도 클리어
                            }
                          }
                    }
                    
                    /*else if(!f_aircon_on[aircon_count])
                    {
                        f_lg_on_off[aircon_count] = 1; // 전원 flag set
                        f_aircon_on[aircon_count] = 1; 
                    }*/
                    
                    // ------------------ //
                data_count_delay = 0; 
            break;       
          }
          
          UART3_tx_check();
          UART3_rx_check();
}

void lg_aircon_control_send(unsigned char index,unsigned char data_code0,unsigned char data_code1,unsigned char data_code2,unsigned char data_code3,unsigned char data_code4)   //에어컨 컨트롤 하는 부분 수신으로 아무 값도 받지 않는다
{
  unsigned char i = 0;
  unsigned char CRC_Low,CRC_High;
  unsigned int bcc_buf;
          
  P_UART3_DIR = 1;
  u3_dir_delay_counter = 0;
  
  uart3_tx_data_buf[i++] = index;      //index
  uart3_tx_data_buf[i++] = data_code0;  //TX1
  uart3_tx_data_buf[i++] = data_code1;  //TX2(고정)
  uart3_tx_data_buf[i++] = data_code2; //TX3
  uart3_tx_data_buf[i++] = data_code3; //TX4
  uart3_tx_data_buf[i++] = data_code4; //TX5
  
  bcc_buf = CrcCompute(&uart3_tx_data_buf[0],6);
  CRC_Low = (unsigned char)bcc_buf;
  CRC_High = (unsigned char)(bcc_buf >> 8);  
          
  uart3_tx_data_buf[i++] = CRC_Low;
  uart3_tx_data_buf[i++] = CRC_High;  
  
  f_uart3_data_send = 1;
  uart3_tx_length = 0;
  uart3_tx_backup = i;    
}

void lg_aircon_monitor_send(unsigned char index,unsigned char data_code,unsigned char data_code1,unsigned char data_code2,unsigned char data_code3,unsigned char data_code4)    //모니터링하는 부분 tx
{
  unsigned char i = 0;
  unsigned char CRC_Low,CRC_High;
  unsigned int bcc_buf;
  
  P_UART3_DIR = 1;
  u3_dir_delay_counter = 0;
  
  uart3_tx_data_buf[i++] = index;         //index
  uart3_tx_data_buf[i++] = data_code;  //TX1
  uart3_tx_data_buf[i++] = data_code1; //TX2
  uart3_tx_data_buf[i++] = data_code2; //TX3
  uart3_tx_data_buf[i++] = data_code3; //TX4
  uart3_tx_data_buf[i++] = data_code4; //TX5
  
  bcc_buf = CrcCompute(&uart3_tx_data_buf[0],6);
  CRC_Low = (unsigned char)bcc_buf;
  CRC_High = (unsigned char)(bcc_buf >> 8);  
          
  uart3_tx_data_buf[i++] = CRC_Low;
  uart3_tx_data_buf[i++] = CRC_High;  
  
  f_uart3_data_send = 1;
  uart3_tx_length = 0;
  uart3_tx_backup = i;    
}

/* UART3 data send check routine */
void UART3_tx_check(void)
{
  if(f_uart3_data_start){
      f_uart3_data_send = 0;
	  if(USART_GetFlagStatus(USART3, USART_FLAG_TC) !=RESET) {
      if(uart3_tx_length < uart3_tx_backup){
        USART3->DR = (uart3_tx_data_buf[uart3_tx_length++] & (u16)0x01FF);
      }
      else{
        P_UART3_DIR = 0;
        f_uart3_data_send = 0;
        f_uart3_data_start = 0;
      }
    }
  }
}

//
//========== LG 에어컨 data 수신 처리 routine =============
//
void UART3_rx_check(void)
{
  if(f_uart3_frame_rx_ok)       //수신완료
  {
    f_uart3_frame_rx_ok = 0;
    led4 ^= 1;
    
    switch(uart3_rxd_buf[0]) //AIRCON address
    {
      case 0x01: //address 1
        
                    if(uart3_rxd_buf[1] & 0x01) //CMD        운전관련 데이터를 받음
                    {
                        if(uart3_rxd_buf[0] & 0x01) // 에어컨 NUM 1
                        {
                            if(uart3_rxd_buf[3] !=  (b_lg_on_off[0] & 0x01)) //운전 ON,OFF DATA     받는 데이터와 가지고 있는 데이터를 비교
                            {
                                ++aircon_power_counter[0];
                            }
                            else aircon_power_counter[0] = 0;
                        }
                        
                        if(aircon_power_counter[0] > 2) //에어컨 1 재전송 (3회이상 다를시)  쓰레기값 방지
                        {
                          aircon_power_counter[0] = 0;
                          f_lg_on_off[0] = 1; //운전 ON,OFF 재전송
                          
                          if(b_lg_on_off[0] == 0xff) //전원ON 상태일시
                          {
                              f_lg_on_off[0] = 1; //ON,OFF
                              f_lg_mode[0] = 1; //모드
                              f_lg_temp[0] = 1; //희망온도
                              f_lg_wind[0] = 1; //풍량
                          }     
                          else f_lg_on_off[0] = 1; //ON,OFF
                        }
                    }
                    
        break;
        
        case 0x02: //address 2
          
                      if(uart3_rxd_buf[1] & 0x01) //CMD        운전관련 데이터를 받음
                      {
                          if(uart3_rxd_buf[0] & 0x02) // 에어컨 NUM 1
                          {
                              if(uart3_rxd_buf[3] !=  (b_lg_on_off[1] & 0x01)) //운전 ON,OFF DATA     받는 데이터와 가지고 있는 데이터를 비교
                              {
                                  ++aircon_power_counter[1];
                              }
                              else aircon_power_counter[1] = 0;
                          }
                          
                          if(aircon_power_counter[1] > 2) //에어컨 1 재전송 (3회이상 다를시)  쓰레기값 방지
                          {
                            aircon_power_counter[1] = 0;
                            f_lg_on_off[1] = 1; //운전 ON,OFF 재전송
                            
                            if(b_lg_on_off[1] == 0xff) //전원ON 상태일시
                            {
                                f_lg_on_off[1] = 1; //ON,OFF
                                f_lg_mode[1] = 1; //모드
                                f_lg_temp[1] = 1; //희망온도
                                f_lg_wind[1] = 1; //풍량
                            }     
                            else f_lg_on_off[1] = 1; //ON,OFF
                          }
                      }
                      
          break;        
    }
  }
}
