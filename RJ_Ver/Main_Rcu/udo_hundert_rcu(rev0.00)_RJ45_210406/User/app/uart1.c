/*=====================(C) COPYRIGHT 2014 PLUS-H.=========================
program		    : CONTROL BOX(RCS-300-RCU)
processor 	  : STM32F103xx
compiler		  : IAR 6.2A Compiler 
program BY	  : H.H.Hwang
date			    : 2014.	 .
copy right	  : P L U S  -  H. 
============================================================================*/

#define __UART1_H__

#include "stm32f10x_lib.h"
#include "main.h"


//=======================================================================
u8 u1ctrl =7, u1txinx;
void uart1_frame_proc() {

  UART1_tx_check();
  
/*  
	switch(u1ctrl) {
		case 0:
			if(dtx.ldad !=dtx.svad) { 
				USART1->DR =dtx.buf[dtx.ldad] & (u16)0x01FF;
				u1ctrl++;
			}
			led.s.txd =0;
			watchdog.s.b5 =1;					// watchdog setup
			Extm.s.u1c =10;
			break;
			
		case 1:
			if(USART_GetFlagStatus(USART1, USART_FLAG_TC) !=RESET) {
				if(++dtx.ldad >=sizeof(dtx.buf)) dtx.ldad =0;
				if(dtx.ldad !=dtx.svad) {
					led.s.txd++;
					USART1->DR = (dtx.buf[dtx.ldad] & (u16)0x01FF);
					Extm.s.u1c =10;
				}
				else u1ctrl =0;
			}
			else if(!Extm.s.u1c) u1ctrl =7;			
			break;

// powerup start
		case 7:
			UART1_initial();						// UART1 initial
			hyper_clear();
			sprintf(xdebug, "%c[1;0H%c[1;%xm%c[1;%xm", 0x1b, 0x1b, cyan,  0x1b, _black); mm;

			if(zin.msel &0x80) __xcprintf("Uart1 Initial !!!\r\n");
			u1_init_f =1;
			
		default:		u1ctrl =0;		break;
 	}
*/  
}


/* UART1 data arrange routine */
void UART1_data_set(void)
{
/*  
  if((room_data_buf[99] & 0x07) == 0x07)  cb_data_send('R');
  else if((room_data_buf[99] & 0x07) == 0x03) cb_data_send('t');
  else if((room_data_buf[99] & 0x07) == 0x02) cb_data_send('T');
  else if((room_data_buf[99] & 0x07) == 0x04) cb_data_send('F');
  else cb_data_send('S');
*/
  
  if((room_data_buf[99] & 0x07) == 0x07)  cb_data_send('R');
  else if((room_data_buf[99] & 0x07) == 0x03) cb_data_send('t');
  else if(room_data_buf[99] & 0x02) cb_data_send('T');
  else if(room_data_buf[99] & 0x04) cb_data_send('F');
  else cb_data_send('S');
}

