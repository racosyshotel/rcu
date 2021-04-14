/*=====================(C) COPYRIGHT 2014 PLUS-H.=========================
program		    : CONTROL BOX(RCS-300-RCU)
processor 	  : STM32F103xx
compiler		  : IAR 6.2A Compiler 
program BY	  : H.H.Hwang
date			    : 2014.	 .
copy right	  : P L U S  -  H. 
============================================================================*/
#define __STM32F10X_INIT_H__

#include "stm32f10x_lib.h"
#include "main.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static volatile ErrorStatus HSEStartUpStatus = SUCCESS;

/******************************************************************************************************/
/* Internal_timer_Proc : ���� tmxfg�� �׻� "1" �̴�	(Timer_setup �� tmxg�� "0"���� �����)	      */ 											
/******************************************************************************************************/
void Internal_timer_Proc() {				
u8 ix;


	if(tm1ms_f) { tm1ms_f =0;	//every 1msec 

      if(!tm0fg) {	if(!--timer_buf[0]) tm0fg =1;		}		
      if(!tm1fg) {	if(!--timer_buf[1]) tm1fg =1;		}
		  if(!tm2fg) {	if(!--timer_buf[2]) tm2fg =1;		}
      if(!tm3fg) {	if(!--timer_buf[3]) tm3fg =1;		}
      if(!tm4fg) {	if(!--timer_buf[4]) tm4fg =1;		}
      if(!tm5fg) {	if(!--timer_buf[5]) tm5fg =1;		}
      if(!tm6fg) {	if(!--timer_buf[6]) tm6fg =1;		}
      if(!tm7fg) {	if(!--timer_buf[7]) tm7fg =1;		} 

      if(!tm11fg) {	if(!--timer_buf[11]) tm11fg =1;		} 
      if(!tm12fg) {	if(!--timer_buf[12]) tm12fg =1;		} 

		for(ix=0; ix<sizeof(Extm.buf); ix++) {
			if(Extm.buf[ix]) Extm.buf[ix]--;
		}

		watchdog.s.b0 =1;
    
    ++key_sensor_chet;      //Key sensor chetering time
    if(++fcu_action_time[0] > 10000){ 
      fcu_action_time[0] = 0; 
      flag_fcu_action_time[0] = 1; 
    }
    if(++fcu_action_time[1] > 10000){ fcu_action_time[1] = 0; flag_fcu_action_time[1] = 1;  }
    if(++fcu_action_time[2] > 10000){ fcu_action_time[2] = 0; flag_fcu_action_time[2] = 1;  }
    if(++fcu_action_time[3] > 10000){ fcu_action_time[3] = 0; flag_fcu_action_time[3] = 1;  }
    
    if(++floor_action_time[0] > 10000){ floor_action_time[0] = 0; flag_floor_action_time[0] = 1; }
    if(++floor_action_time[1] > 10000){ floor_action_time[1] = 0; flag_floor_action_time[1] = 1; }
    if(++floor_action_time[2] > 10000){ floor_action_time[2] = 0; flag_floor_action_time[2] = 1; }
    if(++floor_action_time[3] > 10000){ floor_action_time[3] = 0; flag_floor_action_time[3] = 1; }
   
      
    

    
    if(++timer_1sec > 50){ 
      timer_1sec = 0;  
      led1 ^= 1;                    //PCB LED RUN Indicator
      dip_switch_read();
      eeprom_save_check();
    }                       
    
    if(++power_reset_timer > 1000) // 1sec
    {
        power_reset_timer = 2000; //2sec
        
        if(++sub_call_timer > 20){ sub_call_timer = 0; f_uart2_send_time = 1; }    //SUB call Timer
    }
    
    if(room_data_buf[40] & 0x02){   //�Խ��ΰ�?  
      P_ks_led = 1;                 //�Խ��� ��� KS�� LED�� �ҵ��Ѵ�.
      if(++ind_timer > 500){
        ind_timer = 0;
//        P_ind_led ^= 1 ;            //�Խ��� ��� �Ա� Indicator�� 0.5�� ������ �����Ѵ�.
      }
    }
    else{
//      P_ind_led = 0;                //����� ��� �Ա� Indicator�� LED�� �ҵ��Ѵ�.
      if(++ind_timer > 500){
        ind_timer = 0;
        P_ks_led ^= 1 ;            //����� ��� KS�� LED�� 0.5�� ������ �����Ѵ�.
      }
    }
    
    if(f_dnd_sign)
    {
        if(++dnd_count>150)
        {
            dnd_count = 0;
            f_dnd_ok = 1;
        }
    }      
    
    if(f_chime_sign)
    {
      if(++handy_sign_timer_2 > 10000)
      {
        handy_sign_timer_2 = 0;
        f_chime_sign = 0;
        
        room_data_buf[48] &= 0x7f;
      }
    }
    else handy_sign_timer_2 = 0;
    
    
      if(room_data_buf[48] & 0x80) //�湮�ھ˸���10�ʰ� ������
      {
            if(++guest_alrm_timer_1 > 100)
            {
                guest_alrm_timer_1 = 0;
                P_ex_led2 = 0;
                P_ex_led3 = 0;
            }
            else if(++guest_alrm_timer_2 > 400)
            {
                guest_alrm_timer_2 = 0;
                P_ex_led2 = 1;
                P_ex_led3 = 1;
            }
      }
      else
      {
          guest_alrm_timer_1 = 0;
          guest_alrm_timer_2 = 0;
          P_ex_led2 = 1;
          P_ex_led3 = 1;
      }
    
    
    if(!f_enterance_lamp_delay){
      if(++timer_1sec_entry > 1000){
        timer_1sec_entry = 0;
        if(++enterance_lamp_delay_timer > entry_lamp_delay_time){   //1min
          enterance_lamp_delay_timer = 0;
          f_enterance_lamp_delay = 1;
          //room_data_buf[45] &= 0xfd;    //�Ա��� �ҵ�  
        }
      }
    }

    if(!flag_relay_all_off_delay_timer){
      if(++relay_all_off_delay_timer > 10000){    //realy all off delay
        relay_all_off_delay_timer = 0;
        flag_relay_all_off_delay_timer = 1;
      }
    }
    
    if(!f_chime_sq_timer){
      if(++chime_sq_timer > chime_sq_comp) f_chime_sq_timer = 1;
    }
    chime_execution();
    
    if(f_handy_sign){
      if(++handy_sign_timer > 20000){
        handy_sign_timer = 0;
        room_data_buf[46] &= 0x7f;
        f_handy_sign = 0;
      }
    }
    
    
     if(dip_switch_buf[0] & 0x40){    //����� Key sensor�϶� �����ð� ������ �ȵǸ� ��� ó�� �Ѵ�.
       if(++ks_rx_timer > 5000){
         ks_rx_timer = 0;
         f_ks_error = 1;
       } 
     }
     
     if(!f_common_send_time){
       if(++common_send_time > 10000){
         common_send_time = 0;
         f_common_send_time = 1;
       }
     }
     
     if(f_check_temp_change){ //AIR
       if(++check_temp_send_time > 10000){
         f_check_temp_change = 0;
         check_temp_send_time = 0;
         room_data_buf[99] |= 0x02;   //temp data send 
       }
     }
     
     if(f_check_temp_change_2){ //FLOOR
       if(++check_temp_send_time_2 > 10000){
         f_check_temp_change_2 = 0;
         check_temp_send_time_2 = 0;
         room_data_buf[99] |= 0x04;   //temp data send 
       }
     }     

     if(room_data_buf[40] & 0x80){
       if(++wait_sign_timer > 5000){
         wait_sign_timer = 0;
         room_data_buf[40] &= 0x7f;
       }
     }

     
//     if(room_data_buf[42] & 0x01){
     if(f_em_flag){                                     //����ư 
       if(++em_sign_keep_timer > 5000){
         em_sign_keep_timer = 5001;
         room_data_buf[42] |= 0x01;
         if(++em_sign_timer > 100){
           em_sign_timer = 0;
           P_ex_led4 ^= 1;
         }
       }
       else{
         if(++em_sign_timer > 500){
           em_sign_timer = 0;
           P_ex_led4 ^= 1;
         }
       }
     }
     else P_ex_led4 = 1;     
    
    if(f_uart3_data_send){
      if(++u3_dir_delay_counter > 3){
        u3_dir_delay_counter = 0;
        f_uart3_data_send = 0;
        f_uart3_data_start = 1;
      }
    }         
     
     if(++indicator_timer > 1000){
       indicator_timer = 0;
       f_indicator_timer = 1;
     }
     
     else{
       if(++uart3_send_timer > 300){
         uart3_send_timer = 0;
         f_uart3_send_time = 1;
       }
     }
     
        if(++uart3_send_time_2 > 1000)
        {
          uart3_send_time_2 = 0;
          
          f_uart3_data_send_2 = 1;
        }
     
    /////// 1�� Ŀư ������ ������ ////////
    if(curtain_sw_buf[0] & 0x01) //curtain 1 open
    {
        if(++curtain_time[0] > 500) //500ms
        {
          curtain_time[0] = 0;
          curtain_sw_buf[0] &= 0xfe;
          
          curtain_state[0] = 1;
          curtain_control = 0;
        }
    }
    else if(curtain_sw_buf[0] & 0x02) //curtain 1 stop
    {
        if(++curtain_time[1] > 500) //500ms
        {
          curtain_time[1] = 0;
          curtain_sw_buf[0] &= 0xfd;
          
          curtain_state[0] = 4;
          curtain_control = 0;
        }
    }
    else if(curtain_sw_buf[0] & 0x04) //curtain 1 close
    {
        if(++curtain_time[2] > 500) //500ms
        {
          curtain_time[2] = 0;
          curtain_sw_buf[0] &= 0xfb;
          
          curtain_state[0] = 2;
          curtain_control = 0;
        }
    }
    /////////////////////////////////////////     
     
    /////// 2�� Ŀư ������ ������ ////////
    if(curtain_sw_buf[1] & 0x01) //curtain 2 open
    {
        if(++curtain_time[3] > 500) //500ms
        {          curtain_time[3] = 0;
          curtain_sw_buf[1] &= 0xfe;
          
          curtain_state[1] = 1;
          curtain_control = 0;
        }
    }
    else if(curtain_sw_buf[1] & 0x02) //curtain 2 stop
    {
        if(++curtain_time[4] > 500) //500ms
        {
          curtain_time[4] = 0;
          curtain_sw_buf[1] &= 0xfd;
          
          curtain_state[1] = 4;
          curtain_control = 0;
        }
    }
    else if(curtain_sw_buf[1] & 0x04) //curtain 2 close
    {
        if(++curtain_time[5] > 500) //500ms
        {
          curtain_time[5] = 0;
          curtain_sw_buf[1] &= 0xfb;
          
          curtain_state[1] = 2;
          curtain_control = 0;
        }
    }   
    /////////////////////////////////////////    
    
    if(fan_control_flag) // fan control �� ���
    {
        if(++fan_control_timer > 6000) //6000ms
        {
            fan_control_timer = 0;
            
            fan_control_flag = 0;
        }
    }
    
    if(f_curtain_open)  //Ŀư ������ȣ Ȱ��ȭ
    {
      if(++curtain_open_timer> 500)     // Ű���� ���� �� 500ms ���
      {
        curtain_open_timer = 0; // ī��Ʈ Ŭ����
        f_curtain_open = 0;
        
        curtain_sw_buf[0] |= 0x01; //Ŀư 1 open ��ȣ ����
        curtain_control = 1; //LED ǥ�� X
        f_key_curtain_control = 0; //�����ѹ����� flag 
      }
    }
    else curtain_open_timer = 0;
    
    // ����Ʈ ����ġ ��Ż��� �ľ� 0 or 20 //  0=����̻�,  1=��ſ�Ȱ                                                      ///////////////////
   
    for(int i=0; i < room_data_buf[64]; i++)            // 58��(LS�ⱸ�� ��ż���)���� �����ڴ�.
    {

      if(light_switch_falling_count[i]==20)              // 20 =��� �̻��ϋ�
      {
        if(i==0) room_data_buf[65] &= 0xfe;            // 59�� ������(��� �ⱸ�� ���¹���)
        if(i==1) room_data_buf[65] &= 0xfd;            // fe,fd,fc = �ش� LS���� 0(����̻�)���� ����ڴ�
        if(i==2) room_data_buf[65] &= 0xfb;            //  bit�� �з�
        if(i==3) room_data_buf[65] &= 0xf7;            //  4�� Light switch
        if(i==4) room_data_buf[65] &= 0xef;            //  5�� Light switch
        if(i==5) room_data_buf[65] &= 0xdf;            //  6�� Light switch
        if(i==6) room_data_buf[65] &= 0xbf;            //  7�� Light switch
        if(i==7) room_data_buf[65] &= 0x7f;            //  8�� Light switch
        
        if(i==8) room_data_buf[66] &= 0xfe;            // 9�� Light switch
        if(i==9) room_data_buf[66] &= 0xfd;            // 10�� Light switch
        if(i==10) room_data_buf[66] &= 0xfb;            // 11�� Light switch
        if(i==11) room_data_buf[66] &= 0xf7;            // 12�� Light switch
        if(i==12) room_data_buf[66] &= 0xef;            // 13�� Light switch
        if(i==13) room_data_buf[66] &= 0xdf;            // 14�� Light switch
        if(i==14) room_data_buf[66] &= 0xbf;            // 15�� Light switch
        if(i==15) room_data_buf[66] &= 0x7f;            // 16�� Light switch        
      }
      else                                                        // 20 X, ��, ��� ��Ȱ
      {
        if(i==0) room_data_buf[65] |= 0x01;            //  1�� Light switch
        if(i==1) room_data_buf[65] |= 0x02;            //  2�� Light switch
        if(i==2) room_data_buf[65] |= 0x04;            //  3�� Light switch
        if(i==3) room_data_buf[65] |= 0x08;            //  4�� Light switch
        if(i==4) room_data_buf[65] |= 0x10;            //  5�� Light switch
        if(i==5) room_data_buf[65] |= 0x20;            //  6�� Light switch
        if(i==6) room_data_buf[65] |= 0x40;            //  7�� Light switch
        if(i==7) room_data_buf[65] |= 0x80;            //  8�� Light switch   
        
        if(i==8) room_data_buf[66] |= 0x01;            //  9�� Light switch
        if(i==9) room_data_buf[66] |= 0x02;            //  10�� Light switch
        if(i==10) room_data_buf[66] |= 0x04;            //  11�� Light switch
        if(i==11) room_data_buf[66] |= 0x08;            //  12�� Light switch
        if(i==12) room_data_buf[66] |= 0x10;            //  13�� Light switch
        if(i==13) room_data_buf[66] |= 0x20;            //  14�� Light switch
        if(i==14) room_data_buf[66] |= 0x40;            //  15�� Light switch
        if(i==15) room_data_buf[66] |= 0x80;            //  16�� Light switch           
      }
    }
    
    for(int i=0; i<2; i++)
    {
      if(nt_device_falling_count[i] == 20)
      {
        if(i==0) room_data_buf[67] &= 0xfe;           //NT 1,2
        if(i==1) room_data_buf[67] &= 0xfd;
      }
      else
      {
        if(i==0) room_data_buf[67] |= 0x01;           //NT 1,2
        if(i==1) room_data_buf[67] |= 0x02;

       }     
    }
      if(ci_device_falling_count[0] == 20)
      {
        room_data_buf[67] &= 0xfb;           //CI 1
      }
      else
      {
        room_data_buf[67] |= 0x04;           //CI 1
       }     

      if(ks_device_falling_count[0] == 20)
      {
        room_data_buf[67] &= 0xf7;           //KS 1
      }
      else
      {
        room_data_buf[67] |= 0x08;           //KS 1
      }
    
    for(int i=0; i<4; i++)               
    {
      if(ts_device_falling_count[i] == 20)      
      {
        if(i==0) room_data_buf[24] &= 0x7f;     //TS 1
        if(i==1) room_data_buf[25] &= 0x7f;     //TS 2
        if(i==2) room_data_buf[26] &= 0x7f;     //TS 3 
        if(i==3) room_data_buf[27] &= 0x7f;     //TS 4
      }
      else      //��� �̻�ƴҶ� = 1
      {
        if(i==0) room_data_buf[24] |= 0x80;
        if(i==1) room_data_buf[25] |= 0x80;
        if(i==2) room_data_buf[26] |= 0x80;
        if(i==3) room_data_buf[27] |= 0x80;
      }
    }
    
    if(f_clean_wait)  //û�ҿϷ� flag on
     {
         if(++clean_timer > 600) //1 sec state
         {
            clean_timer = 0;
            clean_ok_flag = 1;
         }
     }
     else 
     {
         clean_timer = 0;
         clean_ok_flag = 0;
     }        
  }
}


