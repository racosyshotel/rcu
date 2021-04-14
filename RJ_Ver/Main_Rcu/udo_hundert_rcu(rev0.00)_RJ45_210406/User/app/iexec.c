/*=====================(C) COPYRIGHT 2014 PLUS-H.=========================
program		    : CONTROL BOX(RCS-300-RCU)
processor 	  : STM32F103xx
compiler		  : IAR 6.2A Compiler 
program BY	  : H.H.Hwang
date			    : 2014.	 .
copy right	  : P L U S  -  H. 
============================================================================*/
#define __IEXEC_H__

#include "stm32f10x_lib.h"
#include "main.h"


//========================================================================

//========================================================================
u8 iexecctrl=7;
void internal_exec_proc(void)  //�µ� ������ ���ۺκ�
{
  unsigned char i;
  
  adc_control_proc();
  
  key_sensor_check();
  enterance_lamp_switch_check();
  dnd_switch_check();
  mur_switch_check();
  chime_switch_check();
  door_sensor_check();
  window_sensor_check();
  emergency_switch_check();
  wait_sign_check();
  
 // Thermo_sensor_execution(0);
  
  switch(sq_count & 0x07){
   case 0: Thermo_sensor_execution(0); ++sq_count; break;  //++sq_count; break;
   case 1: Thermo_sensor_execution(1); ++sq_count; break;   //��� �µ� ������ 4�� ���
   case 2: Thermo_sensor_execution(2); ++sq_count; break;
   case 3: Thermo_sensor_execution(3); ++sq_count; break;
   case 4: Thermo_sensor_ondol_execution(0); ++sq_count; break;
   case 5: Thermo_sensor_ondol_execution(1); ++sq_count; break;
   case 6: Thermo_sensor_ondol_execution(2); ++sq_count; break;
   case 7: Thermo_sensor_ondol_execution(3); sq_count = 0; break;
 }
  fcu_4pipe_check();
  relay_out_execution();        //�µ������ relay�� ����ϱ� ������ ���⼭ �������
  
  /*
  for(i=0; i<8; ++i){
    if(room_data_buf[16+i] != room_back_data[i]){   //temp status change ?
      room_back_data[i] = room_data_buf[16+i];      //room data backup      
      room_data_buf[99] |= 0x02;                    //temp data main send(PC)
    }
  }*/
  
//  DAC_SetChannel1Data(DAC_Align_12b_R,4095);


/*  
 #define radian_per_degree              0.0174532925 
       signed int temp1 = 0;
     signed int temp2 = 0;
     unsigned int degree = 0;
  
  if(++degree > 360) degree = 0;
             temp1 = (2047 * cos(radian_per_degree * degree));
             temp1 = (2048 - temp1);
             DAC_SetChannel1Data(DAC_Align_12b_R,temp1);

//DAC_DHR12R1 = ((unsigned int)temp1);
//             DAC_DOR1 = ((unsigned int)temp1);
*/  
  
}







