/*=====================(C) COPYRIGHT 2014 PLUS-H.=========================
program		    : CONTROL BOX(RCS-300-RCU)
processor 	  : STM32F103xx
compiler		  : IAR 6.2A Compiler 
program BY	  : H.H.Hwang
date			    : 2014.	 .
copy right	  : P L U S  -  H. 
============================================================================*/
void UART1_control_proc(void);
void UART1_initial(void);

void UART1_tx_check(void);
void UART1_data_set(void);
unsigned char cb_basic_data(unsigned char index);
void cb_data_send(unsigned char send_cmd);
unsigned char air_temp_control_status(unsigned char index);
unsigned char floor_temp_control_status(unsigned char index);
void main_rxd_check(void);



//=======================================================================

#define P_UART1_DIR		  GpioA->Bit.b8

//=======================================================================


//=======================================================================

#ifdef __UART1_H__

const u8 b_decimal_to_hex_table[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
  0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
  0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
  0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
  0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
  0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
  0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
  0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee
};


#define u1EX 




#else
#define u1EX extern


#endif

u1EX u8 uart1_tx_length;
u1EX u8 uart1_tx_backup;
u1EX u8 uart1_tx_data_buf[50];

u1EX u8 uart1_rxding_point;
u1EX u8 uart1_rxding_buf[50];
u1EX u8 uart1_rxd_buf[50];

u1EX u8 uart1_tx_send_buf[50];
u1EX u8 uart1_delay_dir;


u1EX u8 guest_UID_no[10][4];    //Guest key uid 저장용(0xff 또는 0x00은 삭제로 처리함)
u1EX u8 guest_ROOM_no[6];       //Room No (PC에서 전송 받는다)
u1EX u8 rf_card_class;
u1EX u8 rf_card_uid[4];
u1EX u8 rf_card_valid_date[4];
u1EX u8 rf_card_room_no[6];

u1EX u8 tnt_page_mode[5];
u1EX u8 service_send_count;
u1EX u8 tnt_toggle_bit[10][5];
u1EX u8 language_code;
u1EX u8 dimmer_apply_bit;
u1EX u8 scene_code;
u1EX u8 tnt_toggle_power_on[10];
u1EX u8 dim_apply_count;
u1EX u8 scene_mode;
u1EX u8 tnt_chime_send_buf;
u1EX u8 dimmer_level[16];
u1EX u8 dimmer_step_level[16];
u1EX u8 audio_volume[4];

u1EX u8 tnt_heartbeat;
u1EX u8 tnt_page_mode_buf;
u1EX u8 f_chime_send[5];
//=======================================================================
u1EX __dwbits uart1;
#define Wdtdis_f			uart1.s.b4


u1EX __dwbits flag1;
#define f_uart1_data_send			flag1.s.b0
#define f_uart1_com_start			flag1.s.b1
#define f_uart1_frame_rx_ok		flag1.s.b2
#define f_uart1_dir_start			flag1.s.b3

u1EX __dwbits flag2;
#define main_comm_rx_ok 			flag2.s.b0
#define f_air_set_temp_kind		flag2.s.b1  //1=최대, 최소 온도 적용
#define f_language_rx                           uart2_flag2.s.b3
#define f_scene_lamp_flag    	                uart2_flag3.s.b4


//=======================================================================

//=======================================================================

