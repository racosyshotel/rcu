/*=====================(C) COPYRIGHT 2014 PLUS-H.=========================
program		    : CONTROL BOX(RCS-300-RCU)
processor 	  : STM32F103xx
compiler		  : IAR 6.2A Compiler 
program BY	  : H.H.Hwang
date			    : 2014.	 .
copy right	  : P L U S  -  H. 
============================================================================*/

typedef u16  width;
/*  The CRC16 parameters.                        */
#define POLYNOMIAL          0x8005
#define INITIAL_REMAINDER   0x0000
#define FINAL_XOR_VALUE     0x0000
 
/* The width of the CRC calculation and result */
#define WIDTH   (8 * sizeof(width))
#define TOPBIT  (1 << (WIDTH - 1))

void UART3_control_proc(void);
void UART3_initial(void);
void UART3_frame_proc(void);
void UART3_tx_check(void);

void UART3_rx_check(void);

void lg_aircon_monitor_send(unsigned char index,unsigned char data_code0,unsigned char data_code1,unsigned char data_code2,unsigned char data_code3,unsigned char data_code4);
void lg_aircon_control_send(unsigned char index,unsigned char data_code0,unsigned char data_code1,unsigned char data_code2,unsigned char data_code3,unsigned char data_code4);

width CrcCompute(unsigned char *message, unsigned int nBytes);
#define debug_mode_u3


//=======================================================================

#ifdef __UART3_H__
#define u3EX

const u16 crcTable[256] = 
{
       0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
       0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
       0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
       0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
       0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
       0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
       0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
       0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
       0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
       0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
       0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
       0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
       0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
       0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
       0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
       0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
       0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
       0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
       0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
       0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
       0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
       0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
       0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
       0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
       0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
       0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
       0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
       0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
       0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
       0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
       0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
       0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

const u8 b_lg_air_temp_table[] = {
  40,40,      
  39,39,
  38,38,38,
  37,37,
  36,36,
  35,35,35,
  34,34,34,
  33,33,
  32,32,32,
  31,31,
  30,30,30,
  29,29,29,
  28,28,28,
  27,27,27,
  26,26,
  25,25,25,
  24,24,24,
  23,23,23,
  22,22,22,
  21,21,21,
  20,20,20,
  19,19,19,
  18,18,18,
  17,17,17,17,
  16,16,16,
  15,15,15,
  14,14,14,
  13,13,13,
  12,12,12,
  11,11,11,
  10,10,10,
};


#else
#define u3EX extern 




#endif

u3EX u8 uart3_tx_length;
u3EX u8 uart3_tx_backup;
u3EX u8 uart3_tx_data_buf[100];

u3EX u8 uart3_rxding_point;
u3EX u8 uart3_rxding_buf[100];
u3EX u8 uart3_rxd_buf[100];

u3EX u16 uart3_send_timer;
u3EX u16 uart3_send_time_2;
u3EX u16 uart3_send_time_3;
u3EX u8 u3_rxd_length;

u3EX u8 lg_air_error_code[2];
u3EX u8 f_uart3_com_start;
u3EX u8 aircon_m_flag;
u3EX u8 m_TX1;
u3EX u8 m_TX2;
u3EX u8 m_TX3;
u3EX u16 ondo_settemp_data;
u3EX u8  data_count_delay;
u3EX u8  ondo_set_temp_1;
u3EX u8  ondo_set_temp_2;
u3EX u8  f_monitor;
u3EX u8  aircon_no_respones[2];
u3EX u8  return_speed[2];
u3EX u8  m_data_counting;
u3EX u8  aircon_address;
u3EX u8  f_aircon_on[4];
u3EX u8  f_ondo_power_set[4];
u3EX u8  f_no_wind;
u3EX u8  f_off_data;
u3EX u8  aircon_count;

u3EX u8  reset_send_count;

u3EX u8  b_lg_on_off[4];
u3EX u8  f_lg_on_off[4];
u3EX u8  on_off_data[4];

u3EX u8  b_lg_mode[4];
u3EX u8  f_lg_mode[4];
u3EX u8  mode_data[4];

u3EX u8  b_lg_temp[4][4];
u3EX u8  f_lg_temp[4];

u3EX u8  b_lg_wind[4];
u3EX u8  f_lg_wind[4];
u3EX u8  wind_data[4];
u3EX u8  f_lg_add_toggle[4];
u3EX u8  lg_temp_check;
u3EX u8  lg_temp_count;
u3EX u8  lg_monitoring_ct;
u3EX u8  f_rx_check;
u3EX u8  aircon_power_counter[4];
u3EX u8  no_send_wind;
u3EX u8  no_send_power;
//=======================================================================
u3EX __bits uart3;
#define f_uart3_send_time   uart3.s.b1
#define f_uart3_data_send   uart3.s.b2
#define f_uart3_frame_rx_ok uart3.s.b4
#define f_uart3_data_start  uart3.s.b5
#define data_flag           uart3.s.b6
#define f_uart3_data_send_2 uart3.s.b7
#define f_uart3_data_send_3 uart3.s.b0

u3EX __bits ss_flag;
#define f_traking_ok          ss_flag.s.b0
#define f_ondo_on             ss_flag.s.b1
#define f_ondo_con_ok         ss_flag.s.b2
#define f_ondo_re_on          ss_flag.s.b3
#define f_first_time          ss_flag.s.b4
#define f_ondo_re_on_2        ss_flag.s.b5
#define f_data_send_ok        ss_flag.s.b6
//=======================================================================