void fcu_4pipe_check(void)
{
  unsigned int set_temp_temp;
  unsigned int set_temp_temp_c;
  unsigned int set_temp_temp_w;
  unsigned int check_temp_temp;
  
  if(room_data_buf[74] == 2){   //ȯ���� ��� �ΰ�?
    if((room_data_buf[40] & 0x03) == 0){  //�����̰� ����ΰ��� ���Ǳ��ؿµ��� ��,�� ��ȯ�Ѵ�.
      set_temp_temp_c = (b_decimal_to_hex_table_ie[room_data_buf[72]]) * 10;       //�ù� ���� ���ؿµ�
      set_temp_temp_w = (b_decimal_to_hex_table_ie[room_data_buf[71]]) * 10;       //���� ���Ǳ��ؿµ�
      check_temp_temp = (room_data_buf[4+fcu_check_count] & 0x7f) * 10;              //��� ���� �µ� Ȯ��
      if(room_data_buf[4+fcu_check_count] & 0x80) check_temp_temp += 5;              //��� ���� �µ����� 0.5�� Ȯ��
      if(check_temp_temp > set_temp_temp_c){    //����µ��� �ù���� ���ؿµ����� ū��?
        room_data_buf[20+fcu_check_count] |= 0x04;  //�ù�����
      }
      else{
        if(check_temp_temp < set_temp_temp_w){  //����µ��� ���� ���� ���ؿµ����� ������?
          room_data_buf[20+fcu_check_count] &= 0xfb;  //��������
        }
      }
    }
    else{
      if(room_data_buf[83] == 0){
        set_temp_temp = (room_data_buf[0+fcu_check_count] & 0x7f) * 10;
        if(room_data_buf[0+fcu_check_count] & 0x80) set_temp_temp += 5;
        check_temp_temp = (room_data_buf[4+fcu_check_count] & 0x7f) * 10;
        if(room_data_buf[4+fcu_check_count] & 0x80) check_temp_temp += 5;

        if(check_temp_temp < set_temp_temp){
          if((set_temp_temp - check_temp_temp) > (room_data_buf[75]*10)){
            if(room_data_buf[20+fcu_check_count] & 0x04){
              room_data_buf[20+fcu_check_count] &= 0xfb;  //��������
              room_data_buf[20+fcu_check_count] |= 0x80;
            }
          }
        }
        else{
          if(check_temp_temp > set_temp_temp){
            if((check_temp_temp - set_temp_temp) > (room_data_buf[75]*10)){
              if(!(room_data_buf[20+fcu_check_count] & 0x04)){
                room_data_buf[20+fcu_check_count] |= 0x04;  //�ù�����
                room_data_buf[20+fcu_check_count] |= 0x80;
              }
            }
          }
        }
      }
    }      
    if(++fcu_check_count >= 4) fcu_check_count = 0;
  
  }
}







// ========== FCU control routine ===========
void Thermo_sensor_execution(unsigned char index)       //�µ����� ���๮
{
  if((room_data_buf[16+index] & 0x80) || flag_fcu_action_time[index]){ //�������? �Ǵ� ����ð�?
    if((room_data_buf[60+index] & 0x02) || (room_data_buf[20+index] & 0x04)){   //��������? �Ǵ� �ù��ΰ�?
      flag_fcu_action_time[index] = 0;
      room_data_buf[16+index] &= 0x7f;
      
      if(room_data_buf[40] & 0x02){       //�Խ�?
        if(room_data_buf[41] & 0x02)  going_out_temp_control(index);          //�ɾ�����?  //������ ����
        else{
          if(room_data_buf[16+index] & 0x04) stay_temp_control(index);        //�������   //�µ����� ����ġ ON?
          else{
            room_data_buf[20+index] &= 0xfc;                                  //���Ǹ�� ����
            fan_speed_control(0,index);
          }
        }
      }
      else{
        if(room_data_buf[40] & 0x01){     //Check IN?
          if(room_data_buf[16+index] & 0x04) going_out_temp_control(index);   //�µ����� ON�����̸� ������ ����  
          else{
            room_data_buf[20+index] &= 0xfc;                                  //���Ǹ�� ����
            fan_speed_control(0,index);
          }
        }
        else{
          if(room_data_buf[41] & 0x01) going_out_temp_control(index);         //����.����?  ������ ����
          else empty_temp_control(index);                                      //��������
        }
      }
      if((room_data_buf[20+index] & 0x03) != (fan_mode_back[index] & 0x03)){
        fan_mode_back[index] &= 0xfc;
        fan_mode_back[index] |= (room_data_buf[20+index] & 0x03);
        room_data_buf[99] |= 0x02;
      }
      
    }
  }
}




