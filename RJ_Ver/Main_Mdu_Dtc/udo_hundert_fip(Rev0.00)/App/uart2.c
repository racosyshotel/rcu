/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/

#define __UART2_C__

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
u8 u2ctrl =7, u2txinx;
void uart2_frame_proc(void)
{
  UART2_tx_check();
  UART2_rxd_check();
}

void uart2_data_send(void)
{
  unsigned char crc = 0;
  unsigned char i=0,j;
  unsigned int  temp_id;
  unsigned char temp_temp;
  
  temp_temp = priority_pc_send_check();
  if(temp_temp == 0xff)
  {  
    if(++cb_send_id >= b_max_cb_count[my_device_no]) cb_send_id = 0;
    temp_temp = cb_send_id;
  }
  else P_led1 ^= 1;
  
  temp_id = b_cb_send_id_table[my_device_no][temp_temp];
  
  if(cb_rx_room_data_buf[temp_temp][0] == 'R')
  {                      //CB의 Data를 수신하였는가?
    P_u2dir = 1; 
    uart2_tx_data_buf[i++] = ' ';                                     //Preamble
    uart2_tx_data_buf[i++] = '{';                                     //STX
    ++i;                                                              //Length
    
    uart2_tx_data_buf[i++] = (temp_id & 0xff00) >> 8;                 //ID
    uart2_tx_data_buf[i++] = (temp_id & 0x00ff);                      //ID
    
    uart2_tx_data_buf[i++] = my_device_no;                            //Standard Command
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][1];       //Basic status
    uart2_tx_data_buf[i] = cb_rx_room_data_buf[temp_temp][2];         //Key status   //h@20140702 ==>
    if(cb_rx_room_data_buf[temp_temp][63] > 20) uart2_tx_data_buf[i] = '4';
    ++i;                                                                                //<== h@20140702
    if(cb_rx_room_data_buf[temp_temp][2] == 1) uart2_tx_data_buf[i++] = '1';  //Key상태가 청소중이면 청소현황을 청소중으로 보낸다.
    else  uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][60];      //Clean status
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][3];       //Sub device communication status
                               
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][4];       //TS1 status status
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][5];       //TS2 status status
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][6];       //TS3 status status
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][7];       //TS4 status status
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][12];       //TS1 floor status status
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][13];       //TS2 floor status status
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][14];       //TS3 floor status status
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][15];       //TS4 floor status status
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][20];       //TS1 Set temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][21];       //TS2 Set temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][22];       //TS3 Set temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][23];       //TS4 Set temp
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][28];       //TS1 check temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][29];       //TS2 check temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][30];       //TS3 check temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][31];       //TS4 check temp

    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][36];       //TS1 Floor  temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][37];       //TS2 Floor  temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][38];       //TS3 Floor  temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][39];       //TS4 Floor  temp
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][44];       //TS1 Floor check temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][45];       //TS1 Floor check temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][46];       //TS1 Floor check temp
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][47];       //TS1 Floor check temp
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][53];       //TS1 ~ 4 Emergency & Fire emergency
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][62];       //Service change(K Hotel Only)
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][64];       //inspector 미사용 
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][65];       //Check out 요청( 1 = 요청 , 0 = 미요청 ) 미사용
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][66];        //LS 설치개수
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][67];        //LS [1] 통신상태
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][68];        //LS [2] 통신상태
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][69];        //기구물 사용 및 통신상태
  }
  else
  {
    P_u2dir = 1; 
    uart2_tx_data_buf[i++] = ' ';                                     //Preamble
    uart2_tx_data_buf[i++] = '{';                                     //STX
    ++i;                                                              //Length
    
    uart2_tx_data_buf[i++] = 0xff;                                    //ID
    uart2_tx_data_buf[i++] = 0xff;                                    //ID
    
    uart2_tx_data_buf[i++] = my_device_no;                            //Standard Command
  }
  
  
  uart2_tx_data_buf[2] = i+1;
  for(j=1; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc ^= 0x55;
  uart2_tx_data_buf[i++] = crc;                             //CRC
  uart2_tx_data_buf[i++] = '}';                             //ETX
  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;

}