/*
	timer0 	: buzzer  
	timer1 	: uart1  
	timer2 	: uart2  
	timer3	:  
	timer4	: exio  
	timer5	: sfc  
	timer6	: eeprom
	timer7	:  

	timer11	: uartx	
	timer12	: uartx	
	
*/  



void digital_volume_execution(unsigned char index, unsigned char vol_data)
{
  unsigned char temp_data = 0x11;
  unsigned char i;
  
//  if(index == 0)  P_volum_sc_0 = 0;
//  else P_volum_sc_1 = 0;
  P_volum_sc_0 = 0;
  //P_volum_sc_1 = 0;
  delay(10);
  
  for(i=0; i<8; ++i){
    if((temp_data << i) & 0x80) {
      P_volum_data = 1;
    }
    else P_volum_data = 0;
    delay(10);
    spi_clk_out();
  }
  delay(10);
  temp_data = vol_data;//vol_data;
  for(i=0; i<8; ++i){
    if((temp_data << i) & 0x80) P_volum_data = 1;
    else P_volum_data = 0;
    delay(10);
    spi_clk_out();
  }
  delay(10);
  P_volum_sc_0 = 1;
  //P_volum_sc_1 = 1; 
}

void spi_clk_out(void)
{
  P_volum_sck = 1;
  P_volum_sck = 1;
  P_volum_sck = 1;
  P_volum_sck = 1;
  P_volum_sck = 1;
  P_volum_sck = 0;
  P_volum_sck = 0;
  P_volum_sck = 0;
  P_volum_sck = 0;
  P_volum_sck = 0;
}


