/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/

#define __UART3_C__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bit_define.h"

#include "stm32f2xx_init.h"
#include "stm32f2xx_it.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "main.h"

//=======================================================================
u8 u3ctrl =7, u3txinx;
void uart3_frame_proc(void)
{
  unsigned int temp_id;
  unsigned int temp_ttt;  
  
  if(f_uart3_send_time){
    f_uart3_send_time = 0;
    
    f_sign_sys_tx = 1;
    sign_led_sys_tx = 0;
    
    if(++device_count >= b_device_count) device_count = 0;
    
    temp_id = cb_first_send_check();
    if(!f_power_on){
      cb_power_on_send();
      f_power_on = 1;
    }
    else{
      if(temp_id == 0x9999) cb_suncha_send();
      else{
        P_u3dir = 1;
        send_point_buf = 0;
        uart3_tx_data_buf[send_point_buf++] = ' ';                                 //Preamble
        uart3_tx_data_buf[send_point_buf++] = '(';                                 //STX
        ++send_point_buf;                                                          //Length
        
        if(temp_id == 0xffff){
          uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
          uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
          uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
          uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
          f_uart3_common_data = 1;                                                  //��ü Data ����
        }
        else{
          if(temp_id < 0xff00){
            temp_ttt = hex2dec(temp_id);
          }
          else{
            temp_ttt = temp_id;
          }
          uart3_tx_data_buf[send_point_buf++] = ((temp_ttt & 0xf000) >> 12) | 0x30;    //ID
          uart3_tx_data_buf[send_point_buf++] = ((temp_ttt & 0x0f00) >> 8) | 0x30;    //ID
          uart3_tx_data_buf[send_point_buf++] = ((temp_ttt & 0x00f0) >> 4) | 0x30;    //ID
          uart3_tx_data_buf[send_point_buf++] = ((temp_ttt & 0x000f) >> 0) | 0x30;    //ID
          f_uart3_common_data = 0;                                                   //���� data ����
        }        
        
        uart3_tx_data_buf[send_point_buf++] = ((device_count & 0xf0) >> 4) | 0x30; //Device No
        uart3_tx_data_buf[send_point_buf++] = (device_count & 0x0f) | 0x30;        //Device No
        cb_command_send(temp_id,cb_first_save_buffer[cb_first_check_point][1]);
        uart3_tx_data_buf[2] = send_point_buf+1;
        cb_send_crc_cal();
      }
    }
 }
  UART3_tx_check();
  UART3_rxd_check();

}

void cb_suncha_send(void)
{
  
  P_u3dir = 1;
  send_point_buf = 0;
  uart3_tx_data_buf[send_point_buf++] = ' ';                                 //Preamble
  uart3_tx_data_buf[send_point_buf++] = '(';                                 //STX
  ++send_point_buf;                                                          //Length
  uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
  uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
  uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
  uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
  uart3_tx_data_buf[send_point_buf++] = ((device_count & 0xf0) >> 4) | 0x30; //Device No
  uart3_tx_data_buf[send_point_buf++] = (device_count & 0x0f) | 0x30;        //Device No
  uart3_tx_data_buf[send_point_buf++] = '#';                                 //Fip Call command
  uart3_tx_data_buf[2] = send_point_buf+1;
  cb_send_crc_cal();
  
  
}

void cb_power_on_send(void)
{
  
  P_u3dir = 1;
  send_point_buf = 0;
  uart3_tx_data_buf[send_point_buf++] = ' ';                                 //Preamble
  uart3_tx_data_buf[send_point_buf++] = '(';                                 //STX
  ++send_point_buf;                                                          //Length
  uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
  uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
  uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
  uart3_tx_data_buf[send_point_buf++] = '?';                                 //ID
  uart3_tx_data_buf[send_point_buf++] = ((device_count & 0xf0) >> 4) | 0x30; //Device No
  uart3_tx_data_buf[send_point_buf++] = (device_count & 0x0f) | 0x30;        //Device No
  uart3_tx_data_buf[send_point_buf++] = 'Z';                                 //Command
  uart3_tx_data_buf[send_point_buf++] = 'O';                                 //Command data   power on 
  uart3_tx_data_buf[send_point_buf++] = '#';                                 //Fip Call command
  uart3_tx_data_buf[2] = send_point_buf+1;
  cb_send_crc_cal();
}