void stay_temp_control(unsigned char index)     //����϶� �µ������� ����
{
  unsigned int set_temp_temp;
  unsigned int check_temp_temp;
  
  set_temp_temp = (room_data_buf[0+index] & 0x7f) * 10;
  if(room_data_buf[0+index] & 0x80) set_temp_temp += 5;
  check_temp_temp = (room_data_buf[4+index] & 0x7f) * 10;
  if(room_data_buf[4+index] & 0x80) check_temp_temp += 5;
  
  room_data_buf[20+index] &= 0xfc;
  room_data_buf[20+index] |= 0x02;                //��� ��� ����
  
  if(room_data_buf[20+index] & 0x04){             //�ù� ����϶�
    if(check_temp_temp > set_temp_temp){
      if((check_temp_temp - set_temp_temp) > 9){    //9�� 0.9���� �ǹ���
        if((check_temp_temp - set_temp_temp) > 30)  fan_speed_control(3,index);     //Speed Hi
        else{
          if(((check_temp_temp - set_temp_temp) > 1) && ((check_temp_temp - set_temp_temp) <= 19))  fan_speed_control(1,index);            //Speed Low
          else if(((check_temp_temp - set_temp_temp) >= 20) && ((check_temp_temp - set_temp_temp) <= 29))  fan_speed_control(2,index);     //Speed Mid
          else fan_speed_control(3,index);     //Speed High
        }
      }
      else{
        if((room_data_buf[16+index] & 0x03) != 0) fan_speed_control(1,index);
      }
    }
    else{
      if(check_temp_temp < set_temp_temp) fan_speed_control(0,index);
    }
  }
  else{                                 //���� ���
    if(set_temp_temp > check_temp_temp){
      if((set_temp_temp - check_temp_temp) > 9){    //9�� 0.9���� �ǹ���
        if((set_temp_temp - check_temp_temp) > 30)  fan_speed_control(3,index);     //Speed Hi
        else{
          if(((set_temp_temp - check_temp_temp) > 1) && ((set_temp_temp - check_temp_temp) <= 19))  fan_speed_control(1,index);            //Speed Low
          else if(((set_temp_temp - check_temp_temp) >= 20) && ((set_temp_temp - check_temp_temp) <= 29))  fan_speed_control(2,index);     //Speed Mid
          else fan_speed_control(3,index);     //Speed High
        }
      }
      else{
        if((room_data_buf[16+index] & 0x03) != 0) fan_speed_control(1,index);
      }
    }
    else{
      if(set_temp_temp < check_temp_temp) fan_speed_control(0,index);
    }    
  }
}


void going_out_temp_control(unsigned char index) //����� �µ������� ����
{
  unsigned int set_temp_temp;
  unsigned int check_temp_temp;
  
  set_temp_temp = (room_data_buf[0+index] & 0x7f) * 10;   //�����µ� �ޱ�
  if(room_data_buf[0+index] & 0x80) set_temp_temp += 5;  //�����µ��� 0.5���� �ִ��� Ȯ��
  check_temp_temp = (room_data_buf[4+index] & 0x7f) * 10;
  if(room_data_buf[4+index] & 0x80) check_temp_temp += 5;
  
  room_data_buf[20+index] &= 0xfc;
  room_data_buf[20+index] |= 0x01;                //������ ��� ����
  
  if(room_data_buf[20+index] & 0x04){             //�ù� ���?
    set_temp_temp += room_data_buf[70];           //����µ��� ����
    if(check_temp_temp > set_temp_temp){
      if((check_temp_temp - set_temp_temp) > 9){    //9�� 0.9���� �ǹ���
        if((check_temp_temp - set_temp_temp) > 30)  fan_speed_control(3,index);     //Speed Hi
        else{
          if(((check_temp_temp - set_temp_temp) > 1) && ((check_temp_temp - set_temp_temp) <= 19))  fan_speed_control(1,index);            //Speed Low
          else if(((check_temp_temp - set_temp_temp) >= 20) && ((check_temp_temp - set_temp_temp) <= 29))  fan_speed_control(2,index);     //Speed Mid
          else fan_speed_control(3,index);     //Speed High
        }
      }
      else{
        if((room_data_buf[16+index] & 0x03) != 0) fan_speed_control(1,index);
      }
    }
    else{
      if(check_temp_temp < set_temp_temp) fan_speed_control(0,index);
    }
  }
  else{                                 //���� ���
    check_temp_temp += room_data_buf[70];           //����µ��� ����
    if(set_temp_temp > check_temp_temp){             
      if((set_temp_temp - check_temp_temp) > 9){    //9�� 0.9���� �ǹ���
        if((set_temp_temp - check_temp_temp) > 30)  fan_speed_control(3,index);     //Speed Hi
        else{
          if(((set_temp_temp - check_temp_temp) > 1) && ((set_temp_temp - check_temp_temp) <= 19))  fan_speed_control(1,index);            //Speed Low
          else if(((set_temp_temp - check_temp_temp) >= 20) && ((set_temp_temp - check_temp_temp) <= 29))  fan_speed_control(2,index);     //Speed Mid
          else fan_speed_control(3,index);     //Speed High �Ҽӵ� �����ϴ� �κ�
        }
      }
      else{
        if((room_data_buf[16+index] & 0x03) != 0) fan_speed_control(1,index); //���� �����ְ� �����µ��� ����µ��� ������ �� �ӵ��� 1�ܰԷ��Ѵ�.
      }
    }
    else{
      if(set_temp_temp < check_temp_temp) fan_speed_control(0,index); //�����µ��� ����µ����� ������ ���� ����
    }    
  }
}