void chime_execution(void)
{
  if(f_chime_keep){
    switch(chime_sq){
      case 0:
        if(f_chime_sq_timer){
          P_chime_mute = 1;
          f_chime_sq_timer = 0;
          chime_sq_timer = 0;
          chime_sq_comp = 100;                    
          ++chime_sq;
        }
        break;
        
      case 1:
        if(f_chime_sq_timer){
          P_chime_out = 1;
          f_chime_sq_timer = 0;
          chime_sq_timer = 0;
          chime_sq_comp = 4000;
          ++chime_sq;
        }
        break;
      case 2:
        if(f_chime_sq_timer){
          P_chime_out = 0;
          if(!f_alram_on) P_chime_mute = 0;
          chime_sq = 0;
          f_chime_keep = 0;
          f_dnd_con = 0;
        }
        break;
    }
  }
  else{
    if(f_alram_on){
      P_chime_mute = 1;
      P_alarm_out = 1;
    }
    else{
      if(!f_chime_keep) P_chime_mute = 0;
      P_alarm_out = 0;
    }
  }
}

void dip_switch_read(void)
{
  unsigned char dip_switch_buf_temp[2];
  
  dip_switch_buf_temp[0] = 0;
  dip_switch_buf_temp[1] = 0;
  
  P_dip_rd_1 = 0;
  P_dip_rd_0 = 1;
  if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8))  dip_switch_buf_temp[0] |= 0x80;
  if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7))  dip_switch_buf_temp[0] |= 0x40;