void cb_command_send(unsigned int index, unsigned char send_cmd)
{
//  unsigned char i=0;
//  unsigned char j;
//  unsigned char crc = 0;
  unsigned char group_index;
  
  switch(send_cmd){
    case 'A':   //Check IN/OUT
       uart3_tx_data_buf[send_point_buf++] = 'A'; 
       uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][2]; 
       uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][31]; 
       break;
      
    case 'B':   //Message ON/OFF
       uart3_tx_data_buf[send_point_buf++] = 'B'; 
       uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][3]; 
       break;
      
    case 'C':   //����.���� ON/OFF
       uart3_tx_data_buf[send_point_buf++] = 'C'; 
       uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][4]; 
       break;
      
    case 'D':   //�����µ����
      uart3_tx_data_buf[send_point_buf++] = 'D'; 
      if(cb_send_data_buf[index][6] == 0xff){
        uart3_tx_data_buf[send_point_buf++] = '?'; //��ü �ش�
        uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][7] & 0xf0) >> 4) | 0x30; 
        uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][7] & 0x0f) | 0x30; 
        cb_send_data_buf[index][6] = 0x00;
      }
      else{
        if(cb_send_data_buf[index][6] & 0x01){
          cb_send_data_buf[index][6] &= 0xfe;
          uart3_tx_data_buf[send_point_buf++] = '0'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][7] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][7] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][6] & 0x02){
          cb_send_data_buf[index][6] &= 0xfd;
          uart3_tx_data_buf[send_point_buf++] = '1'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][8] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][8] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][6] & 0x04){
          cb_send_data_buf[index][6] &= 0xfb;
          uart3_tx_data_buf[send_point_buf++] = '2'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][9] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][9] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][6] & 0x08){
          cb_send_data_buf[index][6] &= 0xf7;
          uart3_tx_data_buf[send_point_buf++] = '3'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][10] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][10] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][6] & 0x10){
          cb_send_data_buf[index][6] &= 0xef;
          uart3_tx_data_buf[send_point_buf++] = '4'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][11] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][11] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][6] & 0x20){
          cb_send_data_buf[index][6] &= 0xdf;
          uart3_tx_data_buf[send_point_buf++] = '5'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][12] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][12] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][6] & 0x40){
          cb_send_data_buf[index][6] &= 0xbf;
          uart3_tx_data_buf[send_point_buf++] = '6'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][13] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][13] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][6] & 0x80){
          cb_send_data_buf[index][6] &= 0x7f;
          uart3_tx_data_buf[send_point_buf++] = '7'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][14] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][14] & 0x0f) | 0x30; 
        }
      }
      break;
      
    case 'E':   //�����µ��ٴ�
      uart3_tx_data_buf[send_point_buf++] = 'E'; 
      if(cb_send_data_buf[index][15] == 0xff){
        uart3_tx_data_buf[send_point_buf++] = '?'; //��ü �ش�
        uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][16] & 0xf0) >> 4) | 0x30; 
        uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][16] & 0x0f) | 0x30; 
        cb_send_data_buf[index][15] = 0x00;
      }
      else{
        if(cb_send_data_buf[index][15] & 0x01){
          cb_send_data_buf[index][15] &= 0xfe;
          uart3_tx_data_buf[send_point_buf++] = '0'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][16] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][16] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][15] & 0x02){
          cb_send_data_buf[index][15] &= 0xfd;
          uart3_tx_data_buf[send_point_buf++] = '1'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][17] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][17] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][15] & 0x04){
          cb_send_data_buf[index][15] &= 0xfb;
          uart3_tx_data_buf[send_point_buf++] = '2'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][18] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][18] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][15] & 0x08){
          cb_send_data_buf[index][15] &= 0xf7;
          uart3_tx_data_buf[send_point_buf++] = '3'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][19] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][19] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][15] & 0x10){
          cb_send_data_buf[index][15] &= 0xef;
          uart3_tx_data_buf[send_point_buf++] = '4'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][20] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][20] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][15] & 0x20){
          cb_send_data_buf[index][15] &= 0xdf;
          uart3_tx_data_buf[send_point_buf++] = '5'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][21] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][21] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][15] & 0x40){
          cb_send_data_buf[index][15] &= 0xbf;
          uart3_tx_data_buf[send_point_buf++] = '6'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][22] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][22] & 0x0f) | 0x30; 
        }
        else if(cb_send_data_buf[index][15] & 0x80){
          cb_send_data_buf[index][15] &= 0x7f;
          uart3_tx_data_buf[send_point_buf++] = '7'; //���� ����
          uart3_tx_data_buf[send_point_buf++] = ((cb_send_data_buf[index][23] & 0xf0) >> 4) | 0x30; 
          uart3_tx_data_buf[send_point_buf++] = (cb_send_data_buf[index][23] & 0x0f) | 0x30; 
        }
      }
      break;
  
    case 'F':   //û������/�Ϸ�
       uart3_tx_data_buf[send_point_buf++] = 'F'; 
       uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][28]; 
       break;
    
    case 'G':   //���µ����� ON/OFF
      uart3_tx_data_buf[send_point_buf++] = 'G'; 
      if(cb_send_data_buf[index][24] == 0xff){
        uart3_tx_data_buf[send_point_buf++] = '?'; //��ü �ش�
        if(cb_send_data_buf[index][25] == 0xff)  uart3_tx_data_buf[send_point_buf++] = '1';
        else uart3_tx_data_buf[send_point_buf++] = '0';
        cb_send_data_buf[index][24] = 0x00;
      }
      else{
        if(cb_send_data_buf[index][24] & 0x01){
          cb_send_data_buf[index][24] &= 0xfe;
          uart3_tx_data_buf[send_point_buf++] = '0';
          if(cb_send_data_buf[index][25] & 0x01)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][24] & 0x02){
          cb_send_data_buf[index][24] &= 0xfd;
          uart3_tx_data_buf[send_point_buf++] = '1';
          if(cb_send_data_buf[index][25] & 0x02)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][24] & 0x04){
          cb_send_data_buf[index][24] &= 0xfb;
          uart3_tx_data_buf[send_point_buf++] = '2';
          if(cb_send_data_buf[index][25] & 0x04)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][24] & 0x08){
          cb_send_data_buf[index][24] &= 0xf7;
          uart3_tx_data_buf[send_point_buf++] = '3';
          if(cb_send_data_buf[index][25] & 0x08)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][24] & 0x10){
          cb_send_data_buf[index][24] &= 0xef;
          uart3_tx_data_buf[send_point_buf++] = '4';
          if(cb_send_data_buf[index][25] & 0x10)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][24] & 0x20){
          cb_send_data_buf[index][24] &= 0xdf;
          uart3_tx_data_buf[send_point_buf++] = '5';
          if(cb_send_data_buf[index][25] & 0x20)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][24] & 0x40){
          cb_send_data_buf[index][24] &= 0xbf;
          uart3_tx_data_buf[send_point_buf++] = '6';
          if(cb_send_data_buf[index][25] & 0x40)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][24] & 0x80){
          cb_send_data_buf[index][24] &= 0x7f;
          uart3_tx_data_buf[send_point_buf++] = '7';
          if(cb_send_data_buf[index][25] & 0x80)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
      }
      break;
  
    case 'H':   //�ٴڿµ����� ON/OFF
      uart3_tx_data_buf[send_point_buf++] = 'H'; 
      if(cb_send_data_buf[index][26] == 0xff){
        uart3_tx_data_buf[send_point_buf++] = '?'; //��ü �ش�
        if(cb_send_data_buf[index][27] == 0xff)  uart3_tx_data_buf[send_point_buf++] = '1';
        else uart3_tx_data_buf[send_point_buf++] = '0';
        cb_send_data_buf[index][26] = 0x00;
      }
      else{
        if(cb_send_data_buf[index][26] & 0x01){
          cb_send_data_buf[index][26] &= 0xfe;
          uart3_tx_data_buf[send_point_buf++] = '0';
          if(cb_send_data_buf[index][27] & 0x01)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][26] & 0x02){
          cb_send_data_buf[index][26] &= 0xfd;
          uart3_tx_data_buf[send_point_buf++] = '1';
          if(cb_send_data_buf[index][27] & 0x02)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][26] & 0x04){
          cb_send_data_buf[index][26] &= 0xfb;
          uart3_tx_data_buf[send_point_buf++] = '2';
          if(cb_send_data_buf[index][27] & 0x04)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][26] & 0x08){
          cb_send_data_buf[index][26] &= 0xf7;
          uart3_tx_data_buf[send_point_buf++] = '3';
          if(cb_send_data_buf[index][27] & 0x08)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][26] & 0x10){
          cb_send_data_buf[index][26] &= 0xef;
          uart3_tx_data_buf[send_point_buf++] = '4';
          if(cb_send_data_buf[index][27] & 0x10)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][26] & 0x20){
          cb_send_data_buf[index][26] &= 0xdf;
          uart3_tx_data_buf[send_point_buf++] = '5';
          if(cb_send_data_buf[index][27] & 0x20)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][26] & 0x40){
          cb_send_data_buf[index][26] &= 0xbf;
          uart3_tx_data_buf[send_point_buf++] = '6';
          if(cb_send_data_buf[index][27] & 0x40)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
        else if(cb_send_data_buf[index][26] & 0x80){
          cb_send_data_buf[index][26] &= 0x7f;
          uart3_tx_data_buf[send_point_buf++] = '7';
          if(cb_send_data_buf[index][27] & 0x80)  uart3_tx_data_buf[send_point_buf++] = '1';
          else uart3_tx_data_buf[send_point_buf++] = '0';
        }
      }
      break;
       
    case 'a': //�ɾ�����
      uart3_tx_data_buf[send_point_buf++] = 'a'; 
      uart3_tx_data_buf[send_point_buf++] = common_data_buf[26]; 
      break;
       
    case 'b': //��������µ� ��ü
      uart3_tx_data_buf[send_point_buf++] = 'b'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[22] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[22] & 0x0f) | 0x30; 
      break;
       
    case 'c': //�ٴ������µ� ��ü
      uart3_tx_data_buf[send_point_buf++] = 'c'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[23] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[23] & 0x0f) | 0x30; 
      break;
       
    case 'd': //��������µ� ���Ǹ� ����
      uart3_tx_data_buf[send_point_buf++] = 'd'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[24] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[24] & 0x0f) | 0x30; 
      break;
       
    case 'e': //�ٴ������µ� ���Ǹ� ����
      uart3_tx_data_buf[send_point_buf++] = 'e'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[25] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[25] & 0x0f) | 0x30; 
      break;
       
    case 'f': //�ù� ���� ���ؿµ�
      uart3_tx_data_buf[send_point_buf++] = 'f'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[4] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[4] & 0x0f) | 0x30; 
      break;
       
    case 'g': //���� ���� ���ؿµ�
      uart3_tx_data_buf[send_point_buf++] = 'g'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[3] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[3] & 0x0f) | 0x30; 
      break;
       
    case 'h': //�ٴ� ���� ���ؿµ�
      uart3_tx_data_buf[send_point_buf++] = 'h'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[5] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[5] & 0x0f) | 0x30; 
      break;

    case 'i': //�׷���Ǳ���(�ù�)
      group_index = b_decimal_to_hex_table_3[index & 0x00ff];
      uart3_tx_data_buf[send_point_buf++] = 'i'; 
      uart3_tx_data_buf[send_point_buf++] = ((group_data_buf[group_index][0] & 0xf0) >> 4) | 0x30;
      uart3_tx_data_buf[send_point_buf++] = (group_data_buf[group_index][0] & 0x0f) | 0x30;
      break;
    case 'j': //�׷���Ǳ���(����)
      group_index = b_decimal_to_hex_table_3[index & 0x00ff];
      uart3_tx_data_buf[send_point_buf++] = 'j'; 
      uart3_tx_data_buf[send_point_buf++] = ((group_data_buf[group_index][1] & 0xf0) >> 4) | 0x30;
      uart3_tx_data_buf[send_point_buf++] = (group_data_buf[group_index][1] & 0x0f) | 0x30;
      break;
    case 'k': //�׷���Ǳ���(�ٴ�)
      group_index = b_decimal_to_hex_table_3[index & 0x00ff];
      uart3_tx_data_buf[send_point_buf++] = 'k'; 
      uart3_tx_data_buf[send_point_buf++] = ((group_data_buf[group_index][2] & 0xf0) >> 4) | 0x30;
      uart3_tx_data_buf[send_point_buf++] = (group_data_buf[group_index][2] & 0x0f) | 0x30;
      break;

      
    case 'l': //��� ��.���� �µ�
      uart3_tx_data_buf[send_point_buf++] = 'l'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[6] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[6] & 0x0f) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[7] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[7] & 0x0f) | 0x30; 
      break;
       
    case 'm': //�ٴ� ��.���� �µ�
      uart3_tx_data_buf[send_point_buf++] = 'm'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[8] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[8] & 0x0f) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[9] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[9] & 0x0f) | 0x30; 
      break;
      
    case 'n': //����µ���
      uart3_tx_data_buf[send_point_buf++] = 'n'; 
      uart3_tx_data_buf[send_point_buf++] = common_data_buf[1]; 
      break;
  
    case 'o': //��,��,ȯ����
      uart3_tx_data_buf[send_point_buf++] = 'o'; 
      uart3_tx_data_buf[send_point_buf++] = common_data_buf[27]; 
      break;
      
    case 'p': //��,��,��,��,��
      uart3_tx_data_buf[send_point_buf++] = 'p'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[13] & 0xf0) >> 4) | 0x30;   //��
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[13] & 0x0f) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[14] & 0xf0) >> 4) | 0x30;   //��
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[14] & 0x0f) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[15] & 0xf0) >> 4) | 0x30;   //��
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[15] & 0x0f) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[16] & 0xf0) >> 4) | 0x30;   //��
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[16] & 0x0f) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[17] & 0xf0) >> 4) | 0x30;   //��
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[17] & 0x0f) | 0x30; 
      break;
   
    case 'q': //ȭ��溸�µ�
      uart3_tx_data_buf[send_point_buf++] = 'q'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[2] & 0xf0) >> 4) | 0x30;   //��
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[2] & 0x0f) | 0x30; 
      break;
      
    case 'r': //��.���� ���ѿµ���
      uart3_tx_data_buf[send_point_buf++] = 'r'; 
      uart3_tx_data_buf[send_point_buf++] = common_data_buf[10]; 
      break;
       
       
    case 'x': //�Ա��� ���� �ð�
      uart3_tx_data_buf[send_point_buf++] = 'x'; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[18] & 0x0f) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[19] & 0xf0) >> 4) | 0x30;   //��
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[19] & 0x0f) | 0x30; 
      break;
      
    case 'y': //û������ �ð�
      uart3_tx_data_buf[send_point_buf++] = 'y'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[20] & 0xf0) >> 4) | 0x30;   //��
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[20] & 0x0f) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[21] & 0xf0) >> 4) | 0x30;   //��
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[21] & 0x0f) | 0x30; 
      break;
  
    case 'I': //Chime volume Level
      uart3_tx_data_buf[send_point_buf++] = 'I'; 
      if(f_uart3_common_data){    //��ü ����?
        uart3_tx_data_buf[send_point_buf++] = common_data_buf[28]; 
      }
      else{
        uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][29]; 
      }
      break;
     
    case 'J': //Alram volume Level
      uart3_tx_data_buf[send_point_buf++] = 'J'; 
      if(f_uart3_common_data){    //��ü ����?
        uart3_tx_data_buf[send_point_buf++] = common_data_buf[29]; 
      }
      else{
        uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][30]; 
      }
      break;
  
    case 'z': //���� ���� �ð� �� ���
      uart3_tx_data_buf[send_point_buf++] = 'z'; 
      uart3_tx_data_buf[send_point_buf++] = common_data_buf[30]; 
      break;
  
    case 'K': //�µ����� �µ� ǥ�ù��
      uart3_tx_data_buf[send_point_buf++] = 'K'; 
      uart3_tx_data_buf[send_point_buf++] = common_data_buf[31]; 
      break;

    case '$':   //�����µ� ����                       //h@20140704 =>
      uart3_tx_data_buf[send_point_buf++] = '$'; 
      if(bojung_flag == 0xff){
        uart3_tx_data_buf[send_point_buf++] = '?';
        uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][39];
        bojung_flag = 0;
      }
      else if(bojung_flag & 0x01){
        uart3_tx_data_buf[send_point_buf++] = '0';
        uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][39];
        bojung_flag &= 0xfe;
      }
      else if(bojung_flag & 0x02){
        uart3_tx_data_buf[send_point_buf++] = '1';
        uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][39];
        bojung_flag &= 0xfd;
      }
      else if(bojung_flag & 0x04){
        uart3_tx_data_buf[send_point_buf++] = '2';
        uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][39];
        bojung_flag &= 0xfb;
      }      
      else if(bojung_flag & 0x08){
        uart3_tx_data_buf[send_point_buf++] = '3';
        uart3_tx_data_buf[send_point_buf++] = cb_send_data_buf[index][39];
        bojung_flag &= 0xf7;
      }            
      
      break;                                    //<= h@20140704
      
    case 'Q': //���ؿµ� ���/�ٴ�    //h@20140711
      uart3_tx_data_buf[send_point_buf++] = 'L'; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[32] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[32] & 0x0f) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = ((common_data_buf[33] & 0xf0) >> 4) | 0x30; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[33] & 0x0f) | 0x30; 
      break;
      
    case 'M': //���� ���� �ù�,����,ȯ���� ����
      uart3_tx_data_buf[send_point_buf++] = 'M'; 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[41] & 0x0f);
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[42] & 0x0f);
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[43] & 0x0f); 
      uart3_tx_data_buf[send_point_buf++] = (common_data_buf[44] & 0x0f);
      break;
  }
}

