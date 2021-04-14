/*=====================(C) COPYRIGHT 2014 PLUS-H.=========================
program		    : CONTROL BOX(RCS-300-RCU)
processor 	  : STM32F103xx
compiler		  : IAR 6.2A Compiler 
program BY	  : H.H.Hwang
date			    : 2014.	 .
copy right	  : P L U S  -  H. 
============================================================================*/
#define __SWITCH_IO_H__

#include "stm32f10x_lib.h"
#include "main.h"



//=== Key sensor check routine ===
void key_sensor_check(void)
{
  unsigned char key_temp = 0;   //전역변수 지정으로 인한 지역변수 숨김

    if((dip_switch_buf[0] & 0x40) == 0){    //접점형 key sensor execution

        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)) key_temp |= 0x01;
        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)) key_temp |= 0x02;

      
      if(key_temp == key_sensor_back){
        if(++key_sensor_chet > 500){    //500msec
          key_sensor_chet = 0;
          if(key_temp != 2) f_clean_wait = 0; //청소완료 아닐시 flag clear
          switch(key_temp){
            case 0: room_data_buf[40] &= 0xfd;  room_data_buf[49] = 0;  guest_out_executiom(); break;    //외출처리
            case 1: room_data_buf[40] |= 0x02;  room_data_buf[49] = 1;  guest_in_execution(); break;     //청소중
            case 2: room_data_buf[40] &= 0xfd;          //21.01.18 청소완료 딜레이 루틴 추가
                        f_clean_wait = 1;                         //키 삽입시, 한쪽 접점이 먼저 눌리는 현상을 방지
                        guest_out_executiom();
                        if(clean_ok_flag)
                        {
                          room_data_buf[49] = 2;
                          f_clean_finish = 1;
                        }
                        break;    //청소완료
            case 3: room_data_buf[40] |= 0x02;  room_data_buf[49] = 3;  guest_in_execution(); break;     //재실
          }
        }
      }
        else{
          key_sensor_back = key_temp;
          key_sensor_chet = 0;
        }
      }
      else{                              //통신형 key sensor execution
        if(f_ks_error)
        {
          room_data_buf[49] = 3;  guest_in_execution();
        }
        else
        {
          if(f_ks_rx_ok)
          {
            if(ks_key_status == key_sensor_back)
            {
              switch(ks_key_status & 0x30)
              {
                case 0x00: room_data_buf[40] &= 0xfd;  room_data_buf[49] = 0;  guest_out_executiom(); break;    //외출처리
                case 0x10: room_data_buf[40] |= 0x02;  room_data_buf[49] = 1;  guest_in_execution(); break;     //청소중
                case 0x20: room_data_buf[40] &= 0xfd;  room_data_buf[49] = 2;  f_clean_finish = 1; guest_out_executiom(); break;    //청소완료
                case 0x30: room_data_buf[40] |= 0x02;  room_data_buf[49] = 3;  guest_in_execution(); break;     //재실
              }
            }
            else
            {
              key_sensor_back = ks_key_status;
            }
          }
        }
      }
}

//Guest IN execution routine
void guest_in_execution(void)
{
  relay_all_off_delay_timer = 0;
  flag_relay_all_off_delay_timer = 0;
  
   room_data_buf[45] |= 0x01;
   
   if(f_key_curtain_control) //최초 한번동작
   {  
      f_curtain_open = 1;         // 커튼 열림 신호 플레그
     //curtain_open_timer = 0;
   }
  
  if(!f_first_room_inout){ //최초 키센서 입실시 (퇴실->입실)
    f_first_room_inout = 1;
    flag_fcu_action_time[0] = 1;      //FCU 즉시 실행

    room_data_buf[45] = 0xff;
    if(f_bath_control) room_data_buf[46] = 0xff; //화장실등 ON
    
    room_data_buf[47] = 0xff;
    
 /*   curtain_sw_buf[0] |= 0x01; //커튼 1 open 신호 전송
    curtain_control = 1; //LED 표시 X
    f_key_curtain_control = 0; //최초한번동작 flag */
    
    f_enterance_lamp_delay = 0;
    enterance_lamp_delay_timer = 0;   //입구등 Time Delay clear
  }
}

