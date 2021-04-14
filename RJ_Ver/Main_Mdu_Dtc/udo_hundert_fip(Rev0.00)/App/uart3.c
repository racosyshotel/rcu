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
  unsigned char temp_id;
  
  if(f_uart3_send_time){
    f_uart3_send_time = 0;
    
    if(!f_power_on_start)
    {
      f_power_on_start = 1;
      cb_command_send(0xff,'R');
      P_led3 ^= 1;
    }
    else
    {
      if(f_null_send)
      {
        f_null_send = 0;
        cb_null_send();
      }
      else{
        if(tx_send_id_back < 0xa0)
        {
          if(++cb_send_data_buf[tx_send_id_back][0] > 2)
          {
            cb_send_data_buf[tx_send_id_back][0] = 5;
            cb_suncha_send();
          }
          else
          {
            cb_command_send(tx_send_id_back,cb_first_save_buffer[cb_first_check_point][1]);
          }
        }
        else{
          if(++common_send_count > 5){    //h@20150127
            common_send_count = 6;        //h@20150127
            temp_id = cb_first_send_check();
            if(temp_id == 0xff) cb_suncha_send();
            else{
              cb_command_send(temp_id,cb_first_save_buffer[cb_first_check_point][1]);
              if(temp_id < 64) cb_send_data_buf[temp_id][0] = 0;        //cb_send_data_buf[temp_id][0] = 0;
              if((temp_id == 0xfe) && (cb_first_save_buffer[cb_first_check_point][1] == 'f')){
                common_send_count = 0;
              }
            }
          }
          else{                           //h@20150127 =>
            cb_command_send(0xfe,cb_first_save_buffer[cb_first_check_point][1]);
          }                               //<= h@20150127
        }
      }
    }
    
 }
  UART3_tx_check();
  UART3_rxd_check();
//  led_setup(sub_call_id);

}

void cb_null_send(void)     //h@20141219 =>
{
  unsigned char i=0;
  
  P_u3dir = 1;
  uart3_tx_data_buf[i++] = ')';                                 //Preamble
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                             //ETX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                             //ETX
  uart3_tx_data_buf[i++] = ')';                                 //Preamble
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                             //ETX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                             //ETX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                             //ETX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                             //ETX
  uart3_tx_data_buf[i++] = ')';                                 //STX
  uart3_tx_data_buf[i++] = ')';                             //ETX
  f_uart3_data_send = 1;
  uart3_tx_length = 0;
  uart3_tx_backup = i;
}                           //<= h@20141219
    
    
void cb_suncha_send(void)
{
  unsigned char i=0;
  unsigned char j;
  unsigned char crc = 0;
  
  P_u3dir = 1;
  if(sub_call_id >= cb_max_count[my_device_no])
  {
    sub_call_id = 0;
    f_main_send_condition = 1;
  }
  uart3_tx_data_buf[i++] = ' ';                                 //Preamble
  uart3_tx_data_buf[i++] = '(';                                 //STX
  ++i;                                                          //Length
  uart3_tx_data_buf[i++] = sub_call_id;                         //ID
  uart3_tx_data_buf[i++] = 'G';                                 //Command
  uart3_tx_data_buf[i++] = cb_send_data_buf[sub_call_id][2];    //Data1
  uart3_tx_data_buf[i++] = cb_send_data_buf[sub_call_id][1];    //Group No
  uart3_tx_data_buf[i++] = dip_function_2;                      //Dip switch 1번 DATA 전송 21.01.13
  
  uart3_tx_data_buf[2] = i+1;
  for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
  crc ^= 0x55;
  uart3_tx_data_buf[i++] = crc;                             //CRC
  uart3_tx_data_buf[i++] = ')';                             //ETX
  f_uart3_data_send = 1;
  uart3_tx_length = 0;
  uart3_tx_backup = i;
  tx_send_id_back = 0xff;
  
  if(cb_rx_room_data_buf[sub_call_id][63] > 20){
    if(cb_rx_room_data_buf[sub_call_id][63] != 30){
      cb_rx_room_data_buf[sub_call_id][63] = 30;
      priority_pc_send(sub_call_id);
      cb_rx_room_data_buf[sub_call_id][0] = 'R';
    }
  }
  else ++cb_rx_room_data_buf[sub_call_id][63];
  
  
  led_setup(sub_call_id);
  ++sub_call_id;
  
/*  
  P_u3dir = 1;
  if(++sub_call_id >= 64) sub_call_id = 0;
  uart3_tx_data_buf[i++] = ' ';                                 //Preamble
  uart3_tx_data_buf[i++] = '(';                                 //STX
  ++i;                                                          //Length
  uart3_tx_data_buf[i++] = sub_call_id;                         //ID
  uart3_tx_data_buf[i++] = 'C';                                 //Command
  uart3_tx_data_buf[i++] = cb_send_data_buf[sub_call_id][2];    //Data1
  uart3_tx_data_buf[i++] = '1';                                 //Data2
  uart3_tx_data_buf[i++] = '2';                                 //Data3
  uart3_tx_data_buf[2] = i+1;
  for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
  crc ^= 0x55;
  uart3_tx_data_buf[i++] = crc;                             //CRC
  uart3_tx_data_buf[i++] = ')';                             //ETX
  f_uart3_data_send = 1;
  uart3_tx_length = 0;
  uart3_tx_backup = i;
  tx_send_id_back = 0xff;
*/  
  
  
}