void cb_send_crc_cal(void)
{
  unsigned char j,crc=0; 
  unsigned int cb_rx_id;
  
  for(j=1; j<send_point_buf; ++j) crc += uart3_tx_data_buf[j];
  crc ^= 0x55;
  uart3_tx_data_buf[send_point_buf++] = ((crc & 0xf0) >> 4) | 0x30;          //CRC
  uart3_tx_data_buf[send_point_buf++] = (crc & 0x0f) | 0x30;                 //CRC
  uart3_tx_data_buf[send_point_buf++] = ')';                                 //ETX
  f_uart3_data_send = 1;
  uart3_tx_length = 0;
  uart3_tx_backup = send_point_buf;
  
  if(++device_comm_status[device_count] > 10) device_comm_status[device_count] = 20;
  
  if(device_comm_status[device_count] == 20) //������ �߻�(DTC & FIP)
  {
      for(int i=0; i<b_max_cb_count[device_count];i++)
      {
        cb_rx_id = (dtc_rcu_count[device_count][i]);
        cb_rx_room_data_buf[cb_rx_id][2] = '4'; //�ش� DTC RCU �� '4' �����ַ� ��������
      }
  }
}

unsigned int cb_first_send_check(void)
{
  if(cb_first_save_point != cb_first_check_point){
    if(++cb_first_check_point > 799) cb_first_check_point = 0;
    return(cb_first_save_buffer[cb_first_check_point][0]);
  }
  else return(0x9999);
}