unsigned char temp_dec_conv(unsigned char cb_index, unsigned char index)
{
  unsigned char temp_temp;
  unsigned char return_data;
  
  temp_temp = b_hex_to_decimal_table[(cb_rx_room_data_buf[cb_send_id][index] & 0x7f)];
  return_data = (temp_temp >> 4) | 0x30;                                     
  return(return_data);
}
unsigned char temp_05_check(unsigned char cb_index, unsigned char index)
{
  unsigned char temp_temp;
  unsigned char return_data;
  
  temp_temp = b_hex_to_decimal_table[(cb_rx_room_data_buf[cb_index][index] & 0x7f)];
  if(cb_rx_room_data_buf[cb_index][index] & 0x80) return_data = (temp_temp & 0x0f) | 0x40;    //0.5도
  else return_data = (temp_temp & 0x0f) | 0x30;
  return(return_data);
}

unsigned char priority_pc_send_check(void)
{
  if(priority_pc_send_save_point != priority_pc_send_check_point){
    if(++priority_pc_send_check_point > 99) priority_pc_send_check_point = 0;
    return(priority_pc_send_buffer[priority_pc_send_check_point]);
  }
  else return(0xff);
}

/* UART2 data send check routine */
void UART2_tx_check(void)
{
  if(f_uart2_data_send){
	  if(USART_GetFlagStatus(USART2, USART_FLAG_TC) !=RESET) {
      if(uart2_tx_length < uart2_tx_backup){
        USART2->DR = (uart2_tx_data_buf[uart2_tx_length++] & (u16)0x01FF);
      }
      else{
        P_u2dir = 0;
        f_uart2_data_send = 0;
      }
    }
  }
}

void UART2_rxd_check(void)
{
  
  unsigned char rx_crc=0;
  unsigned char temp_crc;
  unsigned char i;
  unsigned int cb_rx_id;
  unsigned char x;
  unsigned char temp_device_no;
  
  if(f_uart2_frame_rx_ok){
    f_uart2_frame_rx_ok = 0;
      
    for(i=0; i<uart2_rxd_buf[1]-2; ++i) rx_crc += uart2_rxd_buf[i];
    rx_crc ^= 0x55;
    temp_crc = ((uart2_rxd_buf[uart2_rxd_buf[1]-2] & 0x0f) << 4) | ((uart2_rxd_buf[uart2_rxd_buf[1]-1])& 0x0f);
    if(rx_crc == temp_crc){
     P_led2 ^= 1; 
      
      temp_device_no = ((uart2_rxd_buf[6] & 0x0f) << 4) | (uart2_rxd_buf[7] & 0x0f);
      cb_rx_id = hextbl1000[uart2_rxd_buf[2]&0x0F];
      cb_rx_id += hextbl100[uart2_rxd_buf[3]&0x0F];
      cb_rx_id += hextbl10[uart2_rxd_buf[4]&0x0F];
      cb_rx_id += hextbl1[uart2_rxd_buf[5]&0x0F];
      
      if(cb_rx_id < 0xff00){
        for(x=0; x<b_max_cb_count[my_device_no]; ++x){
          if(cb_rx_id == b_cb_send_id_table[my_device_no][x]){
            command_check(x);           //개별 command check
            break;
          }
        }
      }
      else{
        common_command_check(cb_rx_id);     //공통 command check
      }
      if(temp_device_no == my_device_no)
      {
        if(f_main_send_condition) 
          uart2_data_send();
      }
    }
  }  
}


