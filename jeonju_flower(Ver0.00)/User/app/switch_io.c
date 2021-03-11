/*=====================(C) COPYRIGHT 2008 Insem Inc.=========================
program 		: HDPVR FRONT PANEL 
processor		: STM32F101xx
compiler		: IAR 4.41A Compiler 
program BY 	: YN.Kim
date 			: 2008.	  .
copy right 	: Insem Inc.
===========================================================================*/
#define __SWITCH_IO_H__

#include "stm32f10x_lib.h"
#include "main.h"

//=== Key sensor check routine ===
void key_sensor_check(void)
{
  unsigned char key_temp = 0;
  
    if((dip_switch_buf[0] & 0x40) == 0){    //Direct key sensor check
      if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)) key_temp |= 0x01;
      if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)) key_temp |= 0x02;
      
      if(key_temp == key_sensor_back)
      {
        if(key_sensor_chet > 500){    //500msec
          key_sensor_chet = 0;
          if(key_temp != 2) f_clean_wait = 0; //청소완료 아니면 flag clear   
          switch(key_temp)
          {
            case 0: room_data_buf[40] &= 0xfd;  room_data_buf[49] = 0;  guest_out_executiom(); break;    //외출처리
            case 1: room_data_buf[40] |= 0x02;  room_data_buf[49] = 1;  guest_in_execution(); break;     //청소중
            case 2: room_data_buf[40] &= 0xfd;
                         f_clean_wait = 1; //clean flag on
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
      else
      {
        key_sensor_back = key_temp;
        key_sensor_chet = 0;
      }
    }
    else{                                   //통신형 key sensor execution
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
              case 0x20: room_data_buf[40] &= 0xfd;  room_data_buf[49] = 2;  f_clean_finish = 1;  guest_out_executiom(); break;    //청소완료
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
  
  room_data_buf[45] |= 0x01;  //P
  if(!f_first_room_inout)
  {
    f_first_room_inout = 1;
    flag_fcu_action_time[0] = 1;      //FCU 즉시 실행
    flag_fcu_action_time[1] = 1;
    flag_fcu_action_time[2] = 1;
    flag_fcu_action_time[3] = 1;
    flag_floor_action_time[0] = 1;    //온돌 즉시 실행
    flag_floor_action_time[1] = 1;
    flag_floor_action_time[2] = 1;
    flag_floor_action_time[3] = 1;
    
    room_data_buf[45] = 0xff;         //Key IN시 점등되는 전등
    //room_data_buf[46] = 0xff;        //Key IN시 점등되는 전등
    if(f_bath_control) room_data_buf[46] = 0xff;
    room_data_buf[47] = 0xff;
    room_data_buf[50] = 0xff;
    room_data_buf[51] = 0xff;
    room_data_buf[52] = 0xff;
    room_data_buf[53] = 0xff;
    f_enterance_lamp_delay = 0;
    enterance_lamp_delay_timer = 0;   //입구등 Time Delay clear
  }     
}

//Guest OUT execution routine 
void guest_out_executiom(void)
{
  unsigned char i;
  
  if(f_first_room_inout)
  {
    //f_first_room_inout = 0;
    room_data_buf[36] &= 0xf7;        //Radio off
    room_data_buf[37] &= 0xf7;        //Radio off
    room_data_buf[38] &= 0xf7;        //Radio off
    room_data_buf[39] &= 0xf7;        //Radio off
    
    room_data_buf[54] = 0;
    room_data_buf[55] = 0;
    room_data_buf[56] = 0;
    room_data_buf[57] = 0;    
  }
  if(flag_relay_all_off_delay_timer){ //10sec delay
    room_data_buf[45] = 0;            //Relay All OFF
    room_data_buf[46] = 0;
    room_data_buf[47] = 0;
    room_data_buf[50] = 0;
    room_data_buf[51] = 0;
    room_data_buf[52] = 0;
    room_data_buf[53] = 0;
    
    f_first_room_inout = 0;
    for(i=0; i<16; ++i) dimmer_level[i] = 0;
  }
}

// Enterance lamp switch check routine 
void enterance_lamp_switch_check(void)
{
  if(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)))
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
                     //P_dnd_led = 0;
                     //P_mur_led = 1;
                     P_ind_led = 0;
                     ++led_count;
                     break;
                     
                   case 1:
                     //P_dnd_led = 1;
                     //P_mur_led = 0;
                     P_ind_led = 1;
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
                //P_dnd_led = 0;
                //P_mur_led = 0;      
                P_ind_led = 0;
          }
      }
    }
    else
    {
        lead_chet = 0;
        f_lead_sw = 0;
    }
  
  /*if(room_data_buf[40] & 0x02){             //입실인가?
    if(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)))
    {
      if(++enterance_chet > 1000)
      {
        if(!f_enterance_push)
        {
          f_enterance_push = 1;
          //room_data_buf[45] ^= 0x02;        //입구등 점등
          if(room_data_buf[45] & 0x02)
          {
            f_enterance_lamp_delay = 0;
            enterance_lamp_delay_timer = 0;   //입구등 Time Delay clear
          }
        }
      }
    }
    else
    {
      enterance_chet = 0;
      f_enterance_push = 0;
    }
  }*/
  
}