void empty_temp_control(unsigned char index)    //�����ϋ� �µ������� ����(������ �µ����� ���ų� ũ�� �Ҽӵ��� high�� Ų��.
{
  unsigned int set_temp_temp;
  unsigned int check_temp_temp;
  
  check_temp_temp = (room_data_buf[4+index] & 0x7f) * 10;
  if(room_data_buf[4+index] & 0x80) check_temp_temp += 5;
  
  room_data_buf[20+index] &= 0xfc; //���� ��� ����
  
  if(room_data_buf[20+index] & 0x04){             //�ù� ���?
    set_temp_temp = (b_decimal_to_hex_table_ie[room_data_buf[72]]) * 10;       //�ù� ���� ���ؿµ�
    if(check_temp_temp > set_temp_temp){
      if((check_temp_temp - set_temp_temp) > 9){
        fan_speed_control(3,index);     //Speed High
      }
    }
    else{
      if(check_temp_temp < set_temp_temp) fan_speed_control(0,index);
    }
  }
  else{                                 //���� ���
    set_temp_temp = (b_decimal_to_hex_table_ie[room_data_buf[71]]) * 10;           //���� ���� ���ؿµ�
    if(check_temp_temp < set_temp_temp){
      if((set_temp_temp - check_temp_temp) > 9){
        fan_speed_control(3,index);     //Speed High
      }
    }
    else{
      if(check_temp_temp > set_temp_temp) fan_speed_control(0,index);
    }
  }
}


void fan_speed_control(unsigned char speed_value, unsigned char index)
{
  if(room_data_buf[54+index] == 0){
    switch(speed_value){
      case 0: room_data_buf[16+index] &= 0xfc;  break;
      case 1: room_data_buf[16+index] &= 0xfc;  room_data_buf[16+index] |= 0x01; break;
      case 2: room_data_buf[16+index] &= 0xfc;  room_data_buf[16+index] |= 0x02; break;
      case 3: room_data_buf[16+index] &= 0xfc;  room_data_buf[16+index] |= 0x03; break;
    }
  }
  else{
    if(speed_value == 0) room_data_buf[16+index] &= 0xfc;
    else{
      switch(room_data_buf[54+index] & 0x03){
        case 1: room_data_buf[16+index] &= 0xfc;  room_data_buf[16+index] |= 0x01; break;
        case 2: room_data_buf[16+index] &= 0xfc;  room_data_buf[16+index] |= 0x02; break;
        case 3: room_data_buf[16+index] &= 0xfc;  room_data_buf[16+index] |= 0x03; break;
      }
    }
  }
}