//  
  if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2))  dip_switch_buf_temp[0] |= 0x20;
  if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)) dip_switch_buf_temp[0] |= 0x10;
  if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)) dip_switch_buf_temp[0] |= 0x08;
  if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)) dip_switch_buf_temp[0] |= 0x04;
  if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)) dip_switch_buf_temp[0] |= 0x02;
  if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) dip_switch_buf_temp[0] |= 0x01;
  P_dip_rd_1 = 1;
  
  P_dip_rd_0 = 0;
  if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8))  dip_switch_buf_temp[1] |= 0x01;
  if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7))  dip_switch_buf_temp[1] |= 0x02;
  if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2))  dip_switch_buf_temp[1] |= 0x04;
  if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)) dip_switch_buf_temp[1] |= 0x08;
  if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)) dip_switch_buf_temp[1] |= 0x10;
  if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)) dip_switch_buf_temp[1] |= 0x20;
  if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)) dip_switch_buf_temp[1] |= 0x40;
  if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) dip_switch_buf_temp[1] |= 0x80;
  P_dip_rd_0 = 1;
  
  dip_switch_buf[0] = dip_switch_buf_temp[0];
  dip_switch_buf[1] = dip_switch_buf_temp[1];
  main_id_buf = dip_switch_buf[0] & 0x3f;
  main_id_buf = dip_switch_buf[0] & 0x3f; //���� ��巹�� �Ҵ� (����) ������?? �����??  - 04.08 �쵵 ���� ��� ����
  //room_data_buf[79] = dip_switch_buf[0] & 0x3f; // sample room 
  
  if(dip_switch_buf[0] & 0x80) f_volume_control = 1;    //volume control ?
  else f_volume_control = 0;
  
  if(!main_comm_rx_ok)         //������� ���� �ȵǾ� ������ (ex �����)
  {
    led_bright_control = 1;
    //room_data_buf[79] = 0x02; //�ӽ� ���� ���� 2�ܰ�
    
    if(dip_switch_buf[1] & 0x02)
    {
      room_data_buf[74] = 2;          //ȯ���� ����                            
    }
    else{
      if(dip_switch_buf[1] & 0x01){
        room_data_buf[20] |= 0x04;    //�ù�
        room_data_buf[21] |= 0x04;    //�ù�
        room_data_buf[22] |= 0x04;    //�ù�
        room_data_buf[23] |= 0x04;    //�ù�
        room_data_buf[74] = 1;
      }
      else{
        room_data_buf[20] &= 0xfb;    //����
        room_data_buf[21] &= 0xfb;    //����
        room_data_buf[22] &= 0xfb;    //����
        room_data_buf[23] &= 0xfb;    //����
        room_data_buf[74] = 0;
      }
    }
    if(dip_switch_buf[1] & 0x08) f_air_set_temp_kind = 1; //�����µ� �ִ�,�ּ� ����
    else f_air_set_temp_kind = 0;                         //�µ����� �����µ� ����
  }
 
  if((dip_switch_buf[1] & 0xe0) == dip_mode){
    if(++dip_mode_count > 10){
      dip_mode_count = 0;
      
      room_data_buf[60] = 0x02;        //��������
      room_data_buf[61] = 0x02;        //��������
      room_data_buf[62] = 0x02;        //��������
      room_data_buf[63] = 0x02;        //��������
      room_data_buf[41] &= 0xfd;       //�ɾ����� OFFS
      
      switch(dip_mode){   //CB mode�� �����Ѵ�.
        case 0x00:    //RCB -1,2
          
          cb_mode = 0;
          
               room_data_buf[16] |= 0x48;  //1�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[17] |= 0x48;  //2�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[18] |= 0x48;  //3�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[19] &= 0xb7;  //4�� �µ����� �̻��(�ǳ�,�ٴ�)  
                          
               //room_data_buf[64] = 0x00;                      // ����Ʈ����ġ �Ѽ��� (16ea)
               room_data_buf[67] |= 0xd0;                       //  KS, CI, NT �Ѽ���

          
          break;
          
        case 0x80:    //RCB - 3
          
          cb_mode = 1;
          
                room_data_buf[16] |= 0x48;  //1�� �µ����� ���(�ǳ�,�ٴ�)
                room_data_buf[17] |= 0x48;  //2�� �µ����� ���(�ǳ�,�ٴ�)
                room_data_buf[18] |= 0x48; // 3�� �µ����� ���(�ǳ�,�ٴ�)
                room_data_buf[19] &= 0xb7;  //4�� �µ����� �̻��(�ǳ�,�ٴ�)
                
                //room_data_buf[64] = 0x04;      //LS ����
                room_data_buf[67] |= 0xd0;          
          
          
          break;
          
        case 0x40:   //RCB - 4
          
          cb_mode = 2; 
          
               room_data_buf[16] |= 0x48;  //1�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[17] |= 0x48;  //2�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[18] &= 0xb7; // 3�� �µ����� �̻��(�ǳ�,�ٴ�)
               room_data_buf[19] &= 0xb7;  //4�� �µ����� �̻��(�ǳ�,�ٴ�)
               
               //room_data_buf[64] = 0x04;      //LS ����
               room_data_buf[67] |= 0xd0;    
               
          break;
          
        case 0xc0:   //RCB-5,7
        
          cb_mode = 3;  
          
               room_data_buf[16] |= 0x48;  //1�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[17] |= 0x48;  //2�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[18] |= 0x48; // 3�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[19] |= 0x48;  //4�� �µ����� ���(�ǳ�,�ٴ�)
               
               //room_data_buf[64] = 0x04;      //LS ����
               room_data_buf[67] |= 0xd0;
          
          break;
          
        case 0x20:  //RCB - 6
        
          cb_mode = 4;
          
               room_data_buf[16] |= 0x48;  //1�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[17] |= 0x48;  //2�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[18] |= 0x48; // 3�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[19] &= 0xb7;  //4�� �µ����� �̻��(�ǳ�,�ٴ�)
               
               //room_data_buf[64] = 0x04;      //LS ����
               room_data_buf[67] |= 0xd0;
          
          break;
          
        case 0xa0:  //RCB - 8
        
          cb_mode = 5;
        
               room_data_buf[16] |= 0x48;  //1�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[17] |= 0x48;  //2�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[18] |= 0x48; // 3�� �µ����� ���(�ǳ�,�ٴ�)
               room_data_buf[19] |= 0x48;  //4�� �µ����� ���(�ǳ�,�ٴ�)
               
               //room_data_buf[64] = 0x04;      //LS ����
               room_data_buf[67] |= 0xd0;
        case 0xe0:
          break;
      }
      
    }
  }
  else{
    dip_mode = dip_switch_buf[1] & 0xe0;
    dip_mode_count = 0;
  }
  
  //if(dip_switch_buf[1] & 0x08) f_bath_control = 0; //ó�� ��ǽ� ȭ��� �� OFF
  //else f_bath_control = 1; //ó�� ��ǽ� ȭ��� �� ON  
  
}
//Dip switch 1 
// 1      ON = Normal       OFF = Volume set(use the jig)
// 2      ON = ������ K/S   OFF = ����� K/S
// 3 ~ 8  3(MSB), 8(LSB)    ON = 1, OFF = 0  (BCD Binary)
//
//Dip switch 2
// 1      ON = ����,        OFF = �ù�
// 2      ON = Normal       OFF = 4pipe(ȯ���� standard)
// 3      ON = �ѽ�         OFF = ���
// 4      ON = �и�����     OFF = ��������  (�ѽ��� ��� ����� �ٴڰ� ��⸦ ���ÿ� �����Ѵ�)
// 5      ON = �����µ�     OFF = ����µ� ǥ��
// 6 ~ 8  CB mode(��纰 ��� ����)