void cb_data_send(unsigned char send_cmd)
{
  unsigned char crc = 0;
  unsigned char i=0;
  unsigned char j;
  
  if(!f_uart1_data_send){
    P_UART1_DIR = 1;
    
    uart1_tx_data_buf[i++] = ' ';                                 //Preamble
    uart1_tx_data_buf[i++] = '{';                                 //STX
    ++i;                                                          //Length
    uart1_tx_data_buf[i++] = main_id_buf;                         //ID
    
    switch(send_cmd){
      case 'S':   //Standard data send
        uart1_tx_data_buf[i++] = 'S';                                         //Command
        uart1_tx_data_buf[i++] = cb_basic_data(0);                    //Basic data
        if(f_clean_finish)
        {
          f_clean_finish = 0;
          room_data_buf[49] = 2;
        }        
        uart1_tx_data_buf[i++] = room_data_buf[49];                   //Key status
        uart1_tx_data_buf[i++] = room_data_buf[64];                   //Sub device (light switch ��ġ ����)
        uart1_tx_data_buf[i++] = room_data_buf[65];                   //Sub device (light switch 0~7 ��� ����)
        uart1_tx_data_buf[i++] = room_data_buf[66];                   //Sub device (light switch 8~15 ��� ����)
        uart1_tx_data_buf[i++] = room_data_buf[67];                   //Sub device (k/s, c/i, nt{2} ��Ż��� �� ��ġ)   - 2020.11
        
        room_data_buf[59] = 0;
        room_data_buf[68] = 0;
        uart1_tx_data_buf[i++] = room_data_buf[59];                   // Inspector ���
        uart1_tx_data_buf[i++] = room_data_buf[68];                   // üũ�ƿ���û
        
        break;
        
      case 't':   //Standard data + Air temp data send ���µ� �۽� data
        room_data_buf[99] &= 0xfc;
        uart1_tx_data_buf[i++] = 't';                                 //Command
        uart1_tx_data_buf[i++] = cb_basic_data(0);                    //Basic data
        if(f_clean_finish)
        {
          f_clean_finish = 0;
          room_data_buf[49] = 2;
        }          
        uart1_tx_data_buf[i++] = room_data_buf[49];                   //Key status
        uart1_tx_data_buf[i++] = room_data_buf[88];                   //Sub device comm status
        uart1_tx_data_buf[i++] = room_data_buf[89];                   //Sub device comm status
        
        uart1_tx_data_buf[i++] = air_temp_control_status(0);          //Air temp control status
        uart1_tx_data_buf[i++] = air_temp_control_status(1);
        uart1_tx_data_buf[i++] = air_temp_control_status(2);
        uart1_tx_data_buf[i++] = air_temp_control_status(3);
        uart1_tx_data_buf[i++] = air_temp_control_status(4);
        uart1_tx_data_buf[i++] = air_temp_control_status(5);
        uart1_tx_data_buf[i++] = air_temp_control_status(6);
        uart1_tx_data_buf[i++] = air_temp_control_status(7);
        
        uart1_tx_data_buf[i++] = room_data_buf[0];                    //Air Set temp
        uart1_tx_data_buf[i++] = room_data_buf[1];
        uart1_tx_data_buf[i++] = room_data_buf[2];
        uart1_tx_data_buf[i++] = room_data_buf[3];
        uart1_tx_data_buf[i++] = room_data_buf[0];
        uart1_tx_data_buf[i++] = room_data_buf[1];
        uart1_tx_data_buf[i++] = room_data_buf[2];
        uart1_tx_data_buf[i++] = room_data_buf[3];
        
        uart1_tx_data_buf[i++] = room_data_buf[4];                    //Air Check temp
        uart1_tx_data_buf[i++] = room_data_buf[5];
        uart1_tx_data_buf[i++] = room_data_buf[6];
        uart1_tx_data_buf[i++] = room_data_buf[7];
        uart1_tx_data_buf[i++] = room_data_buf[4];
        uart1_tx_data_buf[i++] = room_data_buf[5];
        uart1_tx_data_buf[i++] = room_data_buf[6];
        uart1_tx_data_buf[i++] = room_data_buf[7];
        break;
        
      case 'T':   //Air temp data send
        room_data_buf[99] &= 0xfd;
        uart1_tx_data_buf[i++] = 'T';                                 //Command
        uart1_tx_data_buf[i++] = air_temp_control_status(0);          //Air temp control status
        uart1_tx_data_buf[i++] = air_temp_control_status(1);
        uart1_tx_data_buf[i++] = air_temp_control_status(2);
        uart1_tx_data_buf[i++] = air_temp_control_status(3);
        uart1_tx_data_buf[i++] = air_temp_control_status(4);
        uart1_tx_data_buf[i++] = air_temp_control_status(5);
        uart1_tx_data_buf[i++] = air_temp_control_status(6);
        uart1_tx_data_buf[i++] = air_temp_control_status(7);
        
        uart1_tx_data_buf[i++] = room_data_buf[0];                    //Air Set temp
        uart1_tx_data_buf[i++] = room_data_buf[1];
        uart1_tx_data_buf[i++] = room_data_buf[2];
        uart1_tx_data_buf[i++] = room_data_buf[3];
        uart1_tx_data_buf[i++] = room_data_buf[0];
        uart1_tx_data_buf[i++] = room_data_buf[1];
        uart1_tx_data_buf[i++] = room_data_buf[2];
        uart1_tx_data_buf[i++] = room_data_buf[3];
        
        uart1_tx_data_buf[i++] = room_data_buf[4];                    //Air Check temp
        uart1_tx_data_buf[i++] = room_data_buf[5];
        uart1_tx_data_buf[i++] = room_data_buf[6];
        uart1_tx_data_buf[i++] = room_data_buf[7];
        uart1_tx_data_buf[i++] = room_data_buf[4];
        uart1_tx_data_buf[i++] = room_data_buf[5];
        uart1_tx_data_buf[i++] = room_data_buf[6];
        uart1_tx_data_buf[i++] = room_data_buf[7];
        break;
        
      case 'F':   //Floor temp data send
        room_data_buf[99] &= 0xfb;
        uart1_tx_data_buf[i++] = 'F';                                 //Command
      uart1_tx_data_buf[i++] = floor_temp_control_status(0);          //floor temp control status
      uart1_tx_data_buf[i++] = floor_temp_control_status(1);
      uart1_tx_data_buf[i++] = floor_temp_control_status(2);
      uart1_tx_data_buf[i++] = floor_temp_control_status(3);
      uart1_tx_data_buf[i++] = floor_temp_control_status(4);
      uart1_tx_data_buf[i++] = floor_temp_control_status(5);
      uart1_tx_data_buf[i++] = floor_temp_control_status(6);
      uart1_tx_data_buf[i++] = floor_temp_control_status(7);
        
        uart1_tx_data_buf[i++] = room_data_buf[8];                    //Floor Set temp
        uart1_tx_data_buf[i++] = room_data_buf[9];
        uart1_tx_data_buf[i++] = room_data_buf[10];
        uart1_tx_data_buf[i++] = room_data_buf[11];  
        uart1_tx_data_buf[i++] = room_data_buf[8];
        uart1_tx_data_buf[i++] = room_data_buf[9];
        uart1_tx_data_buf[i++] = room_data_buf[10];
        uart1_tx_data_buf[i++] = room_data_buf[11];
        
        uart1_tx_data_buf[i++] = room_data_buf[12];                    //Floor Check temp
        uart1_tx_data_buf[i++] = room_data_buf[13];
        uart1_tx_data_buf[i++] = room_data_buf[14];
        uart1_tx_data_buf[i++] = room_data_buf[15];
        uart1_tx_data_buf[i++] = room_data_buf[12];
        uart1_tx_data_buf[i++] = room_data_buf[13];
        uart1_tx_data_buf[i++] = room_data_buf[14];
        uart1_tx_data_buf[i++] = room_data_buf[15];
        break;
        
      case 'R':
        room_data_buf[99] &= 0xfc;
        uart1_tx_data_buf[i++] = 't';                                 //Command
        uart1_tx_data_buf[i++] = cb_basic_data(0);                    //Basic data
        if(f_clean_finish)
        {
          f_clean_finish = 0;
          room_data_buf[49] = 2;
        }          
        uart1_tx_data_buf[i++] = room_data_buf[49];                   //Key status
        uart1_tx_data_buf[i++] = room_data_buf[88];                   //Sub device comm status
        uart1_tx_data_buf[i++] = room_data_buf[89];                   //Sub device comm status
        
        uart1_tx_data_buf[i++] = air_temp_control_status(0);          //Air temp control status ���µ��� ����
        uart1_tx_data_buf[i++] = air_temp_control_status(1);
        uart1_tx_data_buf[i++] = air_temp_control_status(2);
        uart1_tx_data_buf[i++] = air_temp_control_status(3);
        uart1_tx_data_buf[i++] = air_temp_control_status(4);
        uart1_tx_data_buf[i++] = air_temp_control_status(5);
        uart1_tx_data_buf[i++] = air_temp_control_status(6);
        uart1_tx_data_buf[i++] = air_temp_control_status(7);
        
        uart1_tx_data_buf[i++] = room_data_buf[0];                    //Air Set temp
        uart1_tx_data_buf[i++] = room_data_buf[1];
        uart1_tx_data_buf[i++] = room_data_buf[2];
        uart1_tx_data_buf[i++] = room_data_buf[3];
        uart1_tx_data_buf[i++] = room_data_buf[0];
        uart1_tx_data_buf[i++] = room_data_buf[1];
        uart1_tx_data_buf[i++] = room_data_buf[2];
        uart1_tx_data_buf[i++] = room_data_buf[3];
        
        uart1_tx_data_buf[i++] = room_data_buf[4];                    //Air Check temp
        uart1_tx_data_buf[i++] = room_data_buf[5];
        uart1_tx_data_buf[i++] = room_data_buf[6];
        uart1_tx_data_buf[i++] = room_data_buf[7];
        uart1_tx_data_buf[i++] = room_data_buf[4];
        uart1_tx_data_buf[i++] = room_data_buf[5];
        uart1_tx_data_buf[i++] = room_data_buf[6];
        uart1_tx_data_buf[i++] = room_data_buf[7];
        
        uart1_tx_data_buf[i++] = room_data_buf[8];                    //Floor Set temp
        uart1_tx_data_buf[i++] = room_data_buf[9];
        uart1_tx_data_buf[i++] = room_data_buf[10];
        uart1_tx_data_buf[i++] = room_data_buf[11];
        uart1_tx_data_buf[i++] = room_data_buf[8];
        uart1_tx_data_buf[i++] = room_data_buf[9];
        uart1_tx_data_buf[i++] = room_data_buf[10];
        uart1_tx_data_buf[i++] = room_data_buf[11];
        
        uart1_tx_data_buf[i++] = room_data_buf[12];                    //Floor Check temp
        uart1_tx_data_buf[i++] = room_data_buf[13];
        uart1_tx_data_buf[i++] = room_data_buf[14];
        uart1_tx_data_buf[i++] = room_data_buf[15];
        uart1_tx_data_buf[i++] = room_data_buf[12];
        uart1_tx_data_buf[i++] = room_data_buf[13];
        uart1_tx_data_buf[i++] = room_data_buf[14];
        uart1_tx_data_buf[i++] = room_data_buf[15];
        
        break;
    }

    uart1_tx_data_buf[2] = i+1;
    for(j=1; j<i; ++j){
      crc += uart1_tx_data_buf[j];
    }
    crc ^= 0x55;
    uart1_tx_data_buf[i++] = crc;                             //CRC
    uart1_tx_data_buf[i++] = '}';                             //ETX
    
    
    for(j=0; j<i; ++j){
      uart1_tx_send_buf[j] = uart1_tx_data_buf[j];
    }
    
    f_uart1_data_send = 1;
    uart1_tx_length = 0;
    uart1_tx_backup = i;  
    
    uart1_delay_dir = 0;
  }
}
unsigned char air_temp_control_status(unsigned char index)
{
  unsigned char return_data = 0;

  return_data = room_data_buf[16+index] & 0x0f;             //Speed & on/off status
  if(room_data_buf[20+index] & 0x04) return_data |= 0x10;   //�ó�����
  if(room_data_buf[24+index] & 0x80) return_data |= 0x80;                                            //RCU-> DTC �µ����� ������� �۽� (20.11.18)
  
  return_data &= 0x9f;  
  return_data |= ((room_data_buf[20+index] & 0x03) << 5);   //�������
  return(return_data);
}