//Guest OUT execution routine 
void guest_out_executiom(void)
{
  unsigned char i;
  
  if(f_first_room_inout){
//    f_first_room_inout = 0;
    room_data_buf[36] &= 0xf7;        //Radio off
    room_data_buf[37] &= 0xf7;        //Radio off
    room_data_buf[38] &= 0xf7;        //Radio off
    room_data_buf[39] &= 0xf7;        //Radio off
    
    room_data_buf[54] = 0;  //TS1 AUTO MODE 전환                                  //////////////////////////////////////////
    room_data_buf[55] = 0;  //TS2 AUTO MODE 전환
    room_data_buf[56] = 0;  //TS3 AUTO MODE 전환
    room_data_buf[57] = 0;  //TS4 AUTO MODE 전환 
    
      if(!f_key_curtain_control) //최초 한번동작
      {
        f_key_curtain_control = 1;
        curtain_sw_buf[0] |= 0x04; //커튼 1 close 신호 전송
        curtain_control = 1; //LED 표시 X   
      }        
  }
  if(flag_relay_all_off_delay_timer){ //10sec delay
    f_first_room_inout = 0;
    room_data_buf[45] = 0;            //Relay All OFF
    room_data_buf[46] = 0;
    room_data_buf[47] = 0;
    
    for(i=0; i<16; ++i) dimmer_level[i] = 0;
    
  }
}

// Enterance lamp switch check routine 
void enterance_lamp_switch_check(void) //CHIME LEAD SWITCH (자석센서)
{                                           
  if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)))
    {
          if(++lead_chet > 1000)
          {
              f_lead_sw = 1;
              
              if(room_data_buf[40] & 0x02)  //입실인가?
              {       
                   f_dnd_sign = 1;
                   
                   if(f_dnd_ok)
                   {
                       f_dnd_ok = 0;
                       
                       switch(led_count)
                       {
                       case 0:
                        P_ind_led = 0; //DND LED OFF
                         ++led_count;
                         break;
                         
                       case 1:
                        P_ind_led = 1; //MUR LED ON
                         led_count = 0;
                         break;
                       }
                        
                       ++dnd_stop_count;
                          
                       if(dnd_stop_count > 3) 
                       {
                         dnd_stop_count = 0;
                         f_dnd_sign = 0;
                         f_dnd_con = 0;
                       }
                   }                    
              }
              else
              {
                    P_ind_led = 0; //DND LED OFF
              }
          }
  }
  else
  {
                lead_chet = 0;
                f_lead_sw = 0;
  }   
}

// Do not disturb switch check routine
void dnd_switch_check(void)
{
    if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1))){
      if(room_data_buf[40] & 0x02) //재실인가?
      {
        if(++dnd_chet > 1000)
        {
           if(!f_dnd_push)
           {
              f_dnd_push = 1;
              dnd_chet = 0;
              dnd_mur_execution('D','T');
           }
        }
      }
    }
    else
    {
        f_dnd_push = 0;
        dnd_chet = 0;
    }
}



// Make Up Room switch check routine
void mur_switch_check(void)
{
    if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2))){
      if(room_data_buf[40] & 0x02) //재실인가?
      {      
        if(++mur_chet > 1000)
        {
           if(!f_mur_push)
           {
              f_mur_push = 1;
              mur_chet = 0;
              dnd_mur_execution('M','T');
           }
        }
      }
    }
    else{
        f_mur_push = 0;
        mur_chet = 0;
    }
}


void dnd_mur_execution(unsigned char kind, unsigned char comd)
{
   switch(comd)
  {
    case 'N': 
      room_data_buf[40] &= 0xf3;  
      if(kind == 'D') room_data_buf[40] |= 0x04; 
      else room_data_buf[40] |= 0x08; 
      break;
      
    case 'F': room_data_buf[40] &= 0xf3;  break;
    case 'T':
      if(kind == 'D')
      {
        if(room_data_buf[40] & 0x04) room_data_buf[40] &= 0xf3;
        else
        {
         room_data_buf[40] &= 0xf3;
         room_data_buf[40] |= 0x04;
        }
      }
      else
      {
        if(room_data_buf[40] & 0x08) room_data_buf[40] &= 0xf3;
        else
        {
         room_data_buf[40] &= 0xf3;
         room_data_buf[40] |= 0x08;
        }
      }
        break;
  }
}

