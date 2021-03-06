
/*=====================(C) COPYRIGHT 2008 Insem Inc.=========================
program 		:   
processor	  : STM32F103xx
compiler		: IAR 6.41A Compiler 
program BY 	: H.H.Hwang
date 			  : 2021.03.04 [DTC 에어컨 연동 LG 삼성]
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
        
    if(rcu_dip_aircon)     //LG에어컨 SET
    {
      USART_InitStructure.USART_BaudRate = 9600;
      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      USART_InitStructure.USART_StopBits = USART_StopBits_1;
      USART_InitStructure.USART_Parity = USART_Parity_No;
      USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    }
    else //삼성 PLC SET
    {
      USART_InitStructure.USART_BaudRate = 9600;
      USART_InitStructure.USART_WordLength = USART_WordLength_9b;
      USART_InitStructure.USART_StopBits = USART_StopBits_1;
      USART_InitStructure.USART_Parity = USART_Parity_Even;
      USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    
          
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    }
    
//  USART_InitStructure.USART_Clock = USART_Clock_Disable;
//  USART_InitStructure.USART_CPOL = USART_CPOL_Low;
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
    if(rcu_dip_aircon) // LG 에어컨일때
    {
      if(f_uart3_data_send_2)
      {
        f_uart3_data_send_2 = 0;
      
        if(cb_mode != 0) //일반실이 아니면?
        {
           if(++aircon_count > 1) aircon_count = 0;
        }
        else aircon_count = 0; //에어컨 1대 Address NO.1
      
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
      if(rcu_dip_aircon) // LG에어컨 연동
      {
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
      }
      
    }
    else //삼성 PLC 에어컨
    {
      if(f_uart3_data_send_2_ss) //40초에 한번 동작됨
      {
        f_uart3_data_send_2_ss = 0;
        
        if(cb_mode == 1) //스위트룸
        {
            if(!f_address_set_ok) //어드레스 설정이 되지 않았을때만
            {
              if((f_ss_power_on[ss_air_address[0]]) && (f_ss_power_on[ss_air_address[1]])) //에어컨 두대모두 어드레스를 받았는가??
              {
                if((ss_air_address[0]) > (ss_air_address[1])) //1번 에어컨의 어드레스가 2번보다 큰가?
                {
                  air_ad_save[0] = ss_air_address[0]; //에어컨 1번 어드레스 저장
                  ss_air_address[0] = ss_air_address[1]; //에어컨 2번 어드레스를 1번으로 저장
                  ss_air_address[1] = air_ad_save[0]; //에어컨 1번 어드레스를 2번으로 저장
                  
                  f_address_set_ok = 1; //어드레스 설정완료
                  
                  f_plc_add_toggle[1] = 0; //에어컨2  연결 OK
                  aircon_no_respones[1] = 0; //에어컨2 연결 OK    
                          
                  f_plc_add_toggle[1] = 0; //에어컨2  연결 OK
                  aircon_no_respones[1] = 0; //에어컨2 연결 OK  
                  
                  f_ss_power_on[ss_air_address[0]] = 1; //1번 에어컨 설정 OK
                  f_ss_power_on[ss_air_address[1]] = 1; //2번 에어컨 설정 OK
                }
                else //에어컨 1번 어드레스가 2번보다 더 작을때는 OK
                {
                  f_address_set_ok = 1; //어드레스 설정완료
                  
                  f_plc_add_toggle[1] = 0; //에어컨2  연결 OK
                  aircon_no_respones[1] = 0; //에어컨2 연결 OK    
                          
                  f_plc_add_toggle[1] = 0; //에어컨2  연결 OK
                  aircon_no_respones[1] = 0; //에어컨2 연결 OK                 
                }
              }
            }
            
            if(((!f_ss_power_on[ss_air_address[0]]) || (!f_ss_power_on[ss_air_address[1]]))) 
            {
              ss_air_install_status(ss_data_counter);
            }      
        }
        else 
        {
            f_address_set_ok = 1; //어드레스 설정 완료 강제설정(에어컨 1대일 경우)
          
            if((!f_ss_power_on[ss_air_address[0]])) 
            {
              ss_air_install_status(ss_data_counter);  
            }      
        }
      }
      if(f_uart3_data_send_ok)
      {
        f_uart3_data_send_ok = 0;
      
        if(f_address_set_ok) //어드레스 설정 완료일때 데이터 전송
        {
            if(f_ss_power_on[ss_air_address[ss_data_counter]])
            {
              if(ss_control_monitor[ss_air_address[ss_data_counter]] >= 1)
              {
                ss_control_data_set(ss_air_address[ss_data_counter], ss_data_counter);
              }
              else
              {
                if(f_uart3_data_send_3_ss)
                {
                  f_uart3_data_send_3_ss = 0;
                  ss_control_data_set(ss_air_address[ss_data_counter], ss_data_counter);
                }
                else if(f_uart3_data_send_4_ss)
                {
                  f_uart3_data_send_4_ss = 0;
                  ss_nofication_sensor(ss_air_address[ss_data_counter]);
                }
              }
            }
            
                
               if(cb_mode == 1) //스위트룸
               {
                    if(++ss_data_counter > 1) ss_data_counter = 0; //에어컨 2대
               }
               else ss_data_counter = 0; //에어컨 1대 
        }
      }
      if((ss_fan_speed[ss_air_address[ss_data_counter]] & 0x03) != (room_data_buf[16+ss_data_counter] & 0x03))
      {
        ss_fan_speed[ss_air_address[ss_data_counter]] = (room_data_buf[16+ss_data_counter] & 0x03);
        
        //전원 on,off start
        if((room_data_buf[16+ss_data_counter] & 0x03) != 0)  
        {
          ss_onoff_status[ss_data_counter] = 0x01;
        }
        else
        {
          ss_onoff_status[ss_data_counter] = 0x00;      
        }
        //전원 on,off end
        
        //fan start
         switch(room_data_buf[16+ss_data_counter] & 0x03)
        {
          case 0 : break;
          case 1 : ss_fan_speed[ss_data_counter] = 1; break;
          case 2 : ss_fan_speed[ss_data_counter] = 2; break;
          case 3 : ss_fan_speed[ss_data_counter] = 3; break;
        }   
        //fan end
        
        ++ss_control_monitor[ss_air_address[ss_data_counter]];
      }
      
      if((room_data_buf[110+ss_data_counter] & 0x04) != (room_data_buf[20+ss_data_counter] & 0x04)) //난방,냉방 모드 비교
      {
        room_data_buf[110+ss_data_counter] = (room_data_buf[20+ss_data_counter] & 0x04);
        if(room_data_buf[110+ss_data_counter] & 0x04) 
        {
          ss_control_mode[ss_data_counter] = 0x01;
        }
        else                                         
        {
          ss_control_mode[ss_data_counter] = 0x04;
        }
        ++ss_control_monitor[ss_air_address[ss_data_counter]];
      }
      
      if(room_data_buf[110+ss_data_counter] & 0x04) ss_set_temp[ss_air_address[ss_data_counter]] = 18;
      else ss_set_temp[ss_air_address[ss_data_counter]] = 30;
      
      
    }
    UART3_frame_proc();
  }
  
//=======================================
void UART3_frame_proc()
{
  UART3_tx_check();
  UART3_rx_check();
}
//=======================================


//=====================================================================
//              SAMSUNG Aircon Preamble
//====================================================================
void ss_preamble(void)
{
  uart3_txding_point = 0;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //10
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //20
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //30
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //40
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //50
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //60
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //70
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //80
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //90
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;
  uart3_tx_data_buf[uart3_txding_point++] = 0x55;                       //100
  
}

//====================================================================
//                      SAMSUNG Aircon Install status Request
//====================================================================
void ss_air_install_status(unsigned char address)
{
  int crc = 0;
  P_UART3_DIR = 1;
  
  //ss_preamble();
  uart3_tx_data_buf[uart3_txding_point++] = STX_SSAIR;
  ++uart3_txding_point;
  ++uart3_txding_point;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x51;
  uart3_tx_data_buf[uart3_txding_point++] = 0x00;
  uart3_tx_data_buf[uart3_txding_point++] = 0x00;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0xB0;
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  
  uart3_tx_data_buf[uart3_txding_point++] = SSAIR_COMMAND;
  uart3_tx_data_buf[uart3_txding_point++] = SS_COMMAND_state_normal | SS_COMMAND_type_read;
  uart3_tx_data_buf[uart3_txding_point++] = ss_packet_counter;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x02;
  
  uart3_tx_data_buf[uart3_txding_point++] = SS_long_variable;
  uart3_tx_data_buf[uart3_txding_point++] = 0x01;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  
  uart3_tx_data_buf[uart3_txding_point++] = SS_long_variable;
  uart3_tx_data_buf[uart3_txding_point++] = 0x02;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
  
  crc = CrcCompute(&uart3_tx_data_buf[3],(uart3_txding_point-3));
  
  uart3_tx_data_buf[uart3_txding_point++] = ((crc >> 8) & 0xff);
  uart3_tx_data_buf[uart3_txding_point++] = (crc & 0xff);
  
  uart3_tx_data_buf[uart3_txding_point++] = ETX_SSAIR;
  
  uart3_tx_data_buf[1] = ((uart3_txding_point - 2) >> 8);
  uart3_tx_data_buf[2] = ((uart3_txding_point - 2) & 0xff);
  
  f_uart3_data_send = 1;
  f_ss_send_eco_x = 1;
  uart3_tx_backup = uart3_txding_point;
  uart3_tx_length = 0;
  uart3_txding_point = 0;
  ++ss_packet_counter;
}

//=====================================================================
//              SAMSUNG Airconditioner monitoring or control
//=====================================================================
void ss_control_data_set(unsigned char address, unsigned char buf_addr)
{
  int crc = 0;
  
  P_UART3_DIR = 1;
  //ss_preamble();
  
  uart3_tx_data_buf[uart3_txding_point++] = STX_SSAIR;                  //STX Samsung
  ++uart3_txding_point;
  ++uart3_txding_point;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x51;
  uart3_tx_data_buf[uart3_txding_point++] = 0x00;
  uart3_tx_data_buf[uart3_txding_point++] = 0x00;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x20;
  uart3_tx_data_buf[uart3_txding_point++] = ss_outdoor_address[ss_data_counter];
  uart3_tx_data_buf[uart3_txding_point++] = address;
  
  if(ss_control_monitor[address] >= 1)
  {
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_COMMAND;
      uart3_tx_data_buf[uart3_txding_point++] = SS_COMMAND_state_normal | SS_COMMAND_type_request;
      uart3_tx_data_buf[uart3_txding_point++] = ss_packet_counter;
      
      uart3_tx_data_buf[uart3_txding_point++] = 0x05;
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_enumeration;
      uart3_tx_data_buf[uart3_txding_point++] = 0x50;
      uart3_tx_data_buf[uart3_txding_point++] = 0x01; //BUZZER X 
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_enumeration;
      uart3_tx_data_buf[uart3_txding_point++] = 0x00;
      uart3_tx_data_buf[uart3_txding_point++] = ss_onoff_status[buf_addr];
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_enumeration;
      uart3_tx_data_buf[uart3_txding_point++] = 0x01;
      uart3_tx_data_buf[uart3_txding_point++] = ss_control_mode[buf_addr];
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_enumeration;
      uart3_tx_data_buf[uart3_txding_point++] = 0x06;
      uart3_tx_data_buf[uart3_txding_point++] = ss_fan_speed[buf_addr];
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_variable;
      uart3_tx_data_buf[uart3_txding_point++] = 0x01;
      ss_set_temp_dec = (((int)(ss_set_temp[address] & 0x7f)) * 10);
      if(ss_set_temp[address] & 0x80) ss_set_temp_dec += 5;
      
      uart3_tx_data_buf[uart3_txding_point++] = (ss_set_temp_dec >> 8);
      uart3_tx_data_buf[uart3_txding_point++] = (ss_set_temp_dec & 0x00ff);
      
      crc = CrcCompute(&uart3_tx_data_buf[3], (uart3_txding_point-3));
      
      uart3_tx_data_buf[uart3_txding_point++] = (crc >> 8);
      uart3_tx_data_buf[uart3_txding_point++] = (crc & 0x00ff);
      
      uart3_tx_data_buf[uart3_txding_point++] = ETX_SSAIR;
      
      uart3_tx_data_buf[1] = ((uart3_txding_point - 2) >> 8);
      uart3_tx_data_buf[2] = ((uart3_txding_point - 2) & 0xff);
      
      f_uart3_data_send = 1;
      f_ss_send_eco_x = 1;
      uart3_tx_backup = uart3_txding_point;
      uart3_tx_length = 0;
      uart3_txding_point = 0;
      ++ss_packet_counter;
  }
  else
  {
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_COMMAND;
      uart3_tx_data_buf[uart3_txding_point++] = SS_COMMAND_state_normal | SS_COMMAND_type_read;
      uart3_tx_data_buf[uart3_txding_point++] = ss_packet_counter;
      
      uart3_tx_data_buf[uart3_txding_point++] = 0x07;
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_enumeration;
      uart3_tx_data_buf[uart3_txding_point++] = 0x00;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_enumeration;
      uart3_tx_data_buf[uart3_txding_point++] = 0x01;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_enumeration;
      uart3_tx_data_buf[uart3_txding_point++] = 0x06;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_variable;
      uart3_tx_data_buf[uart3_txding_point++] = 0x01;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_variable;
      uart3_tx_data_buf[uart3_txding_point++] = 0x41;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      
      uart3_tx_data_buf[uart3_txding_point++] = SS_variable;
      uart3_tx_data_buf[uart3_txding_point++] = 0x02;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      
      uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_variable;
      uart3_tx_data_buf[uart3_txding_point++] = 0x03;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;
      uart3_tx_data_buf[uart3_txding_point++] = 0xFF;      
      
      crc = CrcCompute(&uart3_tx_data_buf[3], (uart3_txding_point-3));
    
      uart3_tx_data_buf[uart3_txding_point++] = (crc >> 8);
      uart3_tx_data_buf[uart3_txding_point++] = (crc & 0x00ff);
      
      uart3_tx_data_buf[uart3_txding_point++] = ETX_SSAIR;
      
      uart3_tx_data_buf[1] = ((uart3_txding_point - 2) >> 8);
      uart3_tx_data_buf[2] = ((uart3_txding_point - 2) & 0xff);
      
      f_uart3_data_send = 1;
      f_ss_send_eco_x = 1;
      uart3_tx_backup = uart3_txding_point;
      uart3_tx_length = 0;
      uart3_txding_point = 0;
      ++ss_packet_counter;
      
      
    
  }
  
  
}

//======================================================================
//              SAMSUNG Sensor Nofication
//======================================================================
void ss_nofication_sensor(unsigned char address)
{
  int crc = 0;
  
  P_UART3_DIR = 1;
  //ss_preamble();
  
  uart3_tx_data_buf[uart3_txding_point++] = STX_SSAIR;                  //STX Samsung
  ++uart3_txding_point;
  ++uart3_txding_point;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x51;
  uart3_tx_data_buf[uart3_txding_point++] = 0x00;
  uart3_tx_data_buf[uart3_txding_point++] = 0x00;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x20;
  uart3_tx_data_buf[uart3_txding_point++] = ss_outdoor_address[0];
  uart3_tx_data_buf[uart3_txding_point++] = address;
  
  uart3_tx_data_buf[uart3_txding_point++] = SSAIR_COMMAND;
  uart3_tx_data_buf[uart3_txding_point++] = SS_COMMAND_state_normal | SS_COMMAND_type_nofication;
  uart3_tx_data_buf[uart3_txding_point++] = ss_packet_counter;
  
  uart3_tx_data_buf[uart3_txding_point++] = 0x02;
  
  uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_enumeration;
  uart3_tx_data_buf[uart3_txding_point++] = 0x76;
  
  if(!f_send_tempsensor_data) //현재온도 전송 비활성화 모드
  {
    uart3_tx_data_buf[uart3_txding_point++] = 0x00;
    
    uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_variable;
    uart3_tx_data_buf[uart3_txding_point++] = 0x3A;
    uart3_tx_data_buf[uart3_txding_point++] = 0x00;
    uart3_tx_data_buf[uart3_txding_point++] = 0x00;    
  }
  else //현재온도 전송 활성화 모드
  {
    uart3_tx_data_buf[uart3_txding_point++] = 0x01; //유선리모컨 실내온도센서 사용 설정
    
    uart3_tx_data_buf[uart3_txding_point++] = SSAIR_CONTROL_MONITOR | SS_variable;
    uart3_tx_data_buf[uart3_txding_point++] = 0x3A;
    if(cb_mode == 1) //스위트룸
    {
      if(ss_air_address[0] == address) //실내기 0번 온도조절기 1번 실내온도 전송
      {
        uart3_tx_data_buf[uart3_txding_point++] = room_temperature[0] = ((room_data_buf[4] & 0x7f) & 0xff00) >> 8;
        uart3_tx_data_buf[uart3_txding_point++] = room_temperature[1] = ((room_data_buf[4] & 0x7f) & 0x00ff);        
      }
      else if(ss_air_address[1] == address) //실내기 1번 온도조절기 2번 실내온도 전송
      {
        uart3_tx_data_buf[uart3_txding_point++] = room_temperature[0] = ((room_data_buf[5] & 0x7f) & 0xff00) >> 8;
        uart3_tx_data_buf[uart3_txding_point++] = room_temperature[1] = ((room_data_buf[5] & 0x7f) & 0x00ff);          
      }
    }
    else //에어컨 1대 객실
    {
      uart3_tx_data_buf[uart3_txding_point++] = room_temperature[0] = ((room_data_buf[4] & 0x7f) & 0xff00) >> 8;
      uart3_tx_data_buf[uart3_txding_point++] = room_temperature[1] = ((room_data_buf[4] & 0x7f) & 0x00ff);
    }
  }
  crc = CrcCompute(&uart3_tx_data_buf[3], (uart3_txding_point-3));
    
  uart3_tx_data_buf[uart3_txding_point++] = (crc >> 8);
  uart3_tx_data_buf[uart3_txding_point++] = (crc & 0x00ff);
      
  uart3_tx_data_buf[uart3_txding_point++] = ETX_SSAIR;
      
  uart3_tx_data_buf[1] = ((uart3_txding_point - 2) >> 8);
  uart3_tx_data_buf[2] = ((uart3_txding_point - 2) & 0xff);
      
  f_uart3_data_send = 1;
  f_ss_send_eco_x = 1;
  uart3_tx_backup = uart3_txding_point;
  uart3_tx_length = 0;
  uart3_txding_point = 0;
  ++ss_packet_counter;
}
  
//======================================================================
//                        UART3 Tx Check Routine
//======================================================================
void UART3_tx_check(void)
{
  if(f_uart3_data_start)
  {
    f_uart3_data_send = 0;
    if(USART_GetFlagStatus(USART3, USART_FLAG_TC) !=RESET) 
    {
      if(uart3_tx_length < uart3_tx_backup)
      {
        USART3->DR = (uart3_tx_data_buf[uart3_tx_length++] & (u16)0x01FF);
      }
      else
      {
        P_UART3_DIR = 0;
        f_uart3_data_send = 0;
        f_uart3_data_start = 0;
        f_ss_send_eco_x = 0;
      }
    }
  }
}
///////////////////////////////////////////////////////////////


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



//
//========== LG 에어컨 data 수신 처리 routine =============
//
void UART3_rx_check(void)
{
  if(rcu_dip_aircon) //LG
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
                          if(uart3_rxd_buf[0] & 0x02) // 에어컨 NUM 2
                          {
                              if(uart3_rxd_buf[3] !=  (b_lg_on_off[0] & 0x01)) //운전 ON,OFF DATA     받는 데이터와 가지고 있는 데이터를 비교
                              {
                                  ++aircon_power_counter[1];
                              }
                              else aircon_power_counter[1] = 0;
                          }
                          
                          if(aircon_power_counter[1] > 2) //에어컨 2 재전송 (3회이상 다를시)  쓰레기값 방지
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
  else // 삼성 PLC
  {
    short crc = 0;
    short ss_set_temp_buf = 0; 
    if(f_uart3_frame_rx_ok)
    {
      led4 ^= 1;
      
      f_uart3_frame_rx_ok = 0;
      crc = CrcCompute(&uart3_rxd_buf[3],(uart3_rxd_buf[2]-4));
      
        if((uart3_rxd_buf[uart3_rxd_buf[2]-1] == ((unsigned char)(crc >> 8))) && (uart3_rxd_buf[uart3_rxd_buf[2]] == ((unsigned char)(crc & 0xff))))
        {
          if(uart3_rxd_buf[3] == 0x20) //SA
          {
            if(uart3_rxd_buf[5] == ss_air_address[0])
            {
                         f_plc_add_toggle[0] = 0; //에어컨1  연결 OK
                        aircon_no_respones[0] = 0; //에어컨1 연결 OK   
                        
                         f_plc_add_toggle[1] = 0; //에어컨2  연결 OK
                        aircon_no_respones[1] = 0; //에어컨2 연결 OK                        
            }
            
            if(uart3_rxd_buf[6] == 0xB0)
            {
              if(uart3_rxd_buf[9] == SSAIR_COMMAND)
              {
                if((uart3_rxd_buf[10] & 0x30) == 0x00)
                {
                  f_ss_reset = 1;
                }
                if(f_ss_reset)                            
                {
                  if((uart3_rxd_buf[10] & 0x30) == 0x30)          //20170313 RESET 추가 항목
                  {
                    f_ss_reset = 0;
                    f_address_set_ok = 0;
                    f_ss_power_on[ss_air_address[0]] = 0;
                    if(cb_mode == 1) f_ss_power_on[ss_air_address[1]] = 0;       
                  }
                }
              }
            }
            if(uart3_rxd_buf[6] == 0x51)             //SS Install Status
            {
              if(uart3_rxd_buf[9] == SSAIR_COMMAND)
              {
                if(uart3_rxd_buf[10] == (SS_COMMAND_state_normal | SS_COMMAND_type_response))
                {
                  if(uart3_rxd_buf[12] == 0x02)
                  {
                    if(!f_ss_power_on[uart3_rxd_buf[18]])
                    {
                      if(uart3_rxd_buf[3] == uart3_rxd_buf[16])
                      {
                                if(!f_ss_first_addr) //1번 에어컨
                                {          
                                   //1번 에어컨
                                  ss_outdoor_address[0] = uart3_rxd_buf[17];
                                  ss_air_address[0] = uart3_rxd_buf[18];
                                  f_ss_power_on[ss_air_address[0]] = 1; 
                                  if(room_data_buf[20] & 0x04) room_data_buf[110] &= 0xfb;
                                  else                         room_data_buf[110] |= 0x04;                         
                                  f_ss_first_addr = 1;                                  
                                }
                                else
                                {
                                  //2번 에어컨
                                  ss_outdoor_address[1] = uart3_rxd_buf[17];
                                  ss_air_address[1] = uart3_rxd_buf[18];
                                  f_ss_power_on[ss_air_address[1]] = 1;
                                  if(room_data_buf[21] & 0x04) room_data_buf[111] &= 0xfb;
                                  else                         room_data_buf[111] |= 0x04;                                   
                                }
                      }
                    }
                  }
                  else if(uart3_rxd_buf[12] == 0x07)              //SS Monitoring
                  {
                    if(uart3_rxd_buf[5] == ss_air_address[0])
                    {
                      if(f_ss_power_on[ss_air_address[0]]) //에어컨 1
                      {   
                        if(uart3_rxd_buf[15] != ss_onoff_status[0])
                        {
                          ss_control_monitor[ss_air_address[0]] = 1;
                        }
                        if(uart3_rxd_buf[18] != ss_control_mode[0])
                        {
                          ss_control_monitor[ss_air_address[0]] = 1;
                        }
                        if(uart3_rxd_buf[21] != ss_fan_speed[0])
                        {
                          ss_control_monitor[ss_air_address[0]] = 1;
                        }
                        ss_set_temp_buf = (((int)(ss_set_temp[0] & 0x7f)) * 10);
                        if(ss_set_temp[0] & 0x80) ss_set_temp_buf += 5;
                        ss_rx_set_temp = ((uart3_rxd_buf[24] << 8) | (uart3_rxd_buf[25] & 0xff));
                        if(ss_set_temp_buf != ss_rx_set_temp)
                        {
                          ss_control_monitor[ss_air_address[0]] = 1;
                        }
                        if(ss_remocon_sensor_temp[ss_air_address[0]] != ((short)((uart3_rxd_buf[28] << 8) | (uart3_rxd_buf[29] & 0xff))))
                        {
                          ss_remocon_sensor_temp[ss_air_address[0]] = ((uart3_rxd_buf[28] << 8) | (uart3_rxd_buf[29] & 0xff));
                        }
                        if((uart3_rxd_buf[30] == 0x02) && (uart3_rxd_buf[31] == 0x02))
                        {
                          ss_error_code[ss_air_address[0]] = ((uart3_rxd_buf[32] << 8) | (uart3_rxd_buf[33] & 0xff));
                        }
                        
                        if((uart3_rxd_buf[34] == 0x42) && (uart3_rxd_buf[35] == 0x03))
                        {
                          if((room_temperatur_rev[0] != uart3_rxd_buf[36]) || (room_temperatur_rev[1] != uart3_rxd_buf[37]))
                          {
                              room_temperatur_rev[0] = uart3_rxd_buf[36];
                              room_temperatur_rev[1] = uart3_rxd_buf[37];
                          }
                        }
                      }
                    }
                    if(cb_mode == 1) //에어컨 2
                    {
                      if(f_ss_power_on[ss_air_address[1]])
                      {               
                        if(uart3_rxd_buf[15] != ss_onoff_status[1])
                        {
                          ss_control_monitor[ss_air_address[1]] = 1;
                        }
                        if(uart3_rxd_buf[18] != ss_control_mode[1])
                        {
                          ss_control_monitor[ss_air_address[1]] = 1;
                        }
                        if(uart3_rxd_buf[21] != ss_fan_speed[1])
                        {
                          ss_control_monitor[ss_air_address[1]] = 1;
                        }
                        ss_set_temp_buf = (((int)(ss_set_temp[1] & 0x7f)) * 10);
                        if(ss_set_temp[1] & 0x80) ss_set_temp_buf += 5;
                        ss_rx_set_temp = ((uart3_rxd_buf[24] << 8) | (uart3_rxd_buf[25] & 0xff));
                        if(ss_set_temp_buf != ss_rx_set_temp)
                        {
                          ss_control_monitor[ss_air_address[1]] = 1;
                        }
                        if(ss_remocon_sensor_temp[ss_air_address[1]] != ((short)((uart3_rxd_buf[28] << 8) | (uart3_rxd_buf[29] & 0xff))))
                        {
                          ss_remocon_sensor_temp[ss_air_address[1]] = ((uart3_rxd_buf[28] << 8) | (uart3_rxd_buf[29] & 0xff));
                        }
                        if((uart3_rxd_buf[30] == 0x02) && (uart3_rxd_buf[31] == 0x02))
                        {
                          ss_error_code[ss_air_address[1]] = ((uart3_rxd_buf[32] << 8) | (uart3_rxd_buf[33] & 0xff));
                        }
                        
                        if((uart3_rxd_buf[34] == 0x42) && (uart3_rxd_buf[35] == 0x03))
                        {
                          if((room_temperatur_rev[2] != uart3_rxd_buf[36]) || (room_temperatur_rev[3] != uart3_rxd_buf[37]))
                          {
                              room_temperatur_rev[2] = uart3_rxd_buf[36];
                              room_temperatur_rev[3] = uart3_rxd_buf[37];
                          }
                        }                      
                      }
                    }          
                  }
                }
                else if(uart3_rxd_buf[10] == 0x16)
                {
                  if(uart3_rxd_buf[5] == ss_air_address[0]) ss_control_monitor[ss_air_address[0]] = 0;
                  if(uart3_rxd_buf[5] == ss_air_address[1]) ss_control_monitor[ss_air_address[1]] = 0;
                }
              }
            }
          }
        }
    }
    else
    {
              f_plc_add_toggle[0] = 1; //air 1 수신 X flag set
              f_plc_add_toggle[1] = 1; //air 2 수신 X flag set
    }
  }
}