unsigned char floor_temp_control_status(unsigned char index)
{
  unsigned char return_data = 0;
  
  if(room_data_buf[16+index] & 0x10) return_data |= 0x01;   //valve ����              
  if(room_data_buf[16+index] & 0x20) return_data |= 0x04;   //�ٴ� switch on/off status
  if(room_data_buf[16+index] & 0x40) return_data |= 0x08;   //�ٴڼ��� ��� ��/��
  if(room_data_buf[24+index] & 0x80) return_data |= 0x80;                                            //RCU-> DTC �µ����� �������  �۽� (20.11.18)
  
  return_data |= ((room_data_buf[20+index] & 0x30) << 1);   //�������
  
  return(return_data);
}

unsigned char cb_basic_data(unsigned char index)
{
  unsigned char return_data = 0;
  
  if(room_data_buf[index+60] & 0x01) return_data |= 0x01;       //�ѽ�(1) or ���
  if(room_data_buf[42] & 0x01) return_data |= 0x02;  //������
  if(room_data_buf[42] & 0xf0)  return_data |= 0x04;  //�̻�µ�
  if(room_data_buf[40] & 0x04)  return_data |= 0x08;            //DND ?
  if(room_data_buf[40] & 0x08)  return_data |= 0x10;            //MUR ?
  if(room_data_buf[40] & 0x40)  return_data |= 0x20;            //Door ?
  if(room_data_buf[40] & 0x10)  return_data |= 0x40;            //Windows open ?
  if(room_data_buf[41] & 0x01)  return_data |= 0x80;            //����/���� ���� ����
  return(return_data);
}