//Chime switch check routine
void chime_switch_check(void)
{
  if(!f_lead_sw)
  {
       if(room_data_buf[40] & 0x08) //MUR (DM PLATE)
      {
          P_dnd_led = 1; //DND LED OFF
          P_mur_led = 0; //MUR LED ON
      }    
      else P_mur_led = 1;//MUR LED OFF    
  
      if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3))){
        if(++chime_chet > 1000){
          
          P_ind_led = 0;
   
              if(!(room_data_buf[40] & 0x04)) //DND가 아닐때
              {          
                  if(!f_chime_push){
                    f_chime_push = 1;
                    if(!f_chime_keep){
                      f_chime_keep = 1;
                      chime_sq = 0;
                      f_chime_sq_timer = 1;
                      
                      digital_volume_execution(0,room_data_buf[79]);
                      //dac_main();
                      //DMA_Cmd(DMA2_Channel3, ENABLE);
                  
                    //  if(room_data_buf[40] & 0x02) //입실일때 
                    //  {
                        room_data_buf[48] |= 0x80;
                        f_chime_sign = 1;
                    //  }
                    }
                  }
              }
              else
              {
                  f_dnd_con = 1;
              }
              
            }
      }
      else{
        chime_chet = 0;
        f_chime_push = 0;
        P_ind_led = 1;
      }
      
      if(f_dnd_con) //DND 일때 차임벨누를시 DND불 깜박임
      {
         f_dnd_sign = 1;
            
         if(f_dnd_ok)
         {
             f_dnd_ok = 0;
             P_dnd_led ^= 1;
             ++dnd_stop_count;
                
             if(dnd_stop_count > 6) 
             {
               dnd_stop_count = 0;
               f_dnd_sign = 0;
               f_dnd_con = 0; 
             }
         }            
      }
      else
      {
          if(room_data_buf[40] & 0x04) //DND (DM PLATE)
          {
              P_dnd_led = 0; //DND LED ON
              P_mur_led = 1; //MUR LED OFF
          }
          else P_dnd_led = 1;//DND LED OFF      
      }
}
}


void door_sensor_check(void)
{
  if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13))){
    if(++door_chet > 1000){
      door_chet = 0;
      room_data_buf[40] &= 0xbf;    //door close
    }
  }
  else{
    if(++door_chet > 1000){
      door_chet = 0;
      room_data_buf[40] |= 0x40;    //door open
    }
  }
}

void window_sensor_check(void)      //임시 지정 포트
{
  if(!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3))){
    if(++window_chet > 1000){
      window_chet = 0;
      room_data_buf[40] &= 0xef;    //window close
    }
  }
  else{
    if(++window_chet > 1000){
      window_chet = 0;
      room_data_buf[40] |= 0x10;    //window open
    }
  }
}

void emergency_switch_check(void)
{
   if(!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6))){
      if(++emergency_chet > 1000){
         emergency_chet = 0;
         //if((room_data_buf[42] & 0x01) == 0){
         //   room_data_buf[42] |= 0x01;    //Emergency call
            if(!f_em_flag){
               f_em_flag = 1;    //Emergency call
               em_sign_keep_timer = 0;
               em_sign_timer = 0;
            }
        // }
      }
   }
   else{
      if(++emergency_chet > 1000){
         emergency_chet = 0;
         room_data_buf[42] &= 0xfe;    //Emergency clear
         //P_ex_led3 = 1;                  //
         f_em_flag = 0;
      }
   }
}

void wait_sign_check(void)      //임시 지정 포트
{
  if(!(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2))){
    if(++wait_sign_chet > 1000){
      wait_sign_chet = 0;
      room_data_buf[40] |= 0x80;    //window close
      wait_sign_timer = 0;
    }
  }
  else{
    wait_sign_chet = 0;
  }
}

