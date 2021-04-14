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
  unsigned int  temp_temp;
  unsigned int  id_decimal;
  
  f_sign_pc_tx = 1;
  sign_led_pc_tx = 0;  

  
  temp_temp = priority_pc_send_check();
  if(temp_temp == 0xffff){
    if(++cb_send_id >= cb_max_count) cb_send_id = 0;
    temp_temp = cb_send_id;
  }
  else{
    P_led1 ^= 1;
    if(temp_temp > cb_max_count) temp_temp = 0;
  }
  temp_id = b_cb_send_id_table[0][temp_temp];
  id_decimal = hex2dec(temp_id);
  P_u2dir = 1;
  
  if(cb_rx_room_data_buf[temp_temp][0] == 'R'){
  
  //  uart2_tx_data_buf[i++] = ' ';                                               //Preamble
    uart2_tx_data_buf[i++] = '{';                                                 //STX
    ++i;                                                                          //Length
    uart2_tx_data_buf[i++] = ((id_decimal & 0xf000) >> 12) | 0x30;                //ID
    uart2_tx_data_buf[i++] = ((id_decimal & 0x0f00) >> 8) | 0x30;                 //ID
    uart2_tx_data_buf[i++] = ((id_decimal & 0x00f0) >> 4) | 0x30;                 //ID
    uart2_tx_data_buf[i++] = ((id_decimal & 0x000f) >> 0) | 0x30;                 //ID
    uart2_tx_data_buf[i++] = 'S';                                                 //Standard Command
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][1]; 
    uart2_tx_data_buf[i++] = (cb_rx_room_data_buf[temp_temp][2] & 0x0f) | 0x30;   //Key status
    uart2_tx_data_buf[i++] = (cb_rx_room_data_buf[temp_temp][60] & 0x0f) | 0x30;  //Clean status
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][3];
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][3];
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][4]; // �µ����� ��Ż��� 0x80 �߰� (20.11.19)
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][5];
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][6];
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][7];
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][12];
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][13];
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][14];
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][15];
    
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,20);                   //TS1 Set temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,21);                   //TS2 Set temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,22);                   //TS3 Set temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,23);                   //TS4 Set temp
    
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,28);                   //TS1 check temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,29);                   //TS2 check temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,30);                   //TS3 check temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,31);                   //TS4 check temp
    
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,36);                   //TS1 Floor  temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,37);                   //TS2 Floor  temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,38);                   //TS3 Floor  temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,39);                   //TS4 Floor  temp
    
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,44);                   //TS1 Floor check temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,45);                   //TS2 Floor check temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,46);                   //TS3 Floor check temp
    uart2_tx_data_buf[i++] = temp_dec_conv_1byte(temp_temp,47);                   //TS4 Floor check temp
    
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][53];
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][62];                  //Service change(Bit ó��)
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][64];                //Inspector���� ��� �� 
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][65];                //Check out ��û �� 
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][66];                //����Ʈ ����ġ ��ġ ����
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][67];                //����Ʈ ����ġ (1) ��� ����
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][68];                //����Ʈ ����ġ (2) ��� ����
    uart2_tx_data_buf[i++] = cb_rx_room_data_buf[temp_temp][69];                //NT, CI, KS ��� �� ��� ���� 

    
  }
  else{
    uart2_tx_data_buf[i++] = '{';                                                 //STX
    ++i;                                                                          //Length
    uart2_tx_data_buf[i++] = '?';                //ID
    uart2_tx_data_buf[i++] = '?';                 //ID
    uart2_tx_data_buf[i++] = '?';                 //ID
    uart2_tx_data_buf[i++] = '?';                 //ID
    uart2_tx_data_buf[i++] = 'B';                                                 //Busy Command
  }
  
  uart2_tx_data_buf[1] = i+1;
  for(j=0; j<i; ++j) crc += uart2_tx_data_buf[j];
  crc &= 0x7f;
  crc |= 0x40;
  uart2_tx_data_buf[i++] = crc;                                                 //CRC
  uart2_tx_data_buf[i++] = '}';                                                 //ETX
  f_uart2_data_send = 1;
  uart2_tx_length = 0;
  uart2_tx_backup = i;

}