void cb_command_send(unsigned char index, unsigned char send_cmd)
{
  unsigned char i=0;
  unsigned char j;
  unsigned char crc = 0;
  
  switch(send_cmd){
    case 'T':   //대기 설정온도 개별 송신
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      uart3_tx_data_buf[i++] = index;                         //ID
      uart3_tx_data_buf[i++] = 'T';                                 //Command
      uart3_tx_data_buf[i++] = cb_send_data_buf[index][3];
      if(cb_send_data_buf[index][3] & 0x01){
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][4];
        cb_send_data_buf[index][3] &= 0xfe;
      }
      if(cb_send_data_buf[index][3] & 0x02){
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][5];
        cb_send_data_buf[index][3] &= 0xfd;
      }
      if(cb_send_data_buf[index][3] & 0x04){
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][6];
        cb_send_data_buf[index][3] &= 0xfb;
      }
      if(cb_send_data_buf[index][3] & 0x08){
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][7];
        cb_send_data_buf[index][3] &= 0xf7;
      }
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = index;
      break;
      
    case 't':   //대기 설정온도 전체 송신
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      if(index == 0xfe){ 
        uart3_tx_data_buf[i++] = 0xff;                               //ID
        uart3_tx_data_buf[i++] = 't';                                 //Command
        uart3_tx_data_buf[i++] = 0xff;
        uart3_tx_data_buf[i++] = common_data_buf[17];
        uart3_tx_data_buf[i++] = common_data_buf[17];
        uart3_tx_data_buf[i++] = common_data_buf[17];
        uart3_tx_data_buf[i++] = common_data_buf[17];
      }
      else{
        uart3_tx_data_buf[i++] = index;                               //ID
        uart3_tx_data_buf[i++] = 't';                                 //Command
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][3];
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][4];
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][5];
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][6];
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][7];
        cb_send_data_buf[index][3] = 0x00;
      }
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = index;
      break;
    case 'F':   //바닥 설정온도 개별 송신
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      uart3_tx_data_buf[i++] = index;                         //ID
      uart3_tx_data_buf[i++] = 'F';                                 //Command
      uart3_tx_data_buf[i++] = cb_send_data_buf[index][12];
      if(cb_send_data_buf[index][12] & 0x01){
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][13];
        cb_send_data_buf[index][12] &= 0xfe;
      }
      if(cb_send_data_buf[index][12] & 0x02){
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][14];
        cb_send_data_buf[index][12] &= 0xfd;
      }
      if(cb_send_data_buf[index][12] & 0x04){
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][15];
        cb_send_data_buf[index][12] &= 0xfb;
      }
      if(cb_send_data_buf[index][12] & 0x08){
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][16];
        cb_send_data_buf[index][12] &= 0xf7;
      }
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = index;
      break;
    case 'f':   //바닥 설정온도 전체 송신
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      if(index == 0xfe){ 
        uart3_tx_data_buf[i++] = 0xff;                               //ID
        uart3_tx_data_buf[i++] = 'f';                                 //Command
        uart3_tx_data_buf[i++] = 0xff;
        uart3_tx_data_buf[i++] = common_data_buf[18];
        uart3_tx_data_buf[i++] = common_data_buf[18];
        uart3_tx_data_buf[i++] = common_data_buf[18];
        uart3_tx_data_buf[i++] = common_data_buf[18];
      }
      else{
        uart3_tx_data_buf[i++] = index;                               //ID
        uart3_tx_data_buf[i++] = 'f';                                 //Command
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][12];
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][13];
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][14];
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][15];
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][16];
        cb_send_data_buf[index][12] = 0x00;
      }
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = index;
      break;
      
    case 'O':   //온도센서 ON/OFF 원격제어
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      uart3_tx_data_buf[i++] = index;                         //ID
      uart3_tx_data_buf[i++] = 'O';                                 //Command
      uart3_tx_data_buf[i++] = cb_send_data_buf[index][21];
      uart3_tx_data_buf[i++] = cb_send_data_buf[index][22];
      cb_send_data_buf[index][21] = 0x00;
      uart3_tx_data_buf[i++] = cb_send_data_buf[index][23];
      uart3_tx_data_buf[i++] = cb_send_data_buf[index][24];
      cb_send_data_buf[index][23] = 0x00;
        
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = index;
      break;
      
    case 'P':   //공통 Data 송신
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      uart3_tx_data_buf[i++] = 0xff;                                //ID
      uart3_tx_data_buf[i++] = 'P';                                 //Command
      uart3_tx_data_buf[i++] = common_data_buf[1];                  //외출온도차 & 냉난방 전환온도차
      uart3_tx_data_buf[i++] = common_data_buf[2];                  //이상경보온도
      uart3_tx_data_buf[i++] = common_data_buf[3];                  //대기난방공실기준온도
      uart3_tx_data_buf[i++] = common_data_buf[4];                  //대기냉방공실기준온도
      uart3_tx_data_buf[i++] = common_data_buf[5];                  //바닥공실기준온도
      uart3_tx_data_buf[i++] = common_data_buf[6];                  //대기온도센서 상.하한온도
      uart3_tx_data_buf[i++] = common_data_buf[7];                  //바닥온도센서 상.하한온도
      uart3_tx_data_buf[i++] = common_data_buf[8];                  //년
      uart3_tx_data_buf[i++] = common_data_buf[9];                  //월
      uart3_tx_data_buf[i++] = common_data_buf[10];                 //일
      uart3_tx_data_buf[i++] = common_data_buf[11];                 //시
      uart3_tx_data_buf[i++] = common_data_buf[12];                 //분
      uart3_tx_data_buf[i++] = common_data_buf[13];                 //입구등점등유지시간
      uart3_tx_data_buf[i++] = common_data_buf[14];                 //입구등점등유지시간
      uart3_tx_data_buf[i++] = common_data_buf[23];                 //조도조절 시간 및 방법
      uart3_tx_data_buf[i++] = common_data_buf[24];                 //온도센서 온도표시방법 1=설정온도, 0=현재온도
      
      uart3_tx_data_buf[i++] = common_data_buf[25];                 //기준온도 대기   //h@20140711
      uart3_tx_data_buf[i++] = common_data_buf[26];                 //기준온도 바닥   //h@20140711
      
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;

      tx_send_id_back = 0xff;
      break;
      
    case 'I':   //Chime & Alram Volume Level
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      if(index == 0xfe){ 
        uart3_tx_data_buf[i++] = 0xff;                         //ID
        uart3_tx_data_buf[i++] = 'I';                                 //Command
        uart3_tx_data_buf[i++] = common_data_buf[21];
        uart3_tx_data_buf[i++] = common_data_buf[22];
        tx_send_id_back = 0xff;
      }
      else{
        uart3_tx_data_buf[i++] = index;                         //ID
        uart3_tx_data_buf[i++] = 'I';                                 //Command
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][26];
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][27];
        tx_send_id_back = index;
      }
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      break;
      
    case 'R':   //All data send order
      uart3_tx_data_buf[i++] = ' ';                           //Preamble
      uart3_tx_data_buf[i++] = '(';                           //STX
      ++i;                                                    //Length
      uart3_tx_data_buf[i++] = 0xff;                          //ID
      uart3_tx_data_buf[i++] = 'R';                           //Command
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                           //CRC
      uart3_tx_data_buf[i++] = ')';                           //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = 0xff;
      break;
      
      
    case 'i':   //group Data 송신
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      uart3_tx_data_buf[i++] = 0xff;                                //ID
      uart3_tx_data_buf[i++] = 'i';                                 //Command
      uart3_tx_data_buf[i++] = index - 0xa0;                        //Group No
      uart3_tx_data_buf[i++] = group_data_buf[index - 0xa0][0];     //그룹공실(냉방)
      uart3_tx_data_buf[i++] = group_data_buf[index - 0xa0][1];     //그룹공실(난방)
      uart3_tx_data_buf[i++] = group_data_buf[index - 0xa0][2];     //그룹공실(바닥)
        
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = 0xff;
      break;
      
 
    case '$':   //보정온도  //h@20140704
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      uart3_tx_data_buf[i++] = index;                         //ID
      uart3_tx_data_buf[i++] = '$'; 
      if(bojung_flag == 0xff){
        uart3_tx_data_buf[i++] = '?';
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][39];
        bojung_flag = 0;
      }
      else if(bojung_flag & 0x01){
        uart3_tx_data_buf[i++] = '0';
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][39];
        bojung_flag &= 0xfe;
      }
      else if(bojung_flag & 0x02){
        uart3_tx_data_buf[i++] = '1';
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][39];
        bojung_flag &= 0xfd;
      }
      else if(bojung_flag & 0x04){
        uart3_tx_data_buf[i++] = '2';
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][39];
        bojung_flag &= 0xfb;
      }      
      else if(bojung_flag & 0x08){
        uart3_tx_data_buf[i++] = '3';
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][39];
        bojung_flag &= 0xf7;
      }         
      else{
        uart3_tx_data_buf[i++] = '7';
        uart3_tx_data_buf[i++] = cb_send_data_buf[index][39];
        bojung_flag = 0x00;
      }
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = index;
      break;        //<= h@20140704
      
      
      

    case 'n':   //DND On-line control
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      uart3_tx_data_buf[i++] = index;                               //ID
      uart3_tx_data_buf[i++] = 'n';                                 //Command
      if(cb_send_data_buf[index][33] & 0x01) uart3_tx_data_buf[i++] = '1';
      else uart3_tx_data_buf[i++] = '0';
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = index;
      break;

    case 'o':   //MUR On-line control
      P_u3dir = 1;
      uart3_tx_data_buf[i++] = ' ';                                 //Preamble
      uart3_tx_data_buf[i++] = '(';                                 //STX
      ++i;                                                          //Length
      uart3_tx_data_buf[i++] = index;                               //ID
      uart3_tx_data_buf[i++] = 'o';                                 //Command
      if(cb_send_data_buf[index][33] & 0x02) uart3_tx_data_buf[i++] = '1';
      else uart3_tx_data_buf[i++] = '0';
      uart3_tx_data_buf[2] = i+1;
      for(j=1; j<i; ++j) crc += uart3_tx_data_buf[j];
      crc ^= 0x55;
      uart3_tx_data_buf[i++] = crc;                             //CRC
      uart3_tx_data_buf[i++] = ')';                             //ETX
      f_uart3_data_send = 1;
      uart3_tx_length = 0;
      uart3_tx_backup = i;
      tx_send_id_back = index;
      break;

      
      
  }
}