//============= �ٴ� �µ� ���� routine ================
//
void Thermo_sensor_ondol_execution(unsigned char index)
{
    //if((room_data_buf[60+index] & 0x01) && !(room_data_buf[20+index] & 0x04) && !(room_data_buf[74] ==2)){     //�ѽ�? �̰� �����ΰ�? ȯ�����尡 �ƴѰ�?
      if(flag_floor_action_time[index] || (room_data_buf[20+index] & 0x80)){
        room_data_buf[20+index] &= 0x7f;
        flag_floor_action_time[index] = 0;
        
          if(room_data_buf[40] & 0x02){     //�Խ��ΰ�?
            if(room_data_buf[41] & 0x02) going_out_ondol_control(index);      //�ɾ������ΰ�?
            else{
              if(room_data_buf[16+index] & 0x20)  stay_ondol_control(index);  //������� //�µ�����ġ�� ON�ΰ�?
              else{
                ondol_off(index);
                room_data_buf[20+index] &= 0xcf;    //���Ǹ�� ����
              }
            }
          }
          else{
            if(room_data_buf[40] & 0x01){   //Check IN�ΰ�?
              if(room_data_buf[16+index] & 0x20) going_out_ondol_control(index);    //������ ���� //�µ�����ġ�� ON�ΰ�?
              else{
                ondol_off(index);
                room_data_buf[20+index] &= 0xcf;    //���Ǹ�� ����
              }
            }
            else{
              if(room_data_buf[41] & 0x01)  going_out_ondol_control(index);    //������ ���� //����.����
              else empty_ondol_control(index); //��������
            }
          }
      
    }
    if((room_data_buf[20+index] & 0x04) != (air_floor_mode_back[index] & 0x04))
    {
      air_floor_mode_back[index] &= 0xfb;
      air_floor_mode_back[index] = (room_data_buf[20+index] & 0x04);
      if(air_floor_mode_back[index] & 0x04)     //�ù��϶�
      {
        ondol_off(index);                       //�ٴڳ��� off
        room_data_buf[20+index] &= 0xcf;        //���Ǹ�� ����
      }
    }
    
    if((room_data_buf[20+index] & 0x30) != (fan_mode_back[index] & 0x30)){
      fan_mode_back[index] &= 0xcf;
      fan_mode_back[index] |= (room_data_buf[20+index] & 0x30);
      room_data_buf[99] |= 0x04;
    }
}



void stay_ondol_control(unsigned char index)
{
  unsigned int set_temp_temp;
  unsigned int check_temp_temp;
  
  room_data_buf[20+index] &= 0xcf;
  room_data_buf[20+index] |= 0x20;
  
  set_temp_temp = (room_data_buf[8+index] & 0x7f) * 10;
  if(room_data_buf[8+index] & 0x80) set_temp_temp += 5;
  check_temp_temp = (room_data_buf[12+index] & 0x7f) * 10;
  if(room_data_buf[12+index] & 0x80) check_temp_temp += 5;
 
  if(set_temp_temp < check_temp_temp) ondol_off(index);
  else{
    if((set_temp_temp - check_temp_temp) >= 10) ondol_on(index);
  }
}

void going_out_ondol_control(unsigned char index)
{
  unsigned int set_temp_temp;
  unsigned int check_temp_temp;
  
  room_data_buf[20+index] &= 0xcf;
  room_data_buf[20+index] |= 0x10;
  
  set_temp_temp = (room_data_buf[8+index] & 0x7f) * 10;
  if(room_data_buf[8+index] & 0x80) set_temp_temp += 5;
  check_temp_temp = (room_data_buf[12+index] & 0x7f) * 10;
  if(room_data_buf[12+index] & 0x80) check_temp_temp += 5;
  check_temp_temp += room_data_buf[70];   //����µ����� ����.
  
  if(set_temp_temp < check_temp_temp) ondol_off(index);
  else{
    if((set_temp_temp - check_temp_temp) >= 10) ondol_on(index);
  }
}

void empty_ondol_control(unsigned char index)
{
  unsigned int set_temp_temp;
  unsigned int check_temp_temp;
  
  room_data_buf[20+index] &= 0xcf;
  
  set_temp_temp = (b_decimal_to_hex_table_ie[room_data_buf[73]]) * 10;          //�ٴڰ��Ǳ��ؿµ��� ����
  check_temp_temp = (room_data_buf[12+index] & 0x7f) * 10;
  if(room_data_buf[12+index] & 0x80) check_temp_temp += 5;
  
  if(set_temp_temp < check_temp_temp) ondol_off(index);
  else{
    if((set_temp_temp - check_temp_temp) >= 10) ondol_on(index);
  }
}


void ondol_on(unsigned char index)
{
  room_data_buf[16+index] |= 0x10;
  room_data_buf[99] |= 0x04;
}
void ondol_off(unsigned char index)
{
  room_data_buf[16+index] &= 0xef;
  room_data_buf[99] |= 0x04;
}
//========================================================================
//========================================================================