unsigned char temp_dec_conv_1byte(unsigned int cb_index, unsigned char index)
{
  unsigned char return_data;

  return_data = (cb_rx_room_data_buf[cb_index][index] & 0x7f);
  if(cb_rx_room_data_buf[cb_index][index] & 0x80) return_data |= 0x80;
  return(return_data);
  
}



unsigned int priority_pc_send_check(void)
{
  if(priority_pc_send_save_point != priority_pc_send_check_point){
    if(++priority_pc_send_check_point > 799) priority_pc_send_check_point = 0;
//    ++priority_pc_send_check_point;
    return(priority_pc_send_buffer[priority_pc_send_check_point]);
  }
  else return(0xffff);
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
  
//  unsigned char rx_crc=0;
//  unsigned char i;
  unsigned int cb_rx_id;
  
  if(f_uart2_frame_rx_ok){
    f_uart2_frame_rx_ok = 0;

    f_sign_pc_rx = 1;
    sign_led_pc_rx = 0;
    
      
//    for(i=0; i<uart2_rxd_buf[1]-2; ++i) rx_crc += uart2_rxd_buf[i];
//    rx_crc ^= 0x55;
//    if(rx_crc == (uart2_rxd_buf[uart2_rxd_buf[1]-2])){
    
      cb_rx_id = hextbl1000[uart2_rxd_buf[1]&0x0F];
      cb_rx_id += hextbl100[uart2_rxd_buf[2]&0x0F];
      cb_rx_id += hextbl10[uart2_rxd_buf[3]&0x0F];
      cb_rx_id += hextbl1[uart2_rxd_buf[4]&0x0F];
      
      if(cb_rx_id != 0xffff){
        if(cb_rx_id < cb_max_count){
          command_check(cb_rx_id);           //���� command check
        }
      }
      else  common_command_check();     //���� command check
      P_led7++;
      uart2_data_send();

  }  
}


