/*=====================(C) COPYRIGHT 2008 Insem Inc.=========================
program 		: HDPVR FRONT PANEL 
processor	  : STM32F103C6T
compiler		: IAR 4.41A Compiler 
program BY 	: H.H.HWANG
date 			  : 2008.	  .
copy right 	: PLUS-H
=======================================================================*/
void UART2_control_proc(void);
void UART2_initial(void);
void UART2_tx_check(void);
void UART2_data_set(unsigned char,unsigned char);
void Thermo_sensor_data_set(unsigned char index, unsigned char index_bit);
void UART2_rx_check(void);
void thermo_sensor_check(unsigned char index);
void ts_speed_execution(unsigned char index);
void ts_temp_up_excution(unsigned char index, unsigned char ts_mode);
void ts_temp_down_excution(unsigned char index, unsigned char ts_mode);
void night_table_check(unsigned char index);
void nt_switch_bit_check(unsigned char key_code, unsigned char index);
void light_switch_data_set(unsigned char index, unsigned int index_bit);
void light_switch_check(unsigned char index);
void key_sendor_data_set(unsigned char index,unsigned char index_bit);
void key_sensor_rx_check(unsigned char index);
void ex_relay_data_set(unsigned char index, unsigned char index_bit);
void ex_input_data_set(unsigned char index, unsigned char index_bit);
void volume_jig_data_set(void);
void volume_data_check(void);
void dimmer_data_set(void);
void dimmer_check(unsigned char index);
void audio_data_set(void);
void common_data_set(void);
void chime_ind_check(unsigned char index);
void chime_ex_sub_check(void);
void chime_ind_data_set(void);
void samsung_aircon_data_send(void);
void exinput_check(unsigned char index);

unsigned char ts_status_read(unsigned char index);
unsigned char ts_control_data_read(unsigned char index);
unsigned char nt_control_data_read(unsigned char index);
unsigned char nt_room_status(void);
unsigned char ls_control_data_read(unsigned char index);
unsigned char ks_control_data_read(unsigned char index);
unsigned char ts_control_data_read_samsung_aircon(unsigned char index);

// giga gini //
unsigned char room_status(void);
unsigned char ts_status(unsigned char index);
unsigned char ts_set_temp(unsigned char index);
unsigned char ts_check_temp(unsigned char index);
unsigned char light_status(void);
unsigned char curtain_status(void);
void giga_gini(void);
void giga_gini_receive(void);
//////////////

//통신 쪽보드 RS485 CONV
void ex_485_send(unsigned char index);
void ex_485_receive(void);

//=======================================================================

#define P_UART2_DIR		  GpioA->Bit.b1

//=======================================================================

//=======================================================================


#define b_max_id 10

u1EX u8 uart2_tx_length;
u1EX u8 uart2_tx_backup;
u1EX u8 uart2_tx_data_buf[100];

u1EX u8 uart2_rxding_point;
u1EX u8 uart2_rxding_buf[100];
u1EX u8 uart2_rxd_buf[100];
u1EX u8 id_number;

u1EX u8 nt_switch_toggle_bit[4][4];
u1EX u8 nt_status[4];

u1EX u8 light_switch_toggle_bit[8][3];
u1EX u8 exinput_toggle_bit[8][3];
u1EX u8 ks_switch_toggle_bit[4];

u1EX u8 f_uart2_frame_rx_ok;
u1EX u8 f_ts_power_on[4];

u1EX u8 jig_switch_toggle_bit;
u1EX u8 f_first_input_power_on[];

u1EX u8 time_year;
u1EX u8 time_month;
u1EX u8 time_day;
u1EX u8 time_hour;
u1EX u8 time_min;
u1EX u16 ks_rx_timer;
u1EX u8 ks_key_status;

u1EX u8 key_status_data;  // K/S 입력 연동 쪽보드용 변수


u1EX u8 dimmer_level[16];
u1EX u8 audio_volume[4];

u1EX u8 chime_ind_toggle_bit[4];
u1EX u8 f_first_ci_power_on[4];

u1EX u8 em_temp_count[8];     //이상온도 발생 counter
u1EX u8 ks_status_count;
u1EX u8 ks_key_status_back;
u1EX u16 timer_cnt;

u1EX u8 ts_control_mode;    

u1EX u8 bojung_temp[8];

u1EX u8 device_comm_count[8];
u1EX u8 aircon_mode[5];
u1EX u8  f_first_time;
u1EX u8  key_sensor_use_bit;

u1EX u8 step_count[3];
u1EX u8 f_first_dimmer_power_on[4];