unsigned char cb_first_send_check(void)
{
  if(cb_first_save_point != cb_first_check_point){
    if(++cb_first_check_point > 99) cb_first_check_point = 0;
    return(cb_first_save_buffer[cb_first_check_point][0]);
  }
  else return(0xff);
}


/* UART2 data send check routine */
void UART3_tx_check(void)
{
//  if(f_uart3_data_send){  //h@20150127
  if(f_uart3_delay_time){
	  if(USART_GetFlagStatus(USART3, USART_FLAG_TC) !=RESET) {
      if(uart3_tx_length < uart3_tx_backup){
        USART3->DR = (uart3_tx_data_buf[uart3_tx_length++] & (u16)0x01FF);
      }
      else{
        P_u3dir = 0;
        f_uart3_data_send = 0;
        f_uart3_delay_time = 0; //h@20150127
      }
    }
  }
}

void UART3_rxd_check(void)
{
  
  unsigned char rx_crc=0;
  unsigned char i;
  unsigned char cb_rx_id;
  unsigned char data_change = 0;
  
  if(f_uart3_frame_rx_ok)
  {
    f_uart3_frame_rx_ok = 0;
      
    for(i=0; i<uart3_rxd_buf[1]-2; ++i) rx_crc += uart3_rxd_buf[i];
    rx_crc ^= 0x55;
    if(rx_crc == (uart3_rxd_buf[uart3_rxd_buf[1]-2])){
      
      for(i=0; i<uart3_rxd_buf[1]-1; ++i){              //h@20141219 =>
        if(uart3_rxd_buf[i] == '(') f_null_send = 1;
      }                                                 //<= h@20141219
      
      P_led4++;
      cb_rx_id = uart3_rxd_buf[2];
      tx_send_id_back = 0xff;
      
      if(cb_rx_room_data_buf[cb_rx_id][63] == 30){  //h@20140702 ==>
        priority_pc_send(cb_rx_id);
      }                                                 
      cb_rx_room_data_buf[cb_rx_id][63] = 0;        //<== h@20140702  
      
      switch(uart3_rxd_buf[3]){
        case 'S':   //CB basic data
          data_change = 0;
          
          for(i=0; i<2; ++i)
          {
            if(cb_rx_room_data_buf[cb_rx_id][i+1] != uart3_rxd_buf[i+4]) data_change = 1;
            cb_rx_room_data_buf[cb_rx_id][i+1] = uart3_rxd_buf[i+4];
          }
          
          for(i=0; i<4; i++)
          {
            if(cb_rx_room_data_buf[cb_rx_id][i+66] != uart3_rxd_buf[i+6]) data_change = 1;      // sub device status 값이 수신된 새로운값이 들어올시 즉시 동작
            cb_rx_room_data_buf[cb_rx_id][i+66] = uart3_rxd_buf[i+6];                           // 수신된 데이터(uart3_rx ..)를 메모리(cb_rx...)맵에 저장
          }
          
          if(cb_rx_room_data_buf[cb_rx_id][64] != uart3_rxd_buf[10]) data_change = 1;           // inspector 점검
          cb_rx_room_data_buf[cb_rx_id][64] = uart3_rxd_buf[10];
          if(cb_rx_room_data_buf[cb_rx_id][65] != uart3_rxd_buf[11]) data_change = 1;           // check out 원격
          cb_rx_room_data_buf[cb_rx_id][65] = uart3_rxd_buf[11];
          
          basic_data_execution(cb_rx_id);
          if(data_change){
            priority_pc_send(cb_rx_id);
          }
          cb_rx_room_data_buf[cb_rx_id][0] = 'R';   //CB All Data 수신
          
          break;
        case 't':   //CB basci data + TS status + Air TS Set temp & Check temp
          data_change = 0;
          for(i=0; i<3; ++i){
            if(cb_rx_room_data_buf[cb_rx_id][i+1] != uart3_rxd_buf[i+4]) data_change = 1;
            cb_rx_room_data_buf[cb_rx_id][i+1] = uart3_rxd_buf[i+4];
          }
//          data_change = 0;
          for(i=0; i<8; ++i){
            if(cb_rx_room_data_buf[cb_rx_id][i+4] != uart3_rxd_buf[i+8]) data_change = 1;
            cb_rx_room_data_buf[cb_rx_id][i+4] = uart3_rxd_buf[i+8];
          }
//          data_change = 0;
          if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)){   //OFF                 
            for(i=0; i<8; ++i){
              if(cb_rx_room_data_buf[cb_rx_id][i+20] != uart3_rxd_buf[i+12]) data_change = 1;
              cb_rx_room_data_buf[cb_rx_id][i+20] = uart3_rxd_buf[i+12];        
            }
            for(i=0; i<8; ++i){
              cb_rx_room_data_buf[cb_rx_id][i+28] = uart3_rxd_buf[i+20];           
            }
          }
          else{                                           //Check temp priority send
            for(i=0; i<16; ++i){
              if(cb_rx_room_data_buf[cb_rx_id][i+20] != uart3_rxd_buf[i+12]) data_change = 1;
              cb_rx_room_data_buf[cb_rx_id][i+20] = uart3_rxd_buf[i+12];
            }
          }
          if(data_change) priority_pc_send(cb_rx_id);
          basic_data_execution(cb_rx_id);
          break;
        case 'T':   //TS status + Air TS Set temp & Check temp
          data_change = 0;
          for(i=0; i<8; ++i){
            if(cb_rx_room_data_buf[cb_rx_id][i+4] != uart3_rxd_buf[i+4]) data_change = 1;
            cb_rx_room_data_buf[cb_rx_id][i+4] = uart3_rxd_buf[i+4];
          }
//          data_change = 0;
          if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)){   //OFF
            for(i=0; i<8; ++i){
              if(cb_rx_room_data_buf[cb_rx_id][i+20] != uart3_rxd_buf[i+12]) data_change = 1;
              cb_rx_room_data_buf[cb_rx_id][i+20] = uart3_rxd_buf[i+12];
            }
            for(i=0; i<8; ++i){
              cb_rx_room_data_buf[cb_rx_id][i+28] = uart3_rxd_buf[i+20];
            }
          }
          else{                                           //Check temp priority send
            for(i=0; i<16; ++i){
              if(cb_rx_room_data_buf[cb_rx_id][i+20] != uart3_rxd_buf[i+12]) data_change = 1;
              cb_rx_room_data_buf[cb_rx_id][i+20] = uart3_rxd_buf[i+12];
            }
          }
          if(data_change) priority_pc_send(cb_rx_id);
          break;
        case 'F':   //Floor TS Set temp & Check temp
          data_change = 0;
          
          for(i=0; i<8; ++i){
            if(cb_rx_room_data_buf[cb_rx_id][i+12] != uart3_rxd_buf[i+4]) data_change = 1;
            cb_rx_room_data_buf[cb_rx_id][i+12] = uart3_rxd_buf[i+4];
          }