void command_check(unsigned int cb_id)
{
  unsigned char temp_temp = 0;
  
  switch(uart2_rxd_buf[5]){
    case 'F':   //û������?
      if((uart2_rxd_buf[6] == '3') || (uart2_rxd_buf[6] == '2')){
        cb_send_data_buf[cb_id][28] = uart2_rxd_buf[6]-2;
        cb_first_send_save(cb_id,'F');
      }
      break;
      
    case 'A':   //Check IN/OUT
      cb_send_data_buf[cb_id][2] = uart2_rxd_buf[6];
      if(uart2_rxd_buf[8] != ')'){
        cb_send_data_buf[cb_id][31] = b_decimal_to_hex_table[(((uart2_rxd_buf[7] & 0x0f) * 0x10) + (uart2_rxd_buf[8] & 0x0f))];
      }
      else cb_send_data_buf[cb_id][31] = 0xff;    //Group No ������
      cb_first_send_save(cb_id,'A');
      break;
      
    case 'B':   //Message ON/OFF
      cb_send_data_buf[cb_id][3] = uart2_rxd_buf[6];
      cb_first_send_save(cb_id,'B');
      break;
      
    case 'C':   //����.����
      cb_send_data_buf[cb_id][4] = uart2_rxd_buf[6];
      cb_first_send_save(cb_id,'C');
      break;
      
    case 'D':   //��� �����µ�
      if(uart2_rxd_buf[6] != '?'){
        temp_temp = ((uart2_rxd_buf[7] & 0x0f) * 0x10) | (uart2_rxd_buf[8] & 0x0f);
        cb_send_data_buf[cb_id][(uart2_rxd_buf[6] & 0x0f)+7] = temp_temp;
        cb_send_data_buf[cb_id][6] |= (0x01 << (uart2_rxd_buf[6] & 0x0f));
        cb_first_send_save(cb_id,'D');
      }
      else{     //��� ��ü
        temp_temp = ((uart2_rxd_buf[7] & 0x0f) * 0x10) | (uart2_rxd_buf[8] & 0x0f);
        cb_send_data_buf[cb_id][7] = temp_temp;
        cb_send_data_buf[cb_id][8] = temp_temp;
        cb_send_data_buf[cb_id][9] = temp_temp;
        cb_send_data_buf[cb_id][10] = temp_temp;
        cb_send_data_buf[cb_id][11] = temp_temp;
        cb_send_data_buf[cb_id][12] = temp_temp;
        cb_send_data_buf[cb_id][13] = temp_temp;
        cb_send_data_buf[cb_id][14] = temp_temp;
        cb_send_data_buf[cb_id][6] = 0xff;
        cb_first_send_save(cb_id,'D');
      }
      break;
    case 'E':   //�ٴ� �����µ�
      if(uart2_rxd_buf[6] != '?'){
        temp_temp = ((uart2_rxd_buf[7] & 0x0f) * 0x10) | (uart2_rxd_buf[8] & 0x0f);
        cb_send_data_buf[cb_id][(uart2_rxd_buf[6] & 0x0f)+16] = temp_temp;
        cb_send_data_buf[cb_id][15] |= (0x01 << (uart2_rxd_buf[6] & 0x0f));
        cb_first_send_save(cb_id,'E');
      }
      else{
        temp_temp = ((uart2_rxd_buf[7] & 0x0f) * 0x10) | (uart2_rxd_buf[8] & 0x0f);
        cb_send_data_buf[cb_id][16] = temp_temp;
        cb_send_data_buf[cb_id][17] = temp_temp;
        cb_send_data_buf[cb_id][18] = temp_temp;
        cb_send_data_buf[cb_id][19] = temp_temp;
        cb_send_data_buf[cb_id][20] = temp_temp;
        cb_send_data_buf[cb_id][21] = temp_temp;
        cb_send_data_buf[cb_id][22] = temp_temp;
        cb_send_data_buf[cb_id][23] = temp_temp;
        cb_send_data_buf[cb_id][15] = 0xff;
        cb_first_send_save(cb_id,'E');
      }
      break;
      
      
    case 'G':   //��� �µ����� ON/OFF��������
      if(uart2_rxd_buf[6] != '?'){
        cb_send_data_buf[cb_id][24] |= (0x01 << (uart2_rxd_buf[6] & 0x0f));
        if(uart2_rxd_buf[7] == '0') cb_send_data_buf[cb_id][25] &= (~(0x01 << (uart2_rxd_buf[6] & 0x0f)));
        else{
          if(uart2_rxd_buf[7] == '1') cb_send_data_buf[cb_id][25] |= (0x01 << (uart2_rxd_buf[6] & 0x0f));
        }
        cb_first_send_save(cb_id,'G');
      }
      else{
        cb_send_data_buf[cb_id][24] = 0xff;
        if(uart2_rxd_buf[7] == '0') cb_send_data_buf[cb_id][25] = 0x00;
        else{
          if(uart2_rxd_buf[7] == '1') cb_send_data_buf[cb_id][25] = 0xff;
        }
        cb_first_send_save(cb_id,'G');
      }
      break;
      
    case 'H':   //�ٴ� �µ����� ON/OFF��������
      if(uart2_rxd_buf[6] != '?'){
        cb_send_data_buf[cb_id][26] |= (0x01 << (uart2_rxd_buf[6] & 0x0f));
        if(uart2_rxd_buf[7] == '0') cb_send_data_buf[cb_id][27] &= (~(0x01 << (uart2_rxd_buf[6] & 0x0f)));
        else{
          if(uart2_rxd_buf[7] == '1') cb_send_data_buf[cb_id][27] |= (0x01 << (uart2_rxd_buf[6] & 0x0f));
        }
        cb_first_send_save(cb_id,'H');
      }
      else{
        cb_send_data_buf[cb_id][26] = 0xff;
        if(uart2_rxd_buf[7] == '0') cb_send_data_buf[cb_id][27] = 0x00;
        else{
          if(uart2_rxd_buf[7] == '1') cb_send_data_buf[cb_id][27] = 0xff;
        }
        cb_first_send_save(cb_id,'H');
      }
      break;
      
    case 'I':   //���� Chime Digital volume Level
      cb_send_data_buf[cb_id][29] = uart2_rxd_buf[6];
      
      switch(cb_send_data_buf[cb_id][29])
      {
        case 0x01:         // 1�ܰ�
          cb_send_data_buf[cb_id][29] = 0x00;    
        break;
        
        case 0x05:         // 2�ܰ�
         cb_send_data_buf[cb_id][29] = 0x01;    
        break;
        
        case 0x0f:         // 3�ܰ�
          cb_send_data_buf[cb_id][29] = 0x02;
        break;
        
        case 0x25:       // 4�ܰ�
          cb_send_data_buf[cb_id][29] = 0x03;
        break;
        
        case 0x45:       // 5�ܰ�
          cb_send_data_buf[cb_id][29] = 0x05;
        break;
        
        case 0x65:      // 6�ܰ�
          cb_send_data_buf[cb_id][29] = 0x08;
        break;
        
        case 0x85:      // 7�ܰ�
          cb_send_data_buf[cb_id][29] = 0x0c;
        break;
        
        case 0xa5:      // 8�ܰ�
          cb_send_data_buf[cb_id][29] = 0x0f;
        break;
        
        case 0xc5:      // 9�ܰ�
          cb_send_data_buf[cb_id][29] = 0x15;
        break;
        
        case 0xfe:      // 10�ܰ�
          cb_send_data_buf[cb_id][29] = 0x40;
        break;
 
      }
      
      cb_first_send_save(cb_id,'I');
      break;
      
    case 'J':   //���� Alram Digital volume Level
      cb_send_data_buf[cb_id][30] = uart2_rxd_buf[6];
      cb_first_send_save(cb_id,'J');
      break;
      
    case 'M':   //ȥ������
      cb_send_data_buf[cb_id][34] = uart2_rxd_buf[6];
      cb_send_data_buf[cb_id][35] = uart2_rxd_buf[7];
      cb_send_data_buf[cb_id][36] = uart2_rxd_buf[8];
      cb_send_data_buf[cb_id][37] = uart2_rxd_buf[9];
      cb_send_data_buf[cb_id][38] = uart2_rxd_buf[10];
      cb_first_send_save(cb_id,'M');
      break;
/*      
    case 'N':   //DND ON/OFF
      if(uart2_rxd_buf[6] == '1') cb_send_data_buf[cb_id][33] |= 0x01;
      else if(uart2_rxd_buf[6] == '0') cb_send_data_buf[cb_id][33] &= 0xfe;
      cb_first_send_save(cb_id,'N');
      break;
      
    case 'O':   //MUR ON/OFF
      if(uart2_rxd_buf[6] == '1') cb_send_data_buf[cb_id][33] |= 0x02;
      else if(uart2_rxd_buf[6] == '0') cb_send_data_buf[cb_id][33] &= 0xfd;
      cb_first_send_save(cb_id,'O');
      break;
      
    case 'P':   //TOWEL Change ON/OFF
      if(uart2_rxd_buf[6] == '1') cb_send_data_buf[cb_id][33] |= 0x04;
      else if(uart2_rxd_buf[6] == '0') cb_send_data_buf[cb_id][33] &= 0xfb;
      cb_first_send_save(cb_id,'P');
      break;
      */
      
    case '$':   //�����µ�      //h@20140704 =>
      if(((uart2_rxd_buf[6] >= '0') && (uart2_rxd_buf[6] <= '7')) || (uart2_rxd_buf[6] == '?')){
        temp_temp = (uart2_rxd_buf[8] & 0x0f);
        if((uart2_rxd_buf[7] == '+') || (uart2_rxd_buf[7] == '-')){
          if(uart2_rxd_buf[7] == '+') temp_temp &= 0x0f;
          else if(uart2_rxd_buf[7] == '-') temp_temp |= 0xf0;
          
          if(temp_temp != 0xff){
            if(uart2_rxd_buf[6] == '?'){
              cb_send_data_buf[cb_id][39] = temp_temp;
              bojung_flag = 0xff;
            }
            else{
              if(uart2_rxd_buf[6] == '0'){
                cb_send_data_buf[cb_id][39] = temp_temp;
                bojung_flag |= 0x01;
                cb_first_send_save(cb_id,'$');
              }
              else if(uart2_rxd_buf[6] == '1'){
                cb_send_data_buf[cb_id][39] = temp_temp;
                bojung_flag |= 0x02;
                cb_first_send_save(cb_id,'$');
              }
              else if(uart2_rxd_buf[6] == '2'){
                cb_send_data_buf[cb_id][39] = temp_temp;
                bojung_flag |= 0x04;
                cb_first_send_save(cb_id,'$');
              }         
              else if(uart2_rxd_buf[6] == '3'){
                cb_send_data_buf[cb_id][39] = temp_temp;
                bojung_flag |= 0x08;
                cb_first_send_save(cb_id,'$');                
              }
              
            }
          }
        }
      }
      break;                    //<= h@20140704
      
      
  }
}