u1EX u8 dimmer_switch_toggle_bit[4];
u1EX u8 ts_number;
u1EX u8 backup_data[30];
u1EX u8 ts_index;
u1EX u8 data_delay;
u1EX u8 ts_set_index;
u1EX u8 fan_speed_flag;
u1EX u8 fan_control_flag;
u1EX u8 gini_data_length;
u1EX u8 gini_data_code;
u1EX u8 data_processing_length;

//DTC Dip switch 기능 업데이트 [21.01.05]
u1EX u8 led_bright_control;                          //LED 밝기조절 

//라이트스위치 통신 장애 파악 변수설정
u1EX u8 light_switch_falling_count[16];         //총 16개의 라이트스위치 설정
u1EX u8 nt_device_falling_count[4];
u1EX u8 ci_device_falling_count[4];
u1EX u8 ks_device_falling_count[4];
u1EX u8 ts_device_falling_count[4];


u1EX __dwbits uart2_flag1;
#define f_uart2_data_send			uart2_flag1.s.b0
#define f_uart2_com_start			uart2_flag1.s.b1
//#define f_uart2_frame_rx_ok		uart2_flag1.s.b2
#define f_uart2_send_time 		uart2_flag1.s.b3

u1EX __dwbits uart2_flag2;
#define f_alram_on      			uart2_flag2.s.b0

u1EX __dwbits uart2_flag3;
#define f_ks_rx_ok      			uart2_flag3.s.b0
#define f_ks_error      			uart2_flag3.s.b1
#define f_check_temp_change 	uart2_flag3.s.b2
#define f_check_temp_change_2   uart2_flag3.s.b3

//curtain define
u1EX u8 curtain_sw_buf[10];
u1EX u8 curtain_control;

#define B_KS_CODE           0x00
#define B_TEMP_CODE         0x10
#define B_NIGHT_CODE        0x20
#define B_LIGHT_CODE        0x30

#define B_KEY_SENSOR_0       0
#define B_TEMP_SENSOR_1      1
#define B_TEMP_SENSOR_2      2
#define B_TEMP_SENSOR_3      3
#define B_TEMP_SENSOR_4      4
#define B_NIGHT_TABLE_1         5
#define B_NIGHT_TABLE_2         6
#define B_LIGHT_SWITCH_1        7
#define B_LIGHT_SWITCH_2        8
#define B_LIGHT_SWITCH_3        9
#define B_LIGHT_SWITCH_4        10
#define B_DIMMER                11
#define B_AUDIO                 12
#define B_CHIME_IND             13

#define B_LIGHT_SWITCH_5        14
#define B_LIGHT_SWITCH_6        15
#define B_LIGHT_SWITCH_7        17
#define B_EXRELAY               19
#define B_EXINPUT               20
#define B_GIGA          21
#define B_LIGHT_SWITCH_8        22
#define B_LIGHT_SWITCH_9        23
#define B_LIGHT_SWITCH_10        24
#define B_LIGHT_SWITCH_11        25
#define B_LIGHT_SWITCH_12        26
#define B_LIGHT_SWITCH_13        27
#define B_LIGHT_SWITCH_14        28
#define B_LIGHT_SWITCH_15        29
#define B_LIGHT_SWITCH_16        30
#define B_EX_485                        31

#ifdef __UART2_H__
const u8 b_sub_device_table[][25] = {           //우도 훈데르트 (RJ)
  {B_CHIME_IND,B_NIGHT_TABLE_1,B_TEMP_SENSOR_2,B_TEMP_SENSOR_3,B_KEY_SENSOR_0,0xff,0xff,0xff,0xff }, //RCB-1,2 (객실,장애인실)
  {B_CHIME_IND,B_NIGHT_TABLE_1,B_TEMP_SENSOR_2,B_TEMP_SENSOR_3,B_KEY_SENSOR_0,0xff,0xff,0xff,0xff }, //RCB-3 (vip 1)
  {B_CHIME_IND,B_NIGHT_TABLE_1,B_TEMP_SENSOR_2,B_KEY_SENSOR_0,0xff,0xff,0xff,0xff },                              //RCB-4 (vip 2)
  {B_CHIME_IND,B_NIGHT_TABLE_1,B_TEMP_SENSOR_2,B_TEMP_SENSOR_3,B_TEMP_SENSOR_4,B_KEY_SENSOR_0,0xff,0xff,0xff,0xff },   //RCB-5,7 (빌리지 A,C)
  {B_CHIME_IND,B_NIGHT_TABLE_1,B_TEMP_SENSOR_2,B_TEMP_SENSOR_3,B_KEY_SENSOR_0,0xff,0xff,0xff,0xff }, //RCB-6 (빌리지 B)
  {B_CHIME_IND,B_NIGHT_TABLE_1,B_TEMP_SENSOR_2,B_TEMP_SENSOR_3,B_TEMP_SENSOR_4,B_KEY_SENSOR_0,0xff,0xff,0xff,0xff }, //RCB-8 (빌리지 D)
};