/* UART1 data send check routine */
void UART1_tx_check(void)
{
  if(f_uart1_dir_start){
	  if(USART_GetFlagStatus(USART1, USART_FLAG_TC) !=RESET) {
      if(uart1_tx_length < uart1_tx_backup){
        USART1->DR = (uart1_tx_send_buf[uart1_tx_length++] & (u16)0x01FF);
      }
      else{
        P_UART1_DIR = 0;
        f_uart1_data_send = 0;
        f_uart1_dir_start = 0;
      }
    }
  }
}

//=======================================================================
/* UART1_initial	:								              */												
//=======================================================================
void UART1_initial(void){
GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

  /* RS485 UART1 Direction port define */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


  /* Enable GPIOA and USART1 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
  
  /* Configure USART1 Tx (PA9) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART1 Rx (PA10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

/* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
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
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//  USART_InitStructure.USART_Clock = USART_Clock_Disable;
//  USART_InitStructure.USART_CPOL = USART_CPOL_Low;
//  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
//  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
    
  /* Configure the USART1 */
  USART_Init(USART1, &USART_InitStructure);

/* Enable the USART Transmoit interrupt: this interrupt is generated when the 
   USART1 transmit data register is empty */  
//  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

/* Enable the USART Receive interrupt: this interrupt is generated when the 
   USART1 receive data register is not empty */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
  
  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//=======================================================================

void UART1_control_proc(void) {
//u8 ix; 
  unsigned char crc_buf = 0;
  unsigned char i;

  if(f_uart1_frame_rx_ok){
    f_uart1_frame_rx_ok = 0;
    for(i=0; i<uart1_rxd_buf[1]-2; ++i) crc_buf += uart1_rxd_buf[i];
    crc_buf ^= 0x55;
    if(crc_buf == (uart1_rxd_buf[uart1_rxd_buf[1]-2])){
      if(main_id_buf == uart1_rxd_buf[2]){
        main_rxd_check();
        led2 ^= 1;
        UART1_data_set();
      }
      else{
        if(uart1_rxd_buf[2] == 0xff){
          main_rxd_check();
        }
      }

    }
  }
	uart1_frame_proc();
}


void main_rxd_check(void)
{
  unsigned char temp_index;
  
  if(uart1_rxd_buf[3] != 'G'){
    led1 ^= 1;
  }
  switch(uart1_rxd_buf[3]){
    case 'G':   //Basic data & Group o
      
      if((uart1_rxd_buf[4] & 0x01) != (room_data_buf[40] & 0x01)){
        if((room_data_buf[40] & 0x02) == 0){
          room_data_buf[16] |= 0x24;
          room_data_buf[17] |= 0x24;
          room_data_buf[18] |= 0x24;
          room_data_buf[19] |= 0x24;
          
          room_data_buf[40] &= 0xfb;  //DND clear
          
          room_data_buf[83] = 0;      //h@20150616
          
          if((room_data_buf[85] != 0 ) && (room_data_buf[85] != 0xff)){   //���ؿµ� ��� //h@20140711
            ts_air_set_temp_from_PC[0] = room_data_buf[85];
            ts_air_set_temp_from_PC[1] = room_data_buf[85];
            ts_air_set_temp_from_PC[2] = room_data_buf[85];
            ts_air_set_temp_from_PC[3] = room_data_buf[85];
          }
          
          if((room_data_buf[86] != 0 ) && (room_data_buf[86] != 0xff)){   //���ؿµ� �ٴ� //h@20140711
            ts_floor_set_temp_from_PC[0] = room_data_buf[86];
            ts_floor_set_temp_from_PC[1] = room_data_buf[86];
            ts_floor_set_temp_from_PC[2] = room_data_buf[86];
            ts_floor_set_temp_from_PC[3] = room_data_buf[86];
          }
          
        }
        
          
          room_data_buf[54] = 0;  //TS1 AUTO MODE ��ȯ
          room_data_buf[55] = 0;  //TS2 AUTO MODE ��ȯ
          room_data_buf[56] = 0;  //TS3 AUTO MODE ��ȯ
          room_data_buf[57] = 0;  //TS4 AUTO MODE ��ȯ              ///////////////  üũ�� , üũ�ƿ��ϋ� ������ ��ȯ       
      }
      
            
      if(uart1_rxd_buf[4] & 0x01) room_data_buf[40] |= 0x01;  //check IN
      else room_data_buf[40] &= 0xfe;                         //check OUT
      if(uart1_rxd_buf[4] & 0x02) room_data_buf[40] |= 0x20;  //Message ON
      else room_data_buf[40] &= 0xdf;                         //Message OFF
      if(uart1_rxd_buf[4] & 0x04) room_data_buf[41] |= 0x02;  //�ɾ����� ON
      else room_data_buf[41] &= 0xfd;                         //�ɾ����� OFF
      if(uart1_rxd_buf[4] & 0x08) room_data_buf[41] |= 0x01;  //����.���� ON
      else room_data_buf[41] &= 0xfe;                         //����.���� OFF
      room_data_buf[74] = ((uart1_rxd_buf[4] & 0x70) >> 4);   //��.��.ȯ���� ���

      if(uart1_rxd_buf[4] & 0x80) f_air_set_temp_kind = 1;    //�����µ� �ִ�,�ּ� ����   //h@20141215
      else f_air_set_temp_kind = 0;                           //�µ����� �����µ� ����    //h@20141215

      main_comm_rx_ok = 1;

      if(room_data_buf[74] == 0)
      {
        room_data_buf[20] &= 0xfb;    //����
        room_data_buf[21] &= 0xfb;    //����
        room_data_buf[22] &= 0xfb;    //����
        room_data_buf[23] &= 0xfb;    //����
      }
      else if(room_data_buf[74] == 1)
      {
        room_data_buf[20] |= 0x04;    //�ù�
        room_data_buf[21] |= 0x04;    //�ù�
        room_data_buf[22] |= 0x04;    //�ù�
        room_data_buf[23] |= 0x04;    //�ù�
      }
      
      if((uart1_rxd_buf[5] <= 99) || (uart1_rxd_buf[5] == 0xff)){
        if(uart1_rxd_buf[5] == 0) room_data_buf[81] = 0xff;
        else room_data_buf[81] = uart1_rxd_buf[5];
      }
      
      if(uart1_rxd_buf[6] & 0x01) f_bath_control = 0; //ù ��ǽ�, ȭ��� ���� OFF
      else f_bath_control = 1;                                //ù ��ǽ�, ȭ��� ���� ON
      
      if(uart1_rxd_buf[6] & 0x02) led_bright_control = 1; //����  - �ⱸ�� LED �ĸ�� 
      else led_bright_control = 0;                                //��Ӱ� 
      
      //if(uart1_rxd_buf[6] & 0x04) aircon_company = 1; //LG 
      //else aircon_company = 0;                                //�Ｚ PLC
      
      //if(uart1_rxd_buf[6] & 0x08) hotel_dormitory = 1;  //���ǰ���
      //else hotel_dormitory = 0;

      
      break;
    
    
    
    case 'C':   //Basic data
      if(uart1_rxd_buf[4] & 0x01) room_data_buf[40] |= 0x01;  //check IN
      else room_data_buf[40] &= 0xfe;                         //check OUT
      if(uart1_rxd_buf[4] & 0x02) room_data_buf[40] |= 0x20;  //Message ON
      else room_data_buf[40] &= 0xdf;                         //Message OFF
      if(uart1_rxd_buf[4] & 0x04) room_data_buf[41] |= 0x02;  //�ɾ����� ON
      else room_data_buf[41] &= 0xfd;                         //�ɾ����� OFF
      if(uart1_rxd_buf[4] & 0x08) room_data_buf[41] |= 0x01;  //����.���� ON
      else room_data_buf[41] &= 0xfe;                         //����.���� OFF
      room_data_buf[74] = ((uart1_rxd_buf[4] & 0x70) >> 4);   //��.��.ȯ���� ���
      main_comm_rx_ok = 1;

      if(room_data_buf[74] == 0){
        room_data_buf[20] &= 0xfb;    //����
        room_data_buf[21] &= 0xfb;    //����
        room_data_buf[22] &= 0xfb;    //����
        room_data_buf[23] &= 0xfb;    //����
      }
      else if(room_data_buf[74] == 1){
        room_data_buf[20] |= 0x04;    //�ù�
        room_data_buf[21] |= 0x04;    //�ù�
        room_data_buf[22] |= 0x04;    //�ù�
        room_data_buf[23] |= 0x04;    //�ù�
      }
//      else if(room_data_buf[74] == 2){  //ȯ���� ���
//      }
      break;
      
    case 'T':   //��� �����µ� ���� ����
      temp_index = 5;
      if(uart1_rxd_buf[4] & 0x01){
        ts_air_set_temp_from_PC[0] = uart1_rxd_buf[temp_index++];
        ts_set_temp_send_count[0] = 0;
      }
      if(uart1_rxd_buf[4] & 0x02){
        ts_air_set_temp_from_PC[1] = uart1_rxd_buf[temp_index++];
        ts_set_temp_send_count[1] = 0;
      }
      if(uart1_rxd_buf[4] & 0x04){
        ts_air_set_temp_from_PC[2] = uart1_rxd_buf[temp_index++];
        ts_set_temp_send_count[2] = 0;
      }
      if(uart1_rxd_buf[4] & 0x08){
        ts_air_set_temp_from_PC[3] = uart1_rxd_buf[temp_index++];
        ts_set_temp_send_count[3] = 0;
      }
      break;
    case 't':   //��� �����µ� ��ü ����
       ts_air_set_temp_from_PC[0] = uart1_rxd_buf[5];
       ts_air_set_temp_from_PC[1] = uart1_rxd_buf[6];
       ts_air_set_temp_from_PC[2] = uart1_rxd_buf[7];
       ts_air_set_temp_from_PC[3] = uart1_rxd_buf[8];
       ts_set_temp_send_count[0] = 0;
       ts_set_temp_send_count[1] = 0;
       ts_set_temp_send_count[2] = 0;
       ts_set_temp_send_count[3] = 0;
      break;
    case 'F':   //�ٴ� �����µ� ���� ����
      temp_index = 5;
      if(uart1_rxd_buf[4] & 0x01){
        ts_floor_set_temp_from_PC[0] = uart1_rxd_buf[temp_index++];
      }
      if(uart1_rxd_buf[4] & 0x02){
        ts_floor_set_temp_from_PC[1] = uart1_rxd_buf[temp_index++];
      }
      if(uart1_rxd_buf[4] & 0x04){
        ts_floor_set_temp_from_PC[2] = uart1_rxd_buf[temp_index++];
      }
      if(uart1_rxd_buf[4] & 0x08){
        ts_floor_set_temp_from_PC[3] = uart1_rxd_buf[temp_index++];
      }
      break;
    case 'f':   //�ٴ� �����µ� ��ü ����
       ts_floor_set_temp_from_PC[0] = uart1_rxd_buf[5];
       ts_floor_set_temp_from_PC[1] = uart1_rxd_buf[6];
       ts_floor_set_temp_from_PC[2] = uart1_rxd_buf[7];
       ts_floor_set_temp_from_PC[3] = uart1_rxd_buf[8];
       ts_set_temp_send_count[0] = 0;
       ts_set_temp_send_count[1] = 0;
       ts_set_temp_send_count[2] = 0;
       ts_set_temp_send_count[3] = 0;       
      break;
    case 'O':   //�µ����� ON/OFF ���� ����
      if(uart1_rxd_buf[4] & 0x01){
        if(uart1_rxd_buf[5] & 0x01) room_data_buf[16] |= 0x04;  
        else room_data_buf[16] &= 0xfb;  
        room_data_buf[16] |= 0x80;  room_data_buf[54] &= 0xf8;
      }
      if(uart1_rxd_buf[4] & 0x02){
        if(uart1_rxd_buf[5] & 0x02) room_data_buf[17] |= 0x04;  
        else room_data_buf[17] &= 0xfb;  
        room_data_buf[17] |= 0x80;  room_data_buf[55] &= 0xf8;
      }
      if(uart1_rxd_buf[4] & 0x04){
        if(uart1_rxd_buf[5] & 0x04) room_data_buf[18] |= 0x04;  
        else room_data_buf[18] &= 0xfb;  
        room_data_buf[18] |= 0x80;  room_data_buf[56] &= 0xf8;
      }
      if(uart1_rxd_buf[4] & 0x08){
        if(uart1_rxd_buf[5] & 0x08) room_data_buf[19] |= 0x04;  
        else room_data_buf[19] &= 0xfb;  
        room_data_buf[19] |= 0x80;  room_data_buf[57] &= 0xf8;
      }
      
      if(uart1_rxd_buf[6] & 0x01){
        if(uart1_rxd_buf[7] & 0x01) room_data_buf[16] |= 0x20;  
        else room_data_buf[16] &= 0xdf;  
        room_data_buf[20] |= 0x80;
      }
      if(uart1_rxd_buf[6] & 0x02){
        if(uart1_rxd_buf[7] & 0x02) room_data_buf[17] |= 0x20;  
        else room_data_buf[17] &= 0xdf;  
        room_data_buf[21] |= 0x80;
      }
      if(uart1_rxd_buf[6] & 0x04){
        if(uart1_rxd_buf[7] & 0x04) room_data_buf[18] |= 0x20;  
        else room_data_buf[18] &= 0xdf;  
        room_data_buf[22] |= 0x80;
      }
      if(uart1_rxd_buf[6] & 0x08){
        if(uart1_rxd_buf[7] & 0x08) room_data_buf[19] |= 0x20;  
        else room_data_buf[19] &= 0xdf;  
        room_data_buf[23] |= 0x80;
      }      
      
      room_data_buf[99] |= 0x02;
      room_data_buf[99] |= 0x04;
      
      break;
      
    case 'P':   //���� Data


      room_data_buf[70] = (uart1_rxd_buf[4] & 0x0f) * 10; //����µ���
      room_data_buf[75] = (uart1_rxd_buf[4] & 0xf0) >> 4; //��.�� ��ȯ�µ���
      room_data_buf[76] = uart1_rxd_buf[5];               //�̻�溸�µ�
      if(room_data_buf[81] == 0xff){
        if(uart1_rxd_buf[6] != 0xff) room_data_buf[71] = uart1_rxd_buf[6];               //��Ⳮ�� ���Ǳ��ؿµ�
        if(uart1_rxd_buf[7] != 0xff) room_data_buf[72] = uart1_rxd_buf[7];               //���ù� ���Ǳ��ؿµ�
        if(uart1_rxd_buf[8] != 0xff) room_data_buf[73] = uart1_rxd_buf[8];               //�ٴ� ���Ǳ��ؿµ�
      }
      room_data_buf[28] = uart1_rxd_buf[9];               //���µ����� ��.���ѿµ�
      room_data_buf[29] = uart1_rxd_buf[10];              //�ٴڿµ����� ��.���ѿµ�
      if(uart1_rxd_buf[11] <= 0x99) room_data_buf[90] = uart1_rxd_buf[11];              //��
      if(uart1_rxd_buf[12] <= 0x12) room_data_buf[91] = uart1_rxd_buf[12];              //��
      if(uart1_rxd_buf[13] <= 0x31) room_data_buf[92] = uart1_rxd_buf[13];              //��
      if(uart1_rxd_buf[14] <= 0x23) room_data_buf[93] = uart1_rxd_buf[14];              //��
      if(uart1_rxd_buf[15] <= 0x59) room_data_buf[94] = uart1_rxd_buf[15];              //��
      room_data_buf[77] = uart1_rxd_buf[16];              //�Ա��� �����ð� H
      room_data_buf[78] = uart1_rxd_buf[17];              //�Ա��� �����ð� L
      entry_lamp_delay_time = (room_data_buf[77] * 0x100) + room_data_buf[78];
      if((uart1_rxd_buf[18] != 0) && (uart1_rxd_buf[18] != 0xff)){
        room_data_buf[82] &= 0x08;
        room_data_buf[82] |= (uart1_rxd_buf[18] & 0xf7);            //���� ������
      }
      if((uart1_rxd_buf[18] == '0') || (uart1_rxd_buf[18] == '1')){   //�µ����� �µ�ǥ�ù��
        if(uart1_rxd_buf[18] == '0')  room_data_buf[41] &= 0xf7;  //����µ�ǥ��
        else room_data_buf[41] |= 0x08;  //�����µ�ǥ��
      }
      
      if((uart1_rxd_buf[20] != 0) && (uart1_rxd_buf[20] != 0xff)){
        room_data_buf[85] = b_decimal_to_hex_table[uart1_rxd_buf[20]];    //���ؿµ� ���   //h@20140711
      }
      if((uart1_rxd_buf[21] != 0) && (uart1_rxd_buf[21] != 0xff)){
        room_data_buf[86] = b_decimal_to_hex_table[uart1_rxd_buf[21]];    //���ؿµ� ���   //h@20140711
      }
      
      break;
      
    case 'I':   //Chime & alram volume
      room_data_buf[79] = uart1_rxd_buf[4];
      room_data_buf[80] = uart1_rxd_buf[5];
      break;
      
    case 'i':   //group empty temp
      if(uart1_rxd_buf[4] == room_data_buf[81]){
        if(uart1_rxd_buf[5] != 0xff) room_data_buf[72] = uart1_rxd_buf[5];               //���ù� ���Ǳ��ؿµ�
        if(uart1_rxd_buf[6] != 0xff) room_data_buf[71] = uart1_rxd_buf[6];               //��Ⳮ�� ���Ǳ��ؿµ�
        if(uart1_rxd_buf[7] != 0xff) room_data_buf[73] = uart1_rxd_buf[7];               //�ٴ� ���Ǳ��ؿµ�
      }
      break;
   
    case '$':   //�����µ�                                                //h@20140704 =>
      if((uart1_rxd_buf[4] & 0x0f) <= 0x04){
        if(uart1_rxd_buf[5] != 0xff){
          bojung_temp[uart1_rxd_buf[4] & 0x0f] = uart1_rxd_buf[5];
        }
      }
      break;                                                              //<= h@20140704
      
    case 'R':   //All data ����
      room_data_buf[99] = 0x07;
      break;
      
      
//
//************** K Hotel(Domyin) Only ***********
//
    case 'l':   //õ���� Indicator sign status
      room_data_buf[100] = uart1_rxd_buf[4];
      break;
      
    case 'm':   //ȥ������
      break;
      
    case 'n':   //DND On-line control
      if(uart1_rxd_buf[4] == '1') dnd_mur_execution('D','N');
      else if(uart1_rxd_buf[4] == '0') dnd_mur_execution('N','F');
      break;
      
    case 'o':   //MUR On-line control
      if(uart1_rxd_buf[4] == '1') dnd_mur_execution('M','N');
      else if(uart1_rxd_buf[4] == '0') dnd_mur_execution('N','F');
      break;
      
    case 'p':   //TOWEL Change On-line control
      if(uart1_rxd_buf[4] == '1') dnd_mur_execution('T','N');
      else if(uart1_rxd_buf[4] == '0') dnd_mur_execution('N','F');
      break;
      
  }
}

//=======================================================================
//=======================================================================