void common_command_check(void)
{
  unsigned char group_index;
  unsigned char group_hex;
  
  switch(uart2_rxd_buf[5]){
    case 'a':   //�ɾ����� ����
      common_data_buf[26] = uart2_rxd_buf[6];
      cb_first_send_save(0xffff,'a');
      break;
      
    case 'b':   //��� �����µ� ��ü����
      common_data_buf[22] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      cb_first_send_save(0xffff,'b');
      break;
      
    case 'c':   //�ٴ� �����µ� ��ü����
      common_data_buf[23] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      cb_first_send_save(0xffff,'c');
      break;
      
    case 'd':   //��� �����µ� ���Ǹ� ����
      common_data_buf[24] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      cb_first_send_save(0xffff,'d');
      break;
      
    case 'e':   //�ٴ� �����µ� ���Ǹ� ����
      common_data_buf[25] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      cb_first_send_save(0xffff,'e');
      break;
      
    case 'o':   //�ù�,����,�Ϲ�ȯ����,����ȯ����,�ù�ȯ����
      common_data_buf[27] = uart2_rxd_buf[6];
      cb_first_send_save(0xffff,'o');
      break;
      
    case 'n':   //����µ���
      common_data_buf[1] = uart2_rxd_buf[6];
      cb_first_send_save(0xffff,'n');
      break;
      
    case 'q':   //�̻�溸�µ�
      common_data_buf[2] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      cb_first_send_save(0xffff,'q');
      break;
      
    case 'r':   //��.���� ��ȯ �µ���
      common_data_buf[10] = uart2_rxd_buf[6];
      cb_first_send_save(0xffff,'r');
      break;
      
    case 'f':   //��� ���� �ù� ���ؿµ�
      common_data_buf[4] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      cb_first_send_save(0xffff,'f');
      break;
      
    case 'g':   //��� ���� ���� ���ؿµ�
      common_data_buf[3] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      cb_first_send_save(0xffff,'g');
      break;
      
    case 'h':   //�ٴ� ���� ���ؿµ�
      common_data_buf[5] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      cb_first_send_save(0xffff,'h');
      break;
      
    case 'i':   //�׷���Ǳ��ؿµ�(�ù�)
      group_index = b_decimal_to_hex_table[((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f)];
      group_hex = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      if(group_index <= 90){
        group_data_buf[group_index][0] = ((uart2_rxd_buf[8] & 0x0f) * 0x10) | (uart2_rxd_buf[9] & 0x0f);
        cb_first_send_save(0xff00 + group_hex,'i');
      }
      break;
      
    case 'j':   //�׷���Ǳ��ؿµ�(����)
      group_index = b_decimal_to_hex_table[((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f)];
      group_hex = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      if(group_index <= 90){
        group_data_buf[group_index][1] = ((uart2_rxd_buf[8] & 0x0f) * 0x10) | (uart2_rxd_buf[9] & 0x0f);
        cb_first_send_save(0xff00 + group_hex,'j');
      }
      break;
      
    case 'k':   //�׷���Ǳ��ؿµ�(�ٴ�)
      group_index = b_decimal_to_hex_table[((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f)];
      group_hex = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      if(group_index <= 90){
        group_data_buf[group_index][2] = ((uart2_rxd_buf[8] & 0x0f) * 0x10) | (uart2_rxd_buf[9] & 0x0f);
        cb_first_send_save(0xff00 + group_hex,'k');
      }
      break;
      
    case 'l':   //��� �����µ� ��.����
      common_data_buf[6] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      common_data_buf[7] = ((uart2_rxd_buf[8] & 0x0f) * 0x10) | (uart2_rxd_buf[9] & 0x0f);
      cb_first_send_save(0xffff,'l');
      break;
    case 'm':   //�ٴ� �����µ� ��.����
      common_data_buf[8] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);
      common_data_buf[9] = ((uart2_rxd_buf[8] & 0x0f) * 0x10) | (uart2_rxd_buf[9] & 0x0f);
      cb_first_send_save(0xffff,'m');
      break;
      
    case 'p':   //�ð� data
      common_data_buf[13] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);        //��
      common_data_buf[14] = ((uart2_rxd_buf[8] & 0x0f) * 0x10) | (uart2_rxd_buf[9] & 0x0f);        //��
      common_data_buf[15] = ((uart2_rxd_buf[10] & 0x0f) * 0x10) | (uart2_rxd_buf[11] & 0x0f);     //��
      common_data_buf[16] = ((uart2_rxd_buf[12] & 0x0f) * 0x10) | (uart2_rxd_buf[13] & 0x0f);     //��
      common_data_buf[17] = ((uart2_rxd_buf[14] & 0x0f) * 0x10) | (uart2_rxd_buf[15] & 0x0f);     //��
      
      if((common_data_buf[17] >= 0x00) && (common_data_buf[17] <= 0x05)) //0~5�� ���÷� ���� (5ȸ)
      {
        common_data_buf[17] = 0;
      }
      
      cb_first_send_save(0xffff,'p');   //���� data send 
      break;
      
    case 'x':   //�Ա��� ���� �����ð�
      common_data_buf[18] = uart2_rxd_buf[6];
      common_data_buf[19] = ((uart2_rxd_buf[7] & 0x0f) << 4) | (uart2_rxd_buf[8] & 0x0f);
      cb_first_send_save(0xffff,'x');   //���� data send 
      break;
      
    case 'y':   //û������ �ð�
      common_data_buf[20] = ((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f);   //û�����ýð� ��
      common_data_buf[21] = ((uart2_rxd_buf[8] & 0x0f) * 0x10) | (uart2_rxd_buf[9] & 0x0f);   //û�����ýð� ��
      cb_first_send_save(0xffff,'y');   //���� data send 
      break;
      
    case 'I':   //��ü Chime Digital volume Level
      common_data_buf[28] = uart2_rxd_buf[6];
      
      switch(common_data_buf[28])
      {
        case 0x01:         // 1�ܰ�
          common_data_buf[28] = 0x00;    
        break;
        
        case 0x05:         // 2�ܰ�
          common_data_buf[28] = 0x01;    
        break;
        
        case 0x0f:         // 3�ܰ�
          common_data_buf[28] = 0x02;
        break;
        
        case 0x25:       // 4�ܰ�
          common_data_buf[28] = 0x03;
        break;
        
        case 0x45:       // 5�ܰ�
          common_data_buf[28] = 0x05;
        break;
        
        case 0x65:      // 6�ܰ�
          common_data_buf[28] = 0x08;
        break;
        
        case 0x85:      // 7�ܰ�
          common_data_buf[28] = 0x0c;
        break;
        
        case 0xa5:      // 8�ܰ�
          common_data_buf[28] = 0x0f;
        break;
        
        case 0xc5:      // 9�ܰ�
          common_data_buf[28] = 0x15;
        break;
        
        case 0xfe:      // 10�ܰ�
          common_data_buf[28] = 0x40;
        break;
 
      }
      cb_first_send_save(0xffff,'I');   //���� data send 
      break;
      
    case 'J':   //��ü Alram Digital volume Level
      common_data_buf[29] = uart2_rxd_buf[6];
      cb_first_send_save(0xffff,'J');   //���� data send 
      break;
      
    case 'z':   //���� ���� �ð� �� ���
      common_data_buf[30] = uart2_rxd_buf[6];
      //common_data_buf[30] = 0x82;
      cb_first_send_save(0xffff,'z');   //���� data send 
      break;
      
    case 'K':   //�µ����� �µ� ǥ�� ��� (1=�����µ�, 0=����µ�)
      common_data_buf[31] = uart2_rxd_buf[6];
      cb_first_send_save(0xffff,'K');   //���� data send 
      break;
      
    case 'L':   //���ؿµ� ���/�ٴ�(Check IN/OUT �� �����)    //h@20140711
      common_data_buf[32] = (((uart2_rxd_buf[6] & 0x0f) * 0x10) | (uart2_rxd_buf[7] & 0x0f));   //��� ���ؿµ�
      common_data_buf[33] = (((uart2_rxd_buf[8] & 0x0f) * 0x10) | (uart2_rxd_buf[9] & 0x0f));   //�ٴ� ���ؿµ�
      cb_first_send_save(0xffff,'Q');   //���� data send 
      break;
      
    case 'M':   //���� ���� �ù�,����,ȯ���� ����
      common_data_buf[41] = uart2_rxd_buf[6]; //�ù�,����,ȯ���� data 
      common_data_buf[42] = uart2_rxd_buf[7]; //�� �ڵ� 
      common_data_buf[43] = uart2_rxd_buf[8]; //�� �ڵ� 1 
      common_data_buf[44] = uart2_rxd_buf[9]; //�� �ڵ� 2 
      cb_first_send_save(0xffff,'M');
      break;      
      
  }

}

void cb_first_send_save(unsigned int index, unsigned char save_cmd)
{
  if(++cb_first_save_point > 799) cb_first_save_point = 0;
  cb_first_save_buffer[cb_first_save_point][0] = index;
  cb_first_save_buffer[cb_first_save_point][1] = save_cmd;
}

void clean_order_time_check(void)
{
  unsigned int i;
  
  if((common_data_buf[11] == common_data_buf[16]) && (common_data_buf[12] == common_data_buf[17]))
  {
    for(i=0; i<cb_max_count; ++i)
    {
      if(cb_rx_room_data_buf[i][61] & 0x02){   //����̾�����?
        cb_rx_room_data_buf[i][60] = 0x03;     //û������ ����
        cb_rx_room_data_buf[i][61] &= 0xfd;    //��� Flag clear
      }
    }
  }
}


//========== Hex to Decimal convertor routine ============
unsigned int data;
unsigned int hex2dec(unsigned int decimal)
{
 	data = dectbl1000[(unsigned char)(decimal>>12) &0x03];
 	data = decimal_adder(dectbl100[(unsigned char)(decimal>>8) &0x0f]);
	data = decimal_adder(dectbl10[(unsigned char)decimal>>4 &0x0f]);
	data = decimal_adder(dectbl1[(unsigned char)(decimal) &0x0f]);
	return data;
}
unsigned int decimal_adder(unsigned int other_data)
{
unsigned int Sum;
unsigned char ix, cflag,ss[4];

 	cflag =0;
	for(ix=0; ix<4; ix++) {
		Sum =(other_data &0x000f) + (data &0x000f) + cflag;
		if(Sum >=0x0a || Sum >=0x10) Sum +=6, cflag =1;
		else cflag =0;
 		ss[ix] =Sum & 0x0f;
		other_data>>=4;		data>>=4;
 	}
	return (unsigned int)(ss[1]<<4 |ss[0]) | (unsigned int)(ss[3]<<4 |ss[2]) <<8;
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