void command_check(unsigned char cb_id)
{
  unsigned char temp_temp = 0;
  
  switch(uart2_rxd_buf[8]){
    case 'F':   //청소지시?
      if(uart2_rxd_buf[9] == '1') cb_rx_room_data_buf[cb_id][60] = 3;   //청소지시 선언
      else if(uart2_rxd_buf[9] == '0'){
        cb_rx_room_data_buf[cb_id][60] = 2;                             //청소완료 선언
        cb_rx_room_data_buf[cb_id][61] &= 0xfd;                         //재실 Flag clear
      }
      Write_BKPSRAM(b_sram_clean_base + (cb_id*4), cb_rx_room_data_buf[cb_id][60]);
      break;
      
    case 'A':   //Check IN/OUT
      if(uart2_rxd_buf[9] == '1') cb_send_data_buf[cb_id][2] |= 0x01;       //Check IN선언
      else if(uart2_rxd_buf[9] == '0') cb_send_data_buf[cb_id][2] &= 0xfe;  //Check OUT선언
      cb_send_data_buf[cb_id][1] = uart2_rxd_buf[10];                       //Group No
      Write_BKPSRAM(b_sram_check_io_base + (cb_id*4), cb_send_data_buf[cb_id][2]);
      break;
    case 'B':   //Message ON/OFF        
      if(uart2_rxd_buf[9] == '1') cb_send_data_buf[cb_id][2] |= 0x02;       //Message ON
      else if(uart2_rxd_buf[9] == '0') cb_send_data_buf[cb_id][2] &= 0xfd;  //Message OFF
      Write_BKPSRAM(b_sram_check_io_base + (cb_id*4), cb_send_data_buf[cb_id][2]);
      break;
    case 'C':   //예냉.예열
      if(uart2_rxd_buf[9] == '1') cb_send_data_buf[cb_id][2] |= 0x08;       //예냉.예열 ON
      else if(uart2_rxd_buf[9] == '0') cb_send_data_buf[cb_id][2] &= 0xf7;  //예냉.예열 OFF
      Write_BKPSRAM(b_sram_check_io_base + (cb_id*4), cb_send_data_buf[cb_id][2]);
      break;
      
    case 'D':   //대기 설정온도
      if(uart2_rxd_buf[9] != '?'){
        temp_temp = b_decimal_to_hex_table[((uart2_rxd_buf[10] & 0x0f) * 0x10) | (uart2_rxd_buf[11] & 0x0f)];
        cb_send_data_buf[cb_id][(uart2_rxd_buf[9] & 0x0f)+4] = temp_temp;
        cb_send_data_buf[cb_id][3] |= (0x01 << (uart2_rxd_buf[9] & 0x0f));
        cb_first_send_save(cb_id,'T');
      }
      else{     //대기 전체
        temp_temp = b_decimal_to_hex_table[((uart2_rxd_buf[10] & 0x0f) * 0x10) | (uart2_rxd_buf[11] & 0x0f)];
        cb_send_data_buf[cb_id][4] = temp_temp;
        cb_send_data_buf[cb_id][5] = temp_temp;
        cb_send_data_buf[cb_id][6] = temp_temp;
        cb_send_data_buf[cb_id][7] = temp_temp;
        cb_send_data_buf[cb_id][8] = temp_temp;
        cb_send_data_buf[cb_id][9] = temp_temp;
        cb_send_data_buf[cb_id][10] = temp_temp;
        cb_send_data_buf[cb_id][11] = temp_temp;
        cb_send_data_buf[cb_id][3] = 0xff;
        cb_first_send_save(cb_id,'t');
      }
      break;
    case 'E':   //바닥 설정온도
      if(uart2_rxd_buf[9] != '?'){
        temp_temp = b_decimal_to_hex_table[((uart2_rxd_buf[10] & 0x0f) * 0x10) | (uart2_rxd_buf[11] & 0x0f)];
        cb_send_data_buf[cb_id][(uart2_rxd_buf[9] & 0x0f)+13] = temp_temp;
        cb_send_data_buf[cb_id][12] |= (0x01 << (uart2_rxd_buf[9] & 0x0f));
        cb_first_send_save(cb_id,'F');
      }
      else{
        temp_temp = b_decimal_to_hex_table[((uart2_rxd_buf[10] & 0x0f) * 0x10) | (uart2_rxd_buf[11] & 0x0f)];
        cb_send_data_buf[cb_id][13] = temp_temp;
        cb_send_data_buf[cb_id][14] = temp_temp;
        cb_send_data_buf[cb_id][15] = temp_temp;
        cb_send_data_buf[cb_id][16] = temp_temp;
        cb_send_data_buf[cb_id][17] = temp_temp;
        cb_send_data_buf[cb_id][18] = temp_temp;
        cb_send_data_buf[cb_id][19] = temp_temp;
        cb_send_data_buf[cb_id][20] = temp_temp;
        cb_send_data_buf[cb_id][12] = 0xff;
        cb_first_send_save(cb_id,'f');
      }
      break;
      
      
    case 'G':   //대기 온도센서 ON/OFF원격제어
      if(uart2_rxd_buf[9] != '?'){
        cb_send_data_buf[cb_id][21] |= (0x01 << (uart2_rxd_buf[9] & 0x0f));
        if(uart2_rxd_buf[10] == '0') cb_send_data_buf[cb_id][22] &= (~(0x01 << (uart2_rxd_buf[9] & 0x0f)));
        else{
          if(uart2_rxd_buf[10] == '1') cb_send_data_buf[cb_id][22] |= (0x01 << (uart2_rxd_buf[9] & 0x0f));
        }
        cb_first_send_save(cb_id,'O');
      }
      else{
        cb_send_data_buf[cb_id][21] = 0xff;
        if(uart2_rxd_buf[10] == '0') cb_send_data_buf[cb_id][22] = 0x00;
        else{
          if(uart2_rxd_buf[10] == '1') cb_send_data_buf[cb_id][22] = 0xff;
        }
        cb_first_send_save(cb_id,'O');
      }
      break;
      
    case 'H':   //바닥 온도센서 ON/OFF원격제어
      if(uart2_rxd_buf[9] != '?'){
        cb_send_data_buf[cb_id][23] |= (0x01 << (uart2_rxd_buf[9] & 0x0f));
        if(uart2_rxd_buf[10] == '0') cb_send_data_buf[cb_id][24] &= (~(0x01 << (uart2_rxd_buf[9] & 0x0f)));
        else{
          if(uart2_rxd_buf[10] == '1') cb_send_data_buf[cb_id][24] |= (0x01 << (uart2_rxd_buf[9] & 0x0f));
        }
        cb_first_send_save(cb_id,'O');
      }
      else{
        cb_send_data_buf[cb_id][23] = 0xff;
        if(uart2_rxd_buf[10] == '0') cb_send_data_buf[cb_id][24] = 0x00;
        else{
          if(uart2_rxd_buf[10] == '1') cb_send_data_buf[cb_id][24] = 0xff;
        }
        cb_first_send_save(cb_id,'O');
      }
      break;
      
    case 'I':   //Chime Volume Level
      cb_send_data_buf[cb_id][26] = uart2_rxd_buf[9];
      cb_first_send_save(cb_id,'I');
      break;
      
    case 'J':   //Alram Volume Level
      cb_send_data_buf[cb_id][27] = uart2_rxd_buf[9];
      cb_first_send_save(cb_id,'I');
      break;
      
    case '$':     //보정온도      //h@20140704 =>
      if(((uart2_rxd_buf[9] >= '0') && (uart2_rxd_buf[9] <= '7')) || (uart2_rxd_buf[9] == '?'))
      {
        if(uart2_rxd_buf[10] != 0xff)
        {
          if(uart2_rxd_buf[6] == '?')
          {
            cb_send_data_buf[cb_id][39] = uart2_rxd_buf[10];
            bojung_flag = 0xff;
          }
          else
          {
            if(uart2_rxd_buf[9] == '0')
            {
              cb_send_data_buf[cb_id][39] = uart2_rxd_buf[10];
              bojung_flag |= 0x01;
              cb_first_send_save(cb_id,'$');
            }
            else if(uart2_rxd_buf[9] == '1')
            {
              cb_send_data_buf[cb_id][39] = uart2_rxd_buf[10];
              bojung_flag |= 0x02;
              cb_first_send_save(cb_id,'$');
            }
            else if(uart2_rxd_buf[9] == '2')
            {
              cb_send_data_buf[cb_id][39] = uart2_rxd_buf[10];
              bojung_flag |= 0x04;
              cb_first_send_save(cb_id,'$');
            }      
            else if(uart2_rxd_buf[9] == '3')
            {
              cb_send_data_buf[cb_id][39] = uart2_rxd_buf[10];
              bojung_flag |= 0x08;
              cb_first_send_save(cb_id,'$');
            }                 
          }
        }
      }
      break;                    //<= h@20140704      
      
      
//
//********** K Hotel(Domyin) Only *************
//      

    case 'N':   //DND on-line control
      if(uart2_rxd_buf[9] == '1') cb_send_data_buf[cb_id][33] |= 0x01;
      else if(uart2_rxd_buf[9] == '0') cb_send_data_buf[cb_id][33] &= 0xfe;
      cb_first_send_save(cb_id,'n');
      break;
    case 'O':   //MUR on-line control
      if(uart2_rxd_buf[9] == '1') cb_send_data_buf[cb_id][33] |= 0x02;
      else if(uart2_rxd_buf[9] == '0') cb_send_data_buf[cb_id][33] &= 0xfd;
      cb_first_send_save(cb_id,'o');
      break;
      
  }
}

