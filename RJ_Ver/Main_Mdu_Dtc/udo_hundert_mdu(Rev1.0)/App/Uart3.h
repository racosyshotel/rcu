/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/

#ifndef __UART3_H__
#define __UART3_H__

//=======================================================================

void uart3_frame_proc(void);
void UART3_initial(void);
void UART3_tx_check(void);
void UART3_rxd_check(void);
void basic_data_execution(unsigned char index);
unsigned int cb_first_send_check(void);
void cb_suncha_send(void);
void cb_command_send(unsigned int index, unsigned char send_cmd);
void priority_pc_send(unsigned int cb_id);
void cb_send_crc_cal(void);
void cb_power_on_send(void);

//=======================================================================

#define debug_sp1

//=======================================================================
//=======================================================================
//=======================================================================

#ifdef __UART3_C__
#define u3EX 

const u8 b_decimal_to_hex_table_3[] = {
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

const u8 b_max_cb_count[]={   
//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
    28, 20,
};

const u16 dtc_rcu_count[32][128] = {              
  {0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a,
   0x000b, 0x000c, 0x000d, 0x000e, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014,      //DTC 1번 28
   0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c }, 
   
  {0x001d, 0x001e, 0x001f, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026,
    0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030},    //DTC 2번 20
};

#else

#define u3EX extern

#endif
u3EX u8 cb_rx_room_data_buf[800][70];
//u3EX u8 cb_tx_room_data_buf[64][100];
//[0] = 전송 Flag(명령어의 종류)
//[1] = 
u3EX u8 uart3_tx_data_buf[100];
u3EX u8 uart3_tx_length;
u3EX u8 uart3_tx_backup;
u3EX u8 sub_call_id;
u3EX u8 uart3_rxding_point;
u3EX u8 uart3_rxding_buf[100];
u3EX u8 uart3_rxd_buf[100];
u3EX u8 uart3_rx_length;
u3EX u16 cb_first_check_point;
u3EX u16 cb_first_save_point;
u3EX u16 cb_first_save_buffer[800][2];
u3EX u16 priority_pc_send_save_point;
u3EX u16 priority_pc_send_check_point;
u3EX u16 priority_pc_send_buffer[800];
u3EX u8 device_count;
u3EX u8 send_point_buf;
u3EX u8 device_comm_status[32];

u3EX __wbits uart3_flag;
#define f_uart3_send_time		uart3_flag.s.b0
#define f_uart3_data_send		uart3_flag.s.b1
#define f_uart3_com_start		uart3_flag.s.b2
#define f_uart3_frame_rx_ok uart3_flag.s.b3
#define f_uart3_common_data uart3_flag.s.b4

#define b_device_count  2       // DTC 갯수 추가

#endif
//=======================================================================
//=======================================================================