#define K_stand_master    0x10
#define K_stand_1         0x11
#define K_stand_2         0x12
#define K_stand_3         0x13
#define K_stand_4         0x14
#define K_stand_5         0x15
#define K_stand_6         0x16
#define K_stand_7         0x17
#define K_stand_8         0x18
#define K_entry_lamp      0x19
#define K_bath_master     0x1a
#define K_exchanger       0x1b
#define K_bed_master      0x1c
#define K_stand_master_2    0x1d
#define K_stand_9           0x1e
#define K_stand_10           0x1f
#define K_stand_11           0x25
#define K_stand_master_3 0x26
#define K_bath_master_2   0x27
#define K_stand_12          0x28
#define K_stand_13          0x29
#define K_stand_14          0x35
#define K_stand_15          0x36
#define K_stand_16          0x37
#define K_stand_master_4    0x38

#define K_scene_1         0x20
#define K_scene_2         0x21
#define K_scene_3         0x22
#define K_scene_4         0x23
#define K_scene_5         0x24

#define K_radio_onoff     0x30
#define K_channel_up      0x31
#define K_channel_down    0x32
#define K_volume_up       0x33
#define K_volume_down     0x34

#define K_fcu_onoff       0x40
#define K_fcu_low         0x41
#define K_fcu_mid         0x42
#define K_fcu_high        0x43
#define K_fcu_auto_manual 0x44

#define K_dnd             0x50
#define K_mur             0x51
#define K_call_wait       0x52
#define K_emergency       0x53
#define K_chime_push      0x54

#define K_dim_step_up     0x80
#define K_dim_step_down   0x81

#define K_none            0xff

#define K_curtain_1_open       0x82
#define K_curtain_1_stop       0x83
#define K_curtain_1_close      0x84
#define K_curtain_2_open       0x85
#define K_curtain_2_stop       0x86
#define K_curtain_2_close      0x87


const u8 b_switch_kind_table[][5][25] = //우도훈데르트 04.08   [5]->[6]
{
  {
    {K_stand_1, K_stand_2, K_stand_3, K_stand_4, K_stand_5, K_stand_5},               //RCB-1,2(101동 객실)
  }, 
  {     
    {K_stand_1, K_stand_2, K_stand_3, K_stand_4, K_stand_5, K_stand_master},        //RCB-3(101동 VIP1)
  },
  {
    {K_stand_1, K_stand_2, K_stand_3, K_stand_4, K_stand_5, K_stand_master},        //RCB-4(101동 VIP2) 
  },
  {
    {K_stand_1, K_stand_2, K_stand_3, K_stand_3, K_stand_master, K_stand_master}, //RCB-5,7(빌리지 A,C)
  },
  {
    {K_stand_1, K_stand_2, K_stand_3, K_stand_3, K_stand_master, K_stand_master}, //RCB-6(빌리지 B)
  },
  {
     {K_stand_1, K_stand_2, K_stand_3, K_stand_4, K_stand_5, K_stand_master}         //RCB-8(빌리지 D)
  },
};
const u8 b_exinput_switch_kind_table[][6][30] = //외부접점용
{
  {
    {0},                                                 
  }, 
  
  {
    {0},                                                      
  }, 
  
  {
    {0},                                                 
  },
};

const u8 b_ls_switch_kind_table[][6][8] = 
{
  {
     {0}
  }, 
  
  {
     0
  }, 
};
const u8 b_ts_switch_kind_table[][6][8] = 
{
  0
};

const u8 b_dim_master_point_table[][16] = {
  {0,1,2,3,0xff},
};

const u8 b_dimmer_step[11] = {
  0, 9, 19, 29, 39, 49, 59, 69, 79, 89, 99
};

const u8 b_scene_table[][16] = {
  {10, 50, 20, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {20, 40, 20, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {30, 30, 20, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {40, 20, 20, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

const u8 b_volume_step[] = {
  0x02, 0x0c, 0x14, 0x27, 0x40, 0x6c, 0x90, 0xb0, 0xda, 0xfe, 0xff
};









#endif



//=======================================================================

//=======================================================================