void delay(unsigned int i)
{
  while(--i != 0);
}


void eeprom_save_check(void)
{
  switch(eeprom_save_count){
    case 0: eeprom_save_comp(eeprom_save_count);  break;    //��� �����µ� 1 ����
    case 1: eeprom_save_comp(eeprom_save_count);  break;
    case 2: eeprom_save_comp(eeprom_save_count);  break;
    case 3: eeprom_save_comp(eeprom_save_count);  break;
    case 4: ++eeprom_save_count;  break;
    case 5: ++eeprom_save_count;  break;
    case 6: ++eeprom_save_count;  break;
    case 7: ++eeprom_save_count;  break;
    case 8: eeprom_save_comp(eeprom_save_count);  break;    //�ٴ� �����µ� 1 ����
    case 9: eeprom_save_comp(eeprom_save_count);  break;
    case 10: eeprom_save_comp(eeprom_save_count);  break;
    case 11: eeprom_save_comp(eeprom_save_count);  break;
    case 12: ++eeprom_save_count;  break;
    case 13: ++eeprom_save_count;  break;
    case 14: ++eeprom_save_count;  break; 
    case 15: ++eeprom_save_count;  break;
    case 16: eeprom_save_comp(eeprom_save_count);  break;   //�µ����� ���� 1 ����
    case 17: eeprom_save_comp(eeprom_save_count);  break;
    case 18: eeprom_save_comp(eeprom_save_count);  break;
    case 19: eeprom_save_comp(eeprom_save_count);  break;
    case 20: ++eeprom_save_count;  break;
    case 21: ++eeprom_save_count;  break;
    case 22: ++eeprom_save_count;  break;
    case 23: ++eeprom_save_count;  break;
    case 24: ++eeprom_save_count;  break;
    case 25: ++eeprom_save_count;  break;
    case 26: ++eeprom_save_count;  break;
    case 27: ++eeprom_save_count;  break;
    case 28: eeprom_save_comp(eeprom_save_count);  break;   //��� ����µ� ��.����
    case 29: eeprom_save_comp(eeprom_save_count);  break;   //�ٴ� �����µ� ��.����
    case 30: ++eeprom_save_count;  break;
    
    case 31: ++eeprom_save_count;  break;
    case 32: ++eeprom_save_count;  break;
    case 33: ++eeprom_save_count;  break;
    case 34: ++eeprom_save_count;  break;
    case 35: ++eeprom_save_count;  break;
    case 36: ++eeprom_save_count;  break;
    case 37: ++eeprom_save_count;  break;
    case 38: ++eeprom_save_count;  break;
    case 39: ++eeprom_save_count;  break;
    case 40: eeprom_save_comp(eeprom_save_count);  break;   //room status(check in, guest in ....)
    case 41: ++eeprom_save_count;  break;
    case 42: ++eeprom_save_count;  break;
    case 43: ++eeprom_save_count;  break;
    case 44: ++eeprom_save_count;  break;
    case 45: eeprom_save_comp(eeprom_save_count);  break;   //Relay status 1
    case 46: eeprom_save_comp(eeprom_save_count);  break;   //Relay status 2
    case 47: eeprom_save_comp(eeprom_save_count);  break;  //Relay status 3
    case 48: ++eeprom_save_count;  break;
    case 49: ++eeprom_save_count;  break;
    case 50: eeprom_save_comp(eeprom_save_count);  break;   //NT LED Status 1
    case 51: eeprom_save_comp(eeprom_save_count);  break;   //NT LED Status 2
    case 52: ++eeprom_save_count;  break;
    case 53: ++eeprom_save_count;  break;
    case 54: ++eeprom_save_count;  break;
    case 55: ++eeprom_save_count;  break;
    case 56: ++eeprom_save_count;  break;
    case 57: ++eeprom_save_count;  break;
    case 58: ++eeprom_save_count;  break;
    case 59: ++eeprom_save_count;  break;
    case 60: ++eeprom_save_count;  break;
    case 61: ++eeprom_save_count;  break;
    case 62: ++eeprom_save_count;  break;
    case 63: ++eeprom_save_count;  break;
    case 64: ++eeprom_save_count;  break;
    case 65: ++eeprom_save_count;  break;
    case 66: ++eeprom_save_count;  break;
    case 67: ++eeprom_save_count;  break;
    case 68: ++eeprom_save_count;  break;
    case 69: ++eeprom_save_count;  break;
    
    
 //�����ؾ��� �߰� ���� ���� ����   
    case 70: eeprom_save_comp(eeprom_save_count);  break;   //����µ��� ����
    case 71: eeprom_save_comp(eeprom_save_count);  break;   //��� ���� ���� ���ؿµ� ����
    case 72: eeprom_save_comp(eeprom_save_count);  break;   //��� ���� �ù� ���ؿµ�
    case 73: eeprom_save_comp(eeprom_save_count);  break;   //�ٴڰ��� ���ؿµ�
    case 74: eeprom_save_comp(eeprom_save_count);  break;   //��,���� ���
    case 75: eeprom_save_comp(eeprom_save_count);  break;   //��,�� ��ȯ �µ���
    case 76: eeprom_save_comp(eeprom_save_count);  break;   //�̻� �溸�µ�
    case 77: eeprom_save_comp(eeprom_save_count);  break;   //�Ա��� ���� �����ð� ��
    case 78: eeprom_save_comp(eeprom_save_count);  break;   //�Ա��� ���� �����ð� ��
    case 79: eeprom_save_comp(eeprom_save_count);  break;   //Chime volume value
    case 80: eeprom_save_comp(eeprom_save_count);  break;   //Alram Volume value
    case 81: eeprom_save_comp(eeprom_save_count);  break;   //Group No
    case 82: eeprom_save_comp(eeprom_save_count);  break;   //�ⱸ�� ���� ������
    case 83: eeprom_save_comp(eeprom_save_count);  break;   //ts mode backup
    case 84: ++eeprom_save_count;  break;
    case 85: ++eeprom_save_count;  break;
    case 86: ++eeprom_save_count;  break;
    case 87: ++eeprom_save_count;  break;
    case 88: ++eeprom_save_count;  break;
    case 89: ++eeprom_save_count;  break;
    case 90: ++eeprom_save_count;  break;
    case 91: ++eeprom_save_count;  break;
    case 92: ++eeprom_save_count;  break;
    case 93: ++eeprom_save_count;  break;
    case 94: ++eeprom_save_count;  break;
    case 95: ++eeprom_save_count;  break;
    case 96: ++eeprom_save_count;  break;
    case 97: ++eeprom_save_count;  break;
    case 98: ++eeprom_save_count;  break;
    case 99: ++eeprom_save_count;  break;
    
//--------- Dimmer Level save routine ---------    
    case 100: eeprom_save_comp_level(eeprom_save_count);  break;
    case 101: eeprom_save_comp_level(eeprom_save_count);  break;
    case 102: eeprom_save_comp_level(eeprom_save_count);  break;
    case 103: eeprom_save_comp_level(eeprom_save_count);  break;
    case 104: eeprom_save_comp_level(eeprom_save_count);  break;
    case 105: eeprom_save_comp_level(eeprom_save_count);  break;
    case 106: eeprom_save_comp_level(eeprom_save_count);  break;
    case 107: eeprom_save_comp_level(eeprom_save_count);  break;
    case 108: eeprom_save_comp_level(eeprom_save_count);  break;
    case 109: eeprom_save_comp_level(eeprom_save_count);  break;
    case 110: eeprom_save_comp_level(eeprom_save_count);  break;
    case 111: eeprom_save_comp_level(eeprom_save_count);  break;
    case 112: eeprom_save_comp_level(eeprom_save_count);  break;
    case 113: eeprom_save_comp_level(eeprom_save_count);  break;
    case 114: eeprom_save_comp_level(eeprom_save_count);  break;
    case 115: eeprom_save_comp_level(eeprom_save_count);  break;
    case 116: ++eeprom_save_count;  break;
    case 117: ++eeprom_save_count;  break;
    case 118: ++eeprom_save_count;  break;
    case 119: ++eeprom_save_count;  break;
    case 120: eeprom_save_count=0;  break;
  }
}