//          data_change = 0;
          if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)){   //OFF
            for(i=0; i<8; ++i){
              if(cb_rx_room_data_buf[cb_rx_id][i+36] != uart3_rxd_buf[i+12]) data_change = 1;
              cb_rx_room_data_buf[cb_rx_id][i+36] = uart3_rxd_buf[i+12];
            }
            for(i=0; i<8; ++i){
              cb_rx_room_data_buf[cb_rx_id][i+44] = uart3_rxd_buf[i+20];
            }
          }
          else{                                           //Check temp priority send
            for(i=0; i<16; ++i){
              if(cb_rx_room_data_buf[cb_rx_id][i+36] != uart3_rxd_buf[i+12]) data_change = 1;
              cb_rx_room_data_buf[cb_rx_id][i+36] = uart3_rxd_buf[i+12];
            }
          }
          if(data_change) priority_pc_send(cb_rx_id);
          break;
        case 'R':   //All data
          data_change = 0;
          for(i=0; i<53; ++i){
            if(cb_rx_room_data_buf[cb_rx_id][i+1] != uart3_rxd_buf[i+4]) data_change = 1;
            cb_rx_room_data_buf[cb_rx_id][i+1] = uart3_rxd_buf[i+4];
          }
          if(data_change) priority_pc_send(cb_rx_id);
          basic_data_execution(cb_rx_id);
          cb_rx_room_data_buf[cb_rx_id][0] = 'R';   //CB All Data 수신
          break;

      }
      
      uart3_call_timer = 19;
    }
    else{
      P_led3 ^= 1;
    }
  }  
}

          
void basic_data_execution(unsigned char index)
{
  Write_BKPSRAM(b_sram_room_base + (index*4), cb_rx_room_data_buf[index][2]);
  switch(cb_rx_room_data_buf[index][2] & 0x03){
    case 0:             
      /*//h@20160224
        if(cb_rx_room_data_buf[index][61] & 0x02){   //재실이었었나?
          cb_rx_room_data_buf[index][60] = 0x03;     //청소지시 선언
          cb_rx_room_data_buf[index][61] &= 0xfd;    //재실 Flag clear
          Write_BKPSRAM(b_sram_clean_base + index, cb_rx_room_data_buf[index][60]);
        }
        */
        
        
        if(!(cb_send_data_buf[index][2] & 0x01)){  //Check Out 상태인가?
          if(cb_rx_room_data_buf[index][61] & 0x02){   //재실이었었나?
            cb_rx_room_data_buf[index][60] = 0x03;     //청소지시 선언
            cb_rx_room_data_buf[index][61] &= 0xfd;    //재실 Flag clear
            Write_BKPSRAM(b_sram_clean_base + (index*4), cb_rx_room_data_buf[index][60]);
          }
        }
   
      break;
      
    case 1:   //청소중?                                
      break;
      
    case 2:   //청소완료
      cb_rx_room_data_buf[index][60] = 0x02;     //청소완료 선언              
      cb_rx_room_data_buf[index][61] &= 0xfd;    //재실 Flag clear        
      Write_BKPSRAM(b_sram_clean_base + (index*4), cb_rx_room_data_buf[index][60]);
      break;
      
    case 3:   //재실?
      cb_rx_room_data_buf[index][61] |= 0x02;    //재실 Flag set
      break;
      
  }
}