//======= Relay out execution routine ==========
void relay_out_execution(void)
{
   switch(cb_mode)
   {
      case 0:  //온수밸브 3ea
      case 1:  //RCB 1,2,3,6
      case 4:
      
        if(room_data_buf[45] & 0x01) {
           P_rly_1 = 1;
           P_rly_2 = 1;
           P_rly_8 = 1;
        }
        else {
           P_rly_1 = 0;
           P_rly_2 = 0;
           P_rly_8 = 0;
        }
        
        if(room_data_buf[45] & 0x02) P_rly_3 = 1;
        else P_rly_3 = 0;
        
        if(room_data_buf[45] & 0x04) P_rly_4 = 1;
        else P_rly_4 = 0;
      
        if(room_data_buf[45] & 0x08) P_rly_5 = 1;
        else P_rly_5 = 0;
        
        if(room_data_buf[45] & 0x10) P_rly_6 = 1;
        else P_rly_6 = 0;
        
        if(room_data_buf[45] & 0x20) P_rly_7 = 1;
        else P_rly_7 = 0;
        
        
        if(room_data_buf[16] & 0x10) P_rly_11 = 0;     //거실/주방
        else P_rly_11 = 1;
        
        if(room_data_buf[17] & 0x10) P_rly_9 = 0;      //침실 1
        else P_rly_9 = 1;
        
        if(room_data_buf[18] & 0x10) P_rly_10 = 0;    //침실 2
        else P_rly_10 = 1;
        
      break;
      
      case 2:    //온수밸브 2ea
                     //RCB 4
        if(room_data_buf[45] & 0x01) {
           P_rly_1 = 1;
           P_rly_2 = 1;
           P_rly_8 = 1;
        }
        else {
           P_rly_1 = 0;
           P_rly_2 = 0;
           P_rly_8 = 0;
        }
        
        if(room_data_buf[45] & 0x02) P_rly_3 = 1;
        else P_rly_3 = 0;
        
        if(room_data_buf[45] & 0x04) P_rly_4 = 1;
        else P_rly_4 = 0;
      
        if(room_data_buf[45] & 0x08) P_rly_5 = 1;
        else P_rly_5 = 0;
        
        if(room_data_buf[45] & 0x10) P_rly_6 = 1;
        else P_rly_6 = 0;
        
        if(room_data_buf[45] & 0x20) P_rly_7 = 1;
        else P_rly_7 = 0;
        
        
        if(room_data_buf[16] & 0x10) P_rly_10 = 0;      //거실/주방
        else P_rly_10 = 1;
        
        if(room_data_buf[17] & 0x10) P_rly_9 = 0;        //침실 1
        else P_rly_9 = 1;
      
      break;
      
      case 3:    //온수밸브 4ea
      case 5:    //RCB 5,7,8
      
        if(room_data_buf[45] & 0x01) {
           P_rly_1 = 1;
           P_rly_2 = 1;
           P_rly_8 = 1;
        }
        else {
           P_rly_1 = 0;
           P_rly_2 = 0;
           P_rly_8 = 0;
        }
        
        if(room_data_buf[45] & 0x02) P_rly_3 = 1;
        else P_rly_3 = 0;
        
        if(room_data_buf[45] & 0x04) P_rly_4 = 1;
        else P_rly_4 = 0;
      
        if(room_data_buf[45] & 0x08) P_rly_5 = 1;
        else P_rly_5 = 0;
        
        if(room_data_buf[45] & 0x10) P_rly_6 = 1;
        else P_rly_6 = 0;
        
        if(room_data_buf[45] & 0x20) P_rly_7 = 1;
        else P_rly_7 = 0;
        
        
        if(room_data_buf[16] & 0x10) P_rly_12 = 0;    //거실/주방
        else P_rly_12 = 1;
        
        if(room_data_buf[17] & 0x10) P_rly_9 = 0;      //침실 1
        else P_rly_9 = 1;
        
        if(room_data_buf[18] & 0x10) P_rly_10 = 0;    //침실 2
        else P_rly_10 = 1;
        
        if(room_data_buf[19] & 0x10) P_rly_11 = 0;    //침실 3
        else P_rly_11 = 1;
        
      break;
   }
}


//========================================================================