void eeprom_save_comp(unsigned char eep_addr)
{
  if(room_data_buf[eep_addr] != eep_room_data_buf[eep_addr]){
    i2c_bytewrite(Slave_Address, eep_addr, room_data_buf[eep_addr]);
    if(eep_err_status != 'E'){
      ++eeprom_save_count;
      eep_room_data_buf[eep_addr] = room_data_buf[eep_addr];
    }
  }
  else ++eeprom_save_count;
}

void eeprom_save_comp_level(unsigned char eep_addr)
{
  if(dimmer_level[eep_addr-100] != eep_room_data_buf[eep_addr]){
    i2c_bytewrite(Slave_Address, eep_addr, dimmer_level[eep_addr-100]);
    if(eep_err_status != 'E'){
      ++eeprom_save_count;
      eep_room_data_buf[eep_addr] = dimmer_level[eep_addr-100];
    }
  }
  else ++eeprom_save_count;
}


/******************************************************************************************************/
/* Timer_setup : �ش� tmxfg�� "0"���� ���� �� ���ϴ� timer ���� ó���Ѵ�.		              */ 											
/******************************************************************************************************/
void Timer_setup(u8 tmno, u16 timer) {
	
	Clrbit((u32*)&tm.word, tmno);
	timer_buf[tmno] =timer;
}
/******************************************************************************************************/
/* timer base bit initial :		                                                              */																						
/******************************************************************************************************/
void Gp_initial(void){
u8 ix;

   tm.word =(u32)-1;
	for(ix=0; ix<sizeof(timer_buf)/2; ix++) timer_buf[ix] =1;

	Wdtcnt =3000;
}


/******************************************************************************************************/
/* PLL_initial	:	 Configures the system clocks.						      */												
/******************************************************************************************************/
void PLL_initial(void) {	
  //PLL configuration
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();
  
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);
  
    /* Wait till HSE is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
  
    /* HCLK = MAX SYSCLK = 72MHz/1=72MHz*/
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* configure MAX PCLK2 = 72MHz/1=72MHz */
    RCC_PCLK2Config(RCC_HCLK_Div1); 
  
    /* configure MAX PCLK1 = 72MHz/2=36MHz */
    RCC_PCLK1Config(RCC_HCLK_Div2);
    
    /* configure MAX PLLCLK = 8MHz/1 * 9= 72MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  
    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);
  
    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){
       RCC_PLLCmd(ENABLE);
    }
  
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08){
      RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    }
  
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  
    /* This function fills a RCC_ClocksTypeDef structure with the current
       frequencies of different on chip clocks (for debug purpose) */
    RCC_GetClocksFreq(&RCC_ClockFreq);
  
    /* Enable Clock Security System(CSS) */
    RCC_ClockSecuritySystemCmd(ENABLE); 
}

/******************************************************************************************************/
/* NVIC_initial	:	vector configuration   					                      */												
/******************************************************************************************************/
void NVIC_initial(void){

#ifdef  VECT_TAB_RAM  
	/* Set the Vector Table base location at 0x20000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif  

	/* Enable and configure RCC global IRQ channel */  
	NVIC_InitStructure.NVIC_IRQChannel = RCC_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

}