void dnd_switch_check(void)
{ 
  if(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)))
    {
      if(room_data_buf[40] & 0x02) //재실인가??
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

//MUR Switch check
void mur_switch_check(void)
{
    
    if(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)))
    {
      if(room_data_buf[40] & 0x02) //재실인가??
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
    else
    {
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

void chime_switch_check(void)
{
  if(!f_lead_sw)
  {     
    if(room_data_buf[40] & 0x08) //MUR LED
    {
        P_mur_led = 0;
        P_dnd_led = 1;
    }
    else P_mur_led = 1;
    
    if((f_dnd_con == 1) && (room_data_buf[40] & 0x04)) //DND 일때 차임벨 눌릴시 DND 깜박임 
    {
         f_dnd_sign = 1;
         
         if(f_dnd_ok)
         {
             f_dnd_ok = 0;
             P_dnd_led ^= 1;
             dnd_stop_count++;
             
             if(dnd_stop_count > 6) 
             {
               dnd_stop_count = 0;
               f_dnd_sign = 0;
               f_dnd_con = 0;
             }
         }    
    }  
    else if(room_data_buf[40] & 0x04) //DND LED
    {
        P_mur_led = 1;
        P_dnd_led = 0;        
    }  
    else P_dnd_led = 1;
            
    if(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))) //차임벨 동작 
    {
      if(++chime_chet > 500)
      {
        P_ind_led = 0;
        if(room_data_buf[40] & 0x04) f_dnd_con = 1;
        
        if(!(room_data_buf[40] & 0x04))  //DND가 아니면 동작
        {     
          if(!f_chime_push)
          {
            f_chime_push = 1;
            if(!f_chime_keep)
            {
              f_chime_keep = 1;
              chime_sq = 0;
              f_chime_sq_timer = 1;
              digital_volume_execution(0,room_data_buf[79]);
              
              handy_sign_timer = 0;
              
              if(room_data_buf[40] & 0x02) //입실일때
              {
                room_data_buf[48] |= 0x80;
                f_chime_sign = 1;
              }
            }
          }
        }
      }
    }
    else
    {
      chime_chet = 0;
      f_chime_push = 0;
      P_ind_led = 1;
    }
  }
}

void door_sensor_check(void)
{
  
  if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6))){
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

void emergency_switch_check(void)
{
  if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)))
  {
      if(++emergency_chet > 1000)
      {
        emergency_chet = 0;
       if((room_data_buf[42] & 0x01) == 0)
       {
          if(!f_em_flag)
          {
            f_em_flag = 1;    //Emergency call
            
            em_sign_keep_timer = 0;
            em_sign_timer = 0;
          }
          
      }
    }
  }
    else
    {
      if(++emergency_chet > 1000)
      {
        emergency_chet = 0;
        P_ex_led4 = 1;              //ttt
        room_data_buf[42] &= 0xfe; // Emergency clear
        f_em_flag = 0;
      }
    }
}

void wait_sign_check(void)      //임시 지정 포트
{
  /*
  if(!(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)))
  {
    if(++wait_sign_chet > 1000)
    {
      wait_sign_chet = 0;
      room_data_buf[40] |= 0x80;    //window close
      wait_sign_timer = 0;              
    }
  }
  else
  {
    wait_sign_chet = 0;
  }
  */
}