void common_command_check(unsigned int group_id)
{
  unsigned char i,j;
  unsigned int  u16_temp;
  unsigned char temp_temp;
  unsigned char group_index;
  
  
  switch(uart2_rxd_buf[8]){
    case 'a':   //심야제어 설정
      if(uart2_rxd_buf[9] == '1'){
        for(i=0; i<cb_max_count[my_device_no]; ++i){
          cb_send_data_buf[i][2] |= 0x04;    //심야제어 ON
          Write_BKPSRAM(b_sram_check_io_base + (i*4), cb_send_data_buf[i][2]); 
        }
      }
      else{
        if(uart2_rxd_buf[9] == '0'){
          for(i=0; i<cb_max_count[my_device_no]; ++i){
            cb_send_data_buf[i][2] &= 0x0fb; //심야제어 OFF
            Write_BKPSRAM(b_sram_check_io_base + (i*4), cb_send_data_buf[i][2]); 
          }
        }
      }      
      break;
      
    case 'b':   //대기 일제온도 전체적용
      temp_temp = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      for(i=0; i<cb_max_count[my_device_no]; ++i){
        cb_send_data_buf[i][4] = temp_temp;
        cb_send_data_buf[i][5] = temp_temp;
        cb_send_data_buf[i][6] = temp_temp;
        cb_send_data_buf[i][7] = temp_temp;
        cb_send_data_buf[i][8] = temp_temp;
        cb_send_data_buf[i][9] = temp_temp;
        cb_send_data_buf[i][10] = temp_temp;
        cb_send_data_buf[i][11] = temp_temp;
        cb_send_data_buf[i][3] = 0xff;
      }
      common_data_buf[17] = temp_temp;
      cb_first_send_save(0xfe,'t');   //전체 전송 ID = 0xfe
      break;
    case 'c':   //바닥 일제온도 전체적용
      temp_temp = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      for(i=0; i<cb_max_count[my_device_no]; ++i){
        cb_send_data_buf[i][13] = temp_temp;
        cb_send_data_buf[i][14] = temp_temp;
        cb_send_data_buf[i][15] = temp_temp;
        cb_send_data_buf[i][16] = temp_temp;
        cb_send_data_buf[i][17] = temp_temp;
        cb_send_data_buf[i][18] = temp_temp;
        cb_send_data_buf[i][19] = temp_temp;
        cb_send_data_buf[i][20] = temp_temp;
        cb_send_data_buf[i][12] = 0xff;
      }
      common_data_buf[18] = temp_temp;
      cb_first_send_save(0xfe,'f');   //전체 전송 ID = 0xfe
      break;
      
    case 'd':   //대기 일제온도 공실만 적용
      temp_temp = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      for(i=0; i<cb_max_count[my_device_no]; ++i){
        if(!(cb_send_data_buf[i][2] & 0x01)){
          cb_send_data_buf[i][4] = temp_temp;
          cb_send_data_buf[i][5] = temp_temp;
          cb_send_data_buf[i][6] = temp_temp;
          cb_send_data_buf[i][7] = temp_temp;
          cb_send_data_buf[i][8] = temp_temp;
          cb_send_data_buf[i][9] = temp_temp;
          cb_send_data_buf[i][10] = temp_temp;
          cb_send_data_buf[i][11] = temp_temp;
          cb_send_data_buf[i][3] = 0xff;
          cb_first_send_save(i,'t');   //전체 전송 ID = 0xfe
        }
      }
      break;
    case 'e':   //바닥 일제온도 공실만 적용
      temp_temp = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      for(i=0; i<cb_max_count[my_device_no]; ++i){
        if(!(cb_send_data_buf[i][2] & 0x01)){
          cb_send_data_buf[i][13] = temp_temp;
          cb_send_data_buf[i][14] = temp_temp;
          cb_send_data_buf[i][15] = temp_temp;
          cb_send_data_buf[i][16] = temp_temp;
          cb_send_data_buf[i][17] = temp_temp;
          cb_send_data_buf[i][18] = temp_temp;
          cb_send_data_buf[i][19] = temp_temp;
          cb_send_data_buf[i][20] = temp_temp;
          cb_send_data_buf[i][12] = 0xff;
          cb_first_send_save(i,'f');   //전체 전송 ID = 0xfe
        }
      }
      break;
      
    case 'o':   //냉방,난방,일반환절기,난방환절기,냉방환절기
      
      if(common_data_buf[27] == 3) //층별제어 해제일때
      {
        for(i=0; i<cb_max_count[my_device_no]; ++i){
          cb_send_data_buf[i][2] &= 0x8f;
          cb_send_data_buf[i][2] |= ((uart2_rxd_buf[9] & 0x07) << 4);
          Write_BKPSRAM(b_sram_check_io_base + (i*4), cb_send_data_buf[i][2]); 
        }
      }
      break;
      
    case 'n':   //외출온도차
      common_data_buf[1] &= 0xf0;
      common_data_buf[1] |= (uart2_rxd_buf[9] & 0x0f);
      cb_first_send_save(0xfe,'P');   //공통 data send 
      Write_BKPSRAM(b_sram_global_data_base+(1*4),common_data_buf[1]); 
      break;
      
    case 'q':   //이상경보온도
      common_data_buf[2] = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      cb_first_send_save(0xfe,'P');   //공통 data send 
      Write_BKPSRAM(b_sram_global_data_base+(2*4),common_data_buf[2]); 
      break;
      
    case 'r':   //냉.난방 전환 온도차
      common_data_buf[1] &= 0x0f;
      common_data_buf[1] |= ((uart2_rxd_buf[9] & 0x0f) << 4);
      cb_first_send_save(0xfe,'P');   //공통 data send 
      Write_BKPSRAM(b_sram_global_data_base+(1*4),common_data_buf[1]); 
      break;
      
    case 'f':   //대기 공실 냉방 기준온도
//      common_data_buf[4] = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      common_data_buf[4] = ((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f);
      cb_first_send_save(0xfe,'P');   //공통 data send 
      Write_BKPSRAM(b_sram_global_data_base+(4*4),common_data_buf[4]); 
      break;
      
    case 'g':   //대기 공실 난방 기준온도
//      common_data_buf[3] = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      common_data_buf[3] = ((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f);
      cb_first_send_save(0xfe,'P');   //공통 data send 
      Write_BKPSRAM(b_sram_global_data_base+(3*4),common_data_buf[3]); 
      break;

    case 'h':   //바닥 공실 기준온도
//      common_data_buf[5] = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      common_data_buf[5] = ((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f);
      cb_first_send_save(0xfe,'P');   //공통 data send 
      Write_BKPSRAM(b_sram_global_data_base+(5*4),common_data_buf[5]); 
      break;
      
    case 'i':    //그룹공실온도(냉방)
      group_index = (unsigned char)(group_id & 0x00ff);
//      group_data_buf[group_index][0] = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      group_data_buf[group_index][0] = ((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f);
      cb_first_send_save(0xa0+group_index,'i');   //group data send 
      break;
      
    case 'j':    //그룹공실온도(난방)
      group_index = (unsigned char)(group_id & 0x00ff);
//      group_data_buf[group_index][1] = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      group_data_buf[group_index][1] = ((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f);
      cb_first_send_save(0xa0+group_index,'i');   //group data send 
      break;
      
    case 'k':    //그룹공실온도(바닥)
//      group_data_buf[group_index][2] = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)];
      group_index = (unsigned char)(group_id & 0x00ff);
      group_data_buf[group_index][2] = ((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f);
      cb_first_send_save(0xa0+group_index,'i');   //group data send 
      break;
      
      
    case 'l':   //대기 설정온도 상.하한
      i = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)]; //상한
      j = b_decimal_to_hex_table[((uart2_rxd_buf[11] & 0x0f) * 0x10) | (uart2_rxd_buf[12] & 0x0f)]; //하한
      if(((i >= 25) && (i <= 40)) && ((j >= 10) && (j <= 25))){
        common_data_buf[6] = ((i-25) * 0x10) | (25-j);
        cb_first_send_save(0xfe,'P');   //공통 data send 
      }
      Write_BKPSRAM(b_sram_global_data_base+(6*4),common_data_buf[6]); 
      break;
    case 'm':   //바닥 설정온도 상.하한
      i = b_decimal_to_hex_table[((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f)]; //상한
      j = b_decimal_to_hex_table[((uart2_rxd_buf[11] & 0x0f) * 0x10) | (uart2_rxd_buf[12] & 0x0f)]; //하한
      if(((i >= 25) && (i <= 40)) && ((j >= 10) && (j <= 25))){
        common_data_buf[7] = ((i-25) * 0x10) | (25-j);
        cb_first_send_save(0xfe,'P');   //공통 data send 
      }
      Write_BKPSRAM(b_sram_global_data_base+(7*4),common_data_buf[7]); 
      break;
      
    case 'p':   //시간 data
      common_data_buf[8] = ((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f);        //년
      common_data_buf[9] = ((uart2_rxd_buf[11] & 0x0f) * 0x10) | (uart2_rxd_buf[12] & 0x0f);        //월
      common_data_buf[10] = ((uart2_rxd_buf[13] & 0x0f) * 0x10) | (uart2_rxd_buf[14] & 0x0f);     //일
      common_data_buf[11] = ((uart2_rxd_buf[15] & 0x0f) * 0x10) | (uart2_rxd_buf[16] & 0x0f);     //시
      common_data_buf[12] = ((uart2_rxd_buf[17] & 0x0f) * 0x10) | (uart2_rxd_buf[18] & 0x0f);     //분
      cb_first_send_save(0xfe,'P');   //공통 data send
      clean_order_time_check();               
      break;
      
    case 'x':   //입구등 점등 유지시간
      u16_temp = hextbl100[uart2_rxd_buf[9]&0x0F];
      u16_temp += hextbl10[uart2_rxd_buf[10]&0x0F];
      u16_temp += hextbl1[uart2_rxd_buf[11]&0x0F];
      common_data_buf[13] = (u16_temp & 0xff00) >> 8;
      common_data_buf[14] = (u16_temp & 0x00ff);
      cb_first_send_save(0xfe,'P');   //공통 data send 
      Write_BKPSRAM(b_sram_global_data_base+(13*4),common_data_buf[13]); 
      Write_BKPSRAM(b_sram_global_data_base+(14*4),common_data_buf[14]); 
      break;
      
    case 'y':   //청소지시 시간
      common_data_buf[15] = ((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f);   //청소지시시간 시
      common_data_buf[16] = ((uart2_rxd_buf[11] & 0x0f) * 0x10) | (uart2_rxd_buf[12] & 0x0f);   //청소지시시간 분
      Write_BKPSRAM(b_sram_global_data_base+(15*4),common_data_buf[15]); 
      Write_BKPSRAM(b_sram_global_data_base+(16*4),common_data_buf[16]); 
      break;
      
    case 'I':   //Chime Volume Level
      common_data_buf[21] = uart2_rxd_buf[9];
      cb_first_send_save(0xfe,'I');
      Write_BKPSRAM(b_sram_global_data_base+(21*4),common_data_buf[21]); 
      break;
      
    case 'J':   //Alram Volume Level
      common_data_buf[22] = uart2_rxd_buf[9];
      cb_first_send_save(0xfe,'I');
      Write_BKPSRAM(b_sram_global_data_base+(22*4),common_data_buf[22]); 
      break;

    case 'z':   //조도 조절 시간 및 방법
      common_data_buf[23] = uart2_rxd_buf[9];
      cb_first_send_save(0xfe,'P');
      f_illumination_ok = 1;
      Write_BKPSRAM(b_sram_global_data_base+(23*4),common_data_buf[23]); 
      break;

    case 'K':   //온도센서 온도 표시 방법
      common_data_buf[24] = uart2_rxd_buf[9];
      cb_first_send_save(0xfe,'P');
      Write_BKPSRAM(b_sram_global_data_base+(24*4),common_data_buf[24]); 
      break;
      
    case 'Z':   //Power ON reset command?
      f_power_on_start = 0;
      break;
      
    case 'L':   //기준온도 대기/바닥(Check IN/OUT시 적용)  //h@20140711
      common_data_buf[25] = ((uart2_rxd_buf[9] & 0x0f) * 0x10) | (uart2_rxd_buf[10] & 0x0f);
      common_data_buf[26] = ((uart2_rxd_buf[11] & 0x0f) * 0x10) | (uart2_rxd_buf[12] & 0x0f);
      Write_BKPSRAM(b_sram_global_data_base+(25*4),common_data_buf[25]); 
      Write_BKPSRAM(b_sram_global_data_base+(26*4),common_data_buf[26]); 
      cb_first_send_save(0xfe,'P');
      break;
      
    case 'M': // 층별 냉,난방,환절기 제어
      
        common_data_buf[28] = uart2_rxd_buf[10]; //동코드            
        common_data_buf[29] = ((uart2_rxd_buf[11] & 0x0f) * 0x10 | (uart2_rxd_buf[12] & 0x0f)); //층코드       
        
        if(common_data_buf[28] == dong_code)
        {          
          if(common_data_buf[29] == my_device_id_check[my_device_no]) //층 ID CHECK
          {
            common_data_buf[27] = uart2_rxd_buf[9]; //냉,난방,환절기 DATA

            if(common_data_buf[27] != 3) //층별제어 해제아닌가?
            {
              for(i=0; i<cb_max_count[my_device_no]; ++i){
                cb_send_data_buf[i][2] &= 0x8f;
                cb_send_data_buf[i][2] |= ((uart2_rxd_buf[9] & 0x07) << 4);
                Write_BKPSRAM(b_sram_check_io_base + (i*4), cb_send_data_buf[i][2]);
                Write_BKPSRAM(b_sram_global_data_base+(27*4),common_data_buf[27]); 
                Write_BKPSRAM(b_sram_global_data_base+(28*4),common_data_buf[28]);
                Write_BKPSRAM(b_sram_global_data_base+(29*4),common_data_buf[29]);
              }          
            }
          }
        }
      break;
      
  }

}
  
void cb_first_send_save(unsigned char index, unsigned char save_cmd)
{
  if(++cb_first_save_point > 99) cb_first_save_point = 0;
  cb_first_save_buffer[cb_first_save_point][0] = index;
  cb_first_save_buffer[cb_first_save_point][1] = save_cmd;
}

void clean_order_time_check(void)
{
  unsigned char i;
  
  if(dip_function & 0x01){              //Dip switch 2번에 7번 ON이면 청소지시 시간 check     hhh@20160222
    if((common_data_buf[11] == common_data_buf[15]) && (common_data_buf[12] == common_data_buf[16]))
    {
      for(i=0; i<cb_max_count[my_device_no]; ++i)
      {
        if(cb_send_data_buf[i][2] & 0x01) //check in 인가?
        { 
          if(cb_rx_room_data_buf[i][61] & 0x02) // 재실 이었나?
          {
           cb_rx_room_data_buf[i][60] = 0x03;  //청소지시 선언
           cb_rx_room_data_buf[i][61] &= 0xfd; //재실 Flag clear    
           Write_BKPSRAM(b_sram_clean_base + (i*4), cb_rx_room_data_buf[i][60]);
          }
        }
      }
    }
  }  
  
}

/******************************************************************************************************/
/* UART3_initial :                                                                                    */ 											
/******************************************************************************************************/
void UART2_initial(void){
    
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
    
    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
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
    
    USART_Init(USART2, &USART_InitStructure);
    
      /* Enable USART */
    USART_Cmd(USART2, ENABLE);    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    //
    // Install USART1 Handler with preemtion level one above lowest level to ensure communication
    //
    
    // Enable the USART2 Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
}



//=======================================================================
//=======================================================================