/******************************************************************************************************/
/* SYSTICK_initial	:	SYSTICK_ configuration   					      */												
/******************************************************************************************************/
void SYSTICK_initial(void) { 	
    /* SysTick end of count event each 1ms with input clock equal to 9MHz (HCLK/8, default) */
    SysTick_SetReload(9000);
    
    /* Configure the SysTick Handler Priority: Preemption priority and subpriority */
    NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick, 0, 0);  
  
     /* Enable SysTick interrupt */
    SysTick_ITConfig(ENABLE);
  
    /* Enable the SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Enable);
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
  TIM_TimeBaseStructure.TIM_Period = 100;    				//1Mhz/1000=->1msec timer      
  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 
  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
  
  // Enable the USART1 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
}
/******************************************************************************************************/
/* GPIO_port_initial	:								              */												
/******************************************************************************************************/
void GPIO_port_initial(void){

  
  
  
	/* Enable GPIOB clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD\
	|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO, ENABLE);

  
/* JTAG-DP Disabled and SW-DP Enabled*/
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
// led Port Define
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 |GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        led1 = led2 = led3 = led4 = led5 = 1;
        
// Relay port define  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
  
//Ex LED port define
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
        
// Dip read port(DIP Switch) define  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;    //7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;    //Dip 4,3,2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4;    //Dip 5,6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7;    //Dip 0,1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;                    //Dip load
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
        
  P_dip_rd_0 = 1;
  P_dip_rd_1 = 1;
        
        
// Key sensor input port define  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;    //Data read port
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

// Emergency switch input port define  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;    //Data read port
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

// Door switch input port define  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_0;    //Data read port
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

  
// Enterance, DND, MUR, Chime switch  input port define
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  
// Chime out, Alram & MUTE port Define  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  P_chime_out = 0;

// External switch input port define  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;    //Data read port
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;    //Data read port
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5| GPIO_Pin_6;    //Data read port
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
 
// Digital volume port define
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;  //data, sck(SPI)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  P_volum_sck = 0;
  P_volum_data = 0;
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;               //volume 1 sc(chime)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
  P_volum_sc_0 = 1;


  
// 24LCXX write protect, Tuner enable Define  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  
// Hardware watch dog clear pin out port
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

//==================================================================
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


/******************************************************************************************************/
/* EXTI0_initial	:		        						      */												
/******************************************************************************************************/
void EXTI_initial(void) {

	/* Enable GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* Configure PC.0 input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_2  |GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);  

	/* Configure Port EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 |EXTI_Line1 |EXTI_Line2 |EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);
	EXTI_GenerateSWInterrupt(EXTI_Line0);  
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
	EXTI_GenerateSWInterrupt(EXTI_Line1);  
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
	EXTI_GenerateSWInterrupt(EXTI_Line2);  
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource3);
	EXTI_GenerateSWInterrupt(EXTI_Line3);  

	/* Enable the EXTI0 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);    

	/* Enable the EXTI1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	  

	/* Enable the EXTI2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	  

	/* Enable the EXTI3 1Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	  
}

/******************************************************************************************************/
/* CORTEX_initial	:		        						      */												
/******************************************************************************************************/
void CORTEX_initial(void) {

  unsigned char i;
  
	PLL_initial();		
	NVIC_initial();               
	GPIO_port_initial();	

//	SRAM_Init();						// extend ram initial
	Gp_initial();
        SYSTICK_initial();
  TIME2_initial();
  UART1_initial();
  UART2_initial();
  UART3_initial();
  P_24lcxx_enable = 0;

  
  ts_first_power_on[0] = 0;
  ts_first_power_on[1] = 0;
  ts_first_power_on[2] = 0;
  ts_first_power_on[3] = 0;
  
  f_first_nt_power_on[0] = 0;
  f_first_nt_power_on[1] = 0;
  f_first_nt_power_on[2] = 0;
  f_first_nt_power_on[3] = 0;
  
  
  ts_air_set_temp_from_PC[0] = i2c_byteread(Slave_Address, 0);
  ts_air_set_temp_from_PC[1] = i2c_byteread(Slave_Address, 1);
  ts_air_set_temp_from_PC[2] = i2c_byteread(Slave_Address, 2);
  ts_air_set_temp_from_PC[3] = i2c_byteread(Slave_Address, 3);
  ts_floor_set_temp_from_PC[0] = i2c_byteread(Slave_Address, 8);
  ts_floor_set_temp_from_PC[1] = i2c_byteread(Slave_Address, 9);
  ts_floor_set_temp_from_PC[2] = i2c_byteread(Slave_Address, 10);
  ts_floor_set_temp_from_PC[3] = i2c_byteread(Slave_Address, 11);
  ts_set_temp_send_count[0] = 0;
  ts_set_temp_send_count[1] = 0;
  ts_set_temp_send_count[2] = 0;
  ts_set_temp_send_count[3] = 0;
  room_data_buf[16] = i2c_byteread(Slave_Address, 16);    //TS status1
  room_data_buf[17] = i2c_byteread(Slave_Address, 17);    //TS status2
  room_data_buf[18] = i2c_byteread(Slave_Address, 18);    //TS status3
  room_data_buf[19] = i2c_byteread(Slave_Address, 19);    //TS status4

  room_data_buf[40] = i2c_byteread(Slave_Address, 40);    //Room status(chec in, Guest in ....)
  if(room_data_buf[40] & 0x02)  f_first_room_inout = 1;   //Room in/out flag restore
  else  f_first_room_inout = 0;
  if((room_data_buf[40] & 0x0c) == 0x0c) room_data_buf[40] &= 0xf3; //DND, MUR initial
  
  room_data_buf[42] = 0;                                  //�̻�µ� �� ���߻� clear(�ʱ� clear)
  
  room_data_buf[45] = i2c_byteread(Slave_Address, 45);    //Relay status 1
  room_data_buf[46] = i2c_byteread(Slave_Address, 46);    //Relay status 2
  room_data_buf[47] = i2c_byteread(Slave_Address, 47);    //Relay status 3
  
  room_data_buf[50] = i2c_byteread(Slave_Address, 50);    //NT LED status 1
  room_data_buf[51] = i2c_byteread(Slave_Address, 51);    //NT LED status 2

  
  room_data_buf[70] = i2c_byteread(Slave_Address, 70);    //�����µ���
  if(room_data_buf[70] > 90) room_data_buf[70] = 30;
  room_data_buf[71] = i2c_byteread(Slave_Address, 71);    //��� ���� ���� ���ؿµ�
  if((room_data_buf[71] == 0) || (room_data_buf[71] == 0xff)) room_data_buf[71] = 15;
  room_data_buf[72] = i2c_byteread(Slave_Address, 72);    //��� ���� �ù� ���ؿµ�
  if((room_data_buf[72] == 0) || (room_data_buf[72] == 0xff)) room_data_buf[72] = 30;
  room_data_buf[73] = i2c_byteread(Slave_Address, 73);    //�ٴڰ��� ���ؿµ�
  if((room_data_buf[73] == 0) || (room_data_buf[73] == 0xff)) room_data_buf[73] = 10;
  room_data_buf[74] = i2c_byteread(Slave_Address, 74);    //��,���� ���
//  if(room_data_buf[74] > 4) main_comm_rx_ok = 0;
//  else main_comm_rx_ok = 1;
  room_data_buf[75] = i2c_byteread(Slave_Address, 75);    //�ó��� ��ȯ�µ���
  if((room_data_buf[75] > 9) || (room_data_buf[75] == 0)) room_data_buf[75] = 3;
  room_data_buf[76] = i2c_byteread(Slave_Address, 76);    //�̻�溸�µ�
  if((room_data_buf[76] > 90) || (room_data_buf[76] == 0)) room_data_buf[76] = 50;
  room_data_buf[77] = i2c_byteread(Slave_Address, 77);    //�Ա��� ���� �����ð� ��
  room_data_buf[78] = i2c_byteread(Slave_Address, 78);    //�Ա��� ���� �����ð� ��
  entry_lamp_delay_time = (room_data_buf[77] * 0x100) + room_data_buf[78];
  if((entry_lamp_delay_time > 900) || (entry_lamp_delay_time == 0)) entry_lamp_delay_time = 60;
  
  room_data_buf[28] = i2c_byteread(Slave_Address, 28);    //�ǳ� �����µ� ��.����
  if((room_data_buf[28] == 0) || (room_data_buf[28] == 0xff)) room_data_buf[28] = 0xa7;
  
  room_data_buf[29] = i2c_byteread(Slave_Address, 29);    //�ٴ� �����µ� ��.����
  if((room_data_buf[29] == 0) || (room_data_buf[29] == 0xff)) room_data_buf[29] = 0xa7;

  room_data_buf[79] = i2c_byteread(Slave_Address, 79);    //Chime Volume level
  room_data_buf[80] = i2c_byteread(Slave_Address, 80);    //Alram Volume level
  
  room_data_buf[81] = i2c_byteread(Slave_Address, 81);    //Group No
  if((room_data_buf[81] > 99) || (room_data_buf[81] == 0)) room_data_buf[81] = 0xff;

  room_data_buf[82] = i2c_byteread(Slave_Address, 82);    //�ⱸ�� ���� ������
  if((room_data_buf[82] == 0) || (room_data_buf[82] == 0xff)){
    room_data_buf[82] = 0xa2;   //Def. 10sec & auto low illumination
  }
  room_data_buf[82] = 0xa2;
  
  room_data_buf[83] = i2c_byteread(Slave_Address, 83);    //ts mode backup
  if(room_data_buf[83] > 2) room_data_buf[83] = 0;
  
  

  f_air_set_temp_kind = 0;                         //�µ����� �����µ� ����
//----------- Dimmer Level reload ---------------------
  for(i=0; i<16; ++i){
    dimmer_level[i] = i2c_byteread(Slave_Address, 100+i);     //Dimmer Level 0
    if(dimmer_level[i] > 99) dimmer_level[i] = 30;
  }
  
  
  
  f_ks_error = 0;                       ////???
  
  f_ts_power_on[0] = 0;
  f_ts_power_on[1] = 0;
  f_ts_power_on[2] = 0;
  f_ts_power_on[3] = 0;
  
  f_handy_sign = 1;
  
  
  
  //dac_main();
  
#ifdef __WATCHDOG_ENABLE__ 
	Iwdog_initial(3000);
#endif
}

//===========================================================================
#if 0
u16 hex2dec(u16 _hex) {
unsigned char _temp_dec;
	
	_temp_dec =0;
	while(1) {
		if(_hex >9) _hex -=10,	_temp_dec +=0x10;
 		else { _temp_dec +=_hex;
			return(_temp_dec);
 		}
	}
}
#else

const u16 hex1024[] = { 0x00, 0x4096, 0x8192,0 };
const u16 hex256[] = { 0x00, 0x256, 0x512, 0x768, 0x1024, 0x1280, 0x1536,0x1792, 0x2048, 0x2304,0x2560, 0x2816, 0x3072, 0x3328, 0x3584, 0x3840 };
const u16 hex16[] = { 0x00, 0x16, 0x32, 0x48, 0x64, 0x80, 0x96, 0x112, 0x128,0x144,	0x160, 0x176, 0x192, 0x208, 0x224, 0x240 }; 
const u16 hex1[] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15 };
 
u16 data;
u16 decimal_adder(u32 other_data) {
u16 sum;
u8 ix, cflag,ss[4];

 	cflag =0;	
	for(ix=0; ix<4; ix++) {
		sum =(other_data &0x000f) + (data &0x000f) + cflag;
		if(sum >=0x0a || sum >=0x10) sum +=6, cflag =1;
		else cflag =0;
 		ss[ix] =sum & 0x0f;
		other_data>>=4;		data>>=4;
 	}
	return (u32)(ss[1]<<4 |ss[0]) | (u32)(ss[3]<<4 |ss[2]) <<8;
}

u16 hex2dec(u16 decimal) {

 	data = hex1024[(u8)(decimal>>12) &0x03];					data =decimal_adder(hex256[(u8)(decimal>>8) &0x0f]);	 
	data =decimal_adder(hex16[(u8)decimal>>4 &0x0f]);	  	data =decimal_adder(hex1[(u8)(decimal) &0x0f]);	 
	return data;
}

#endif

const u16 dec10[] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 0, 0, 0, 0, 0, 0	};
const u16 dec100[] = { 0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 0, 0, 0, 0, 0, 0		};
const u16 dec1000[] = { 0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 0, 0, 0, 0, 0, 0		};
u16 dec2hex(u16 decimal) {
	
	return (dec1000[(decimal>>12)&0x000f] + dec100[(decimal>>8)&0x000f] + dec10[(decimal>>4)&0x000f] + (decimal & 0x000f));
}

/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/