void light_ex_input(void)
{
  if(room_data_buf[40] & 0x02) // 입실
  {
    // ** EX IN PUT 0 ** //
    if(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1))) // cn5. 1
    {
      if(!ex_in_flag1)
      {
        if(++ex_chet1 > 500)
        {
           ex_chet1 = 0;
           ex_in_flag1 = 1;

        }
      }
    }
    else 
    {
      ex_chet1 = 0;
      ex_in_flag1 = 0;
    }
    //*****************//     
    
    // ** EX IN PUT 1 ** //
    if(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))) // cn5. 2
    {
      if(!ex_in_flag2)
      {
        if(++ex_chet2 > 500)
        {
           ex_chet2 = 0;
           ex_in_flag2 = 1;
                 
        }
      }
    }    
    else 
    {
      ex_chet2 = 0;
      ex_in_flag2 = 0;
    }
    //*****************//  
    
    // ** EX IN PUT 2 ** //
    if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5))) // cn5. 3
    {
      if(!ex_in_flag3)
      {
        if(++ex_chet3 > 500)
        {
           ex_chet3 = 0;
           ex_in_flag3 = 1;
           

        }
      }
    }    
    else 
    {
      ex_chet3 = 0;
      ex_in_flag3 = 0;
    }
    //*****************// 

    // ** EX IN PUT 3 ** //
    if(!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4))) // cn5. 4
    {
      if(!ex_in_flag4)
      {
        if(++ex_chet4 > 500)
        {
           ex_chet4 = 0;
           ex_in_flag4 = 1;
           

        }
      }
    }    
    else 
    {
      ex_chet4 = 0;
      ex_in_flag4 = 0;
    }
    //*****************// 
    
    // ** EX IN PUT 4 ** //
    if(!(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7))) // cn5. 5
    {
      if(!ex_in_flag5)
      {
        if(++ex_chet5 > 500)
        {
           ex_chet5 = 0;
           ex_in_flag5 = 1;
           

        }
      }
    }    
    else 
    {
      ex_chet5 = 0;
      ex_in_flag5 = 0;
    }
    //*****************//   
    
    // ** EX IN PUT 5 ** //
    if(!(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))) // cn5. 6
    {
      if(!ex_in_flag6)
      {
        if(++ex_chet6 > 500)
        {
           ex_chet6 = 0;
           ex_in_flag6 = 1;
           
        }
      }
    }    
    else 
    {
      ex_chet6 = 0;
      ex_in_flag6 = 0;
    }
    //*****************//         
    
    
  } 
}

//======= Relay out execution routine ==========
void relay_out_execution(void)
{
  switch(cb_mode)
  {
    case 0: //RCB - 1 (장애인실)
      
      if(room_data_buf[45] & 0x01) //KEY IN
      {          
          P_rly_2 = 1;  //전열 A                  - B접점
          P_rly_3 = 1;  //일반전등
      }
      else
      {             
          P_rly_2 = 0;  
          P_rly_3 = 0;
      }
      
      if(room_data_buf[45] & 0x02) P_rly_4 = 1; //(LIGHT 1)ROOM
      else P_rly_4 = 0;
      
      if(room_data_buf[45] & 0x04) P_rly_6 = 1; //(LIGHT 2)ROOM
      else P_rly_6 = 0;
      
      if(room_data_buf[45] & 0x08) P_rly_8 = 1; //(LIGHT 3)ROOM
      else P_rly_8 = 0;
      
      if(room_data_buf[45] & 0x10) P_rly_5 = 1; //(LIGHT 4)ROOM
      else P_rly_5 = 0;
      
      if(room_data_buf[45] & 0x20) P_rly_7 = 1; //(LIGHT 5)ROOM
      else P_rly_7 = 0;
      
      if(room_data_buf[16] & 0x10) P_rly_1 = 0;  // 바닥난방          - A접점
      else P_rly_1 = 1;
      
    break;
    
  case 1: //RCB - 2 
  
      if(room_data_buf[45] & 0x01) //KEY IN
      {
          P_rly_2 = 1;  //전열 A                  - B접점
          P_rly_3 = 1;  //일반전등        
      }
      else
      {
          P_rly_2 = 0;  
          P_rly_3 = 0;
      }
      
      if(room_data_buf[45] & 0x02) P_rly_4 = 1; //(LIGHT 1)ROOM
      else P_rly_4 = 0;
      
      if(room_data_buf[45] & 0x04) P_rly_6 = 1; //(LIGHT 2)ROOM
      else P_rly_6 = 0;
      
      if(room_data_buf[45] & 0x08) P_rly_5 = 1; //(LIGHT 3)ROOM
      else P_rly_5 = 0;
      
      if(room_data_buf[45] & 0x10) P_rly_7 = 1; //(LIGHT 4)ROOM
      else P_rly_7 = 0;
      
      
      if(room_data_buf[47] & 0x01) P_rly_8 = 1;   //(LIGHT 5)LIVING
      else P_rly_8 = 0;
      
      if(room_data_buf[47] & 0x02) P_rly_11 = 1; //(LIGHT 6)LIVING
      else P_rly_11 = 0;
      
      if(room_data_buf[47] & 0x04) P_rly_10 = 1; //(LIGHT 7)LIVING
      else P_rly_10 = 0;
      
      if(room_data_buf[47] & 0x08) P_rly_12 = 1; //(LIGHT 8)LIVING
      else P_rly_12 = 0;
      
      if(room_data_buf[16] & 0x10) P_rly_1 = 0;  // 바닥난방          - A접점
      else P_rly_1 = 1;
      
      if(room_data_buf[17] & 0x10) P_rly_9 = 0;  // 바닥난방          - A접점
      else P_rly_9 = 1;
      
    break;
  }
}