void priority_pc_send(unsigned char cb_id)
{
  if(++priority_pc_send_save_point > 99) priority_pc_send_save_point = 0;
  priority_pc_send_buffer[priority_pc_send_save_point] = cb_id;
}

void led_setup(unsigned char index)
{
  unsigned char fip_no = my_device_no;
	//재실?
	if(cb_rx_room_data_buf[index][2] == 3)	led_kind_buf[(b_room_led_position_table[fip_no][index][0] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][0] & 0x0f] = 0x0f;       //재실
	else{
    if(index == 0){
    if(cb_send_data_buf[index][2] & 0x01) led_kind_buf[(b_room_led_position_table[fip_no][index][0] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][0] & 0x0f] = 0x0c;    //Check IN
    else led_kind_buf[(b_room_led_position_table[fip_no][index][0] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][0] & 0x0f] = 0x00;                                         //공실
    }
    if(cb_send_data_buf[index][2] & 0x01) led_kind_buf[(b_room_led_position_table[fip_no][index][0] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][0] & 0x0f] = 0x0c;    //Check IN
    else led_kind_buf[(b_room_led_position_table[fip_no][index][0] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][0] & 0x0f] = 0x00;                                         //공실
	}
  
  //청소LED Set
	if(cb_rx_room_data_buf[index][2] == 1) 	led_kind_buf[(b_room_led_position_table[fip_no][index][1] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][1] & 0x0f] = 0x0c;     //청소중?
	else{
    if(cb_rx_room_data_buf[index][2] == 2) 	led_kind_buf[(b_room_led_position_table[fip_no][index][1] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][1] & 0x0f] = 0x00;   //청소완료?
    else{
      if(cb_rx_room_data_buf[index][60] == 3) led_kind_buf[(b_room_led_position_table[fip_no][index][1] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][1] & 0x0f] = 0x0f; //청소지시?
      else led_kind_buf[(b_room_led_position_table[fip_no][index][1] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][1] & 0x0f] = 0x00;                                    //청소대기
    }
  }
  
  if(dip_function & 0x02) //DIP 2번  ( 8번 SWITCH ) ON = DND,MUR STATE / OFF = 통신 STATE  @KKN20170727
  {
      //DND.MUR LED Set
    if(cb_rx_room_data_buf[index][1] & 0x10) led_kind_buf[(b_room_led_position_table[fip_no][index][2] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][2] & 0x0f] = 0x0c;   //MUR?
    else if(cb_rx_room_data_buf[index][1] & 0x08) led_kind_buf[(b_room_led_position_table[fip_no][index][2] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][2] & 0x0f] = 0x0f;   //DND?
    else                                          led_kind_buf[(b_room_led_position_table[fip_no][index][2] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][2] & 0x0f] = 0x00;   //X
  }
  else
  {
    //통신 LED Set
    if(cb_rx_room_data_buf[index][63] == 0x01) led_kind_buf[(b_room_led_position_table[fip_no][index][2] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][2] & 0x0f] = 0x00;        //통신OK
    else if(cb_rx_room_data_buf[index][63] == 0x1E) led_kind_buf[(b_room_led_position_table[fip_no][index][2] & 0xf0) >> 4][b_room_led_position_table[fip_no][index][2] & 0x0f] = 0x0c;   //통신NO    
  }
  
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