/* UART2 data send check routine */
void UART3_tx_check(void)
{
  if(f_uart3_data_send){
	  if(USART_GetFlagStatus(USART3, USART_FLAG_TC) !=RESET) {
      if(uart3_tx_length < uart3_tx_backup){
        USART3->DR = (uart3_tx_data_buf[uart3_tx_length++] & (u16)0x01FF);
      }
      else{
        P_u3dir = 0;
        f_uart3_data_send = 0;
        uart3_call_timer = 10;
      }
    }
  }
}

void UART3_rxd_check(void)
{
  
  unsigned char rx_crc=0;
  unsigned char i;
  unsigned int cb_rx_id;
  unsigned char data_change = 0;
  
  
  
  if(f_uart3_frame_rx_ok){
    f_uart3_frame_rx_ok = 0;
  
    f_sign_sys_rx = 1;
    sign_led_sys_rx = 0;
    
    for(i=0; i<uart3_rxd_buf[1]-2; ++i) rx_crc += uart3_rxd_buf[i];
    rx_crc ^= 0x55;
    if(rx_crc == (uart3_rxd_buf[uart3_rxd_buf[1]-2])){
      P_led8++;
      cb_rx_id = (uart3_rxd_buf[2] * 0x100) + uart3_rxd_buf[3];
      device_comm_status[uart3_rxd_buf[4]] = 0;
      if(cb_rx_id < cb_max_count){
//0    1        2       3       4      5        
//{  length, cb_id1, cb_id2, fip_no, data1 ~ data n , crc }
        
        if(cb_rx_id == 74){
          P_led8++;
        }
        
        data_change = 0;
        cb_rx_room_data_buf[cb_rx_id][0] = 'R';   //�ش� ID data ���ŵ�(�̶��� PC�� Data �۽��Ѵ�)
        
        if(cb_rx_room_data_buf[cb_rx_id][1] != uart3_rxd_buf[5]){
          cb_rx_room_data_buf[cb_rx_id][1] = uart3_rxd_buf[5];
          data_change = 1;
        }
        if(cb_rx_room_data_buf[cb_rx_id][2] != uart3_rxd_buf[6]){
          cb_rx_room_data_buf[cb_rx_id][2] = uart3_rxd_buf[6];
          data_change = 1;
        }
        if(cb_rx_room_data_buf[cb_rx_id][60] != uart3_rxd_buf[7]){
          cb_rx_room_data_buf[cb_rx_id][60] = uart3_rxd_buf[7];
          data_change = 1;
        }
        if(cb_rx_room_data_buf[cb_rx_id][3] != uart3_rxd_buf[8]){
          cb_rx_room_data_buf[cb_rx_id][3] = uart3_rxd_buf[8];
          data_change = 1;
        }
        
        for(i=0; i<4; ++i){
          if(cb_rx_room_data_buf[cb_rx_id][i+4] != uart3_rxd_buf[i+9]){
            cb_rx_room_data_buf[cb_rx_id][i+4] = uart3_rxd_buf[i+9];
            data_change = 1;
          }
        }
        for(i=0; i<4; ++i){
          if(cb_rx_room_data_buf[cb_rx_id][i+12] != uart3_rxd_buf[i+13]){
            cb_rx_room_data_buf[cb_rx_id][i+12] = uart3_rxd_buf[i+13];
            data_change = 1;
          }
        }
        for(i=0; i<4; ++i){
          if(cb_rx_room_data_buf[cb_rx_id][i+20] != uart3_rxd_buf[i+17]){
            cb_rx_room_data_buf[cb_rx_id][i+20] = uart3_rxd_buf[i+17];
            data_change = 1;
          }
        }
        for(i=0; i<4; ++i){
          if(cb_rx_room_data_buf[cb_rx_id][i+28] != uart3_rxd_buf[i+21]){
            cb_rx_room_data_buf[cb_rx_id][i+28] = uart3_rxd_buf[i+21];
            data_change = 1;
          }
        }
        for(i=0; i<4; ++i){
          if(cb_rx_room_data_buf[cb_rx_id][i+36] != uart3_rxd_buf[i+25]){
            cb_rx_room_data_buf[cb_rx_id][i+36] = uart3_rxd_buf[i+25];
            data_change = 1;
          }
        }
        for(i=0; i<4; ++i){
          if(cb_rx_room_data_buf[cb_rx_id][i+44] != uart3_rxd_buf[i+29]){
            cb_rx_room_data_buf[cb_rx_id][i+44] = uart3_rxd_buf[i+29];
            data_change = 1;
          }
        }
        if(cb_rx_room_data_buf[cb_rx_id][53] != uart3_rxd_buf[33]){
          cb_rx_room_data_buf[cb_rx_id][53] = uart3_rxd_buf[33];
          data_change = 1;
        }

        if(cb_rx_room_data_buf[cb_rx_id][62] != uart3_rxd_buf[34]){   //Service change
          cb_rx_room_data_buf[cb_rx_id][62] = uart3_rxd_buf[34];
          data_change = 1;
        }
        
        if(cb_rx_room_data_buf[cb_rx_id][64] != uart3_rxd_buf[35]){   // inspertor
          cb_rx_room_data_buf[cb_rx_id][64] = uart3_rxd_buf[35];
          data_change = 1;
        }
        
        if(cb_rx_room_data_buf[cb_rx_id][65] != uart3_rxd_buf[36]){   // Check-out request
          cb_rx_room_data_buf[cb_rx_id][65] = uart3_rxd_buf[36];
          data_change = 1;
        }
                                             
        for(i=0; i<4; i++)      
        {
            if(cb_rx_room_data_buf[cb_rx_id][i+66] != uart3_rxd_buf[i+37])        //�ű� ������ ����
            {
            cb_rx_room_data_buf[cb_rx_id][i+66] = uart3_rxd_buf[i+37];          // ����
            data_change = 1;                                                                      // ��� ��ȯ
            }
        }
        
        if(data_change){
          priority_pc_send(cb_rx_id);
        }
      }
      
      
      uart3_call_timer = 15;
    }
  }  
}

          
void basic_data_execution(unsigned char index)
{
  switch(cb_rx_room_data_buf[index][2] & 0x03){
    case 0:
      if(!(cb_send_data_buf[index][2] & 0x01)){  //Check Out �����ΰ�?
        if(cb_rx_room_data_buf[index][61] & 0x02){   //����̾�����?
          cb_rx_room_data_buf[index][60] = 0x03;     //û������ ����
          cb_rx_room_data_buf[index][61] &= 0xfd;    //��� Flag clear
        }
      }
      break;
     
    case 1:   //û����?                                
      break;
      
    case 2:   //û�ҿϷ�
      cb_rx_room_data_buf[index][60] = 0x02;     //û�ҿϷ� ����
      cb_rx_room_data_buf[index][61] &= 0xfd;    //��� Flag clear
      break;
      
    case 3:   //���?
      cb_rx_room_data_buf[index][61] |= 0x02;    //��� Flag set
      break;
      
  }
}


void priority_pc_send(unsigned int cb_id)
{
  if(++priority_pc_send_save_point > 799) priority_pc_send_save_point = 0;
  priority_pc_send_buffer[priority_pc_send_save_point] = cb_id;
}


/******************************************************************************************************/
/* UART3_initial :                                                                                    */ 											
/******************************************************************************************************/
void UART3_initial(void){
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);    
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
    
    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
     
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 19200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART3, &USART_InitStructure);

      /* Enable USART */
    USART_Cmd(USART3, ENABLE);    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    
    //
    // Install USART1 Handler with preemtion level one above lowest level to ensure communication
    //
    // Enable the USART1 Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



//=======================================================================
//=======================================================================

