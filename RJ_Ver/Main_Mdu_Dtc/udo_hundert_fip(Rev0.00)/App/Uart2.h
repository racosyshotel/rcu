/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/
#ifndef __UART2_H__
#define __UART2_H__

//=======================================================================

void UART2_initial(void);
void UART2_tx_check(void);
void UART2_rxd_check(void);
void command_check(unsigned char cb_id);
void cb_first_send_save(unsigned char index, unsigned char save_cmd);
void common_command_check(unsigned int);
void clean_order_time_check(void);
unsigned char temp_dec_conv(unsigned char cb_index, unsigned char index);
unsigned char temp_05_check(unsigned char cb_index, unsigned char index);
unsigned char priority_pc_send_check(void);
void uart2_frame_proc(void);

//=======================================================================



#define debug_sp1

//=======================================================================
//              =======================================================================

//=======================================================================

#ifdef __UART2_C__
#define u2EX    
  
const u8 b_max_cb_count[]={   
//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
   28, 20
};

const u16 b_cb_send_id_table[32][128] = {                                   
  {0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 
   0x000b, 0x000c, 0x000d, 0x000e, 0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014,      //DTC 1�� ����(28EA)
   0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c,  }, 
   
  {0x001d, 0x001e, 0x001f, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 
   0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030,      //DTC 2�� �ܵ�(20EA) 
  },                                                
   
 /* {
    0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f, 0x0020, 
    0x0021, 0x0022, 
  },                                                            //DTC 2��    12ea
  
  {
    0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c,
    0x002d, 0x002e, 0x002f, 
  },                                                            //DTC 3��    13ea
    
  {
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 
    0x003a, 0x003b, 0x003c,       
  },                                                            //DTC 4��     13ea
  
  {      
    0x003d, 0x003e, 0x003f, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046,
    0x0047, 0x0048, 0x0049,           
  },                                                            //DTC 5��     13ea
  
  {
    0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053,
    0x0054, 0x0055, 0x0056, 
  },                                                            //DTC 6��     13ea                                                                            
  
  {
    0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f, 0x0060,
    0x0061, 0x0062, 0x0063,
  },                                                            //DTC 7��     13ea
  
  {
    0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 
    0x006e, 0x006f, 0x0070, 
  },                                                            //DTC 8��     13ea
  
  {
    0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a,
    0x007b, 0x007c, 0x007d,      
  },                                                            //DTC 9��     13ea
  
  {
    0x007e, 0x007f, 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008a, 
  },                                                            //DTC 10��    13ea
  
  {
    0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 0x0090, 0x0091, 0x0092, 0x0093, 0x0094,   
    0x0095, 0x0096, 0x0097,   
  },                                                            //DTC 11��    13ea  
  
  {
    0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, 0x00a0, 0x00a1, 
    0x00a2, 0x00a3, 0x00a4, 
  },                                                            //DTC 12��    13ea
  
  {
    0x00a5, 0x00a6, 0x00a7, 0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae,
    0x00af, 0x00b0, 0x00b1,  
  },                                                            //DTC 13��    13ea
  
  {
    0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, 0x00b8, 0x00b9, 0x00ba, 0x00bb,
    0x00bc, 0x00bd, 0x00be,      
  },                                                            //DTC 14��    13EA
  
  {
    0x00bf, 0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, 0x00c8,     
    0x00c9, 0x00ca, 0x00cb,         
  },                                                           //DTC 15��    13EA
  
  {
    0x00cc, 0x00cd, 0x00ce, 0x00cf, 0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 
    0x00d6, 0x00d7, 0x00d8,  
  },                                                           //DTC 16��    13EA
  */
  /*
    0x00d9, 0x00da, 0x00db, 0x00dc,
    0x00dd, 0x00de, 0x00df, 0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4,    
    0x00e5, 0x00e6, 0x00e7, 0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee,
    0x00ef, 0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,    
    0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff, 0x0100, 0x0101,
    0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107, 0x0108, 0x0109, 0x010a,      
    0x010b, 0x010c, 0x010d, 0x010e, 0x010f, 0x0110, 0x0111, 0x0112, 0x0113, 0x0114,
    0x0115, 0x0116, 0x0117, 0x0118, 0x0119, 0x011a, 0x011b, 0x011c, 0x011d,    
    0x011e, 0x011f, 0x0120, 0x0121, 0x0122, 0x0123, 0x0124, 0x0125, 0x0126, 0x0127,
    0x0128, 0x0129, 0x012a, 0x012b, 0x012c, 0x012d, 0x012e, 0x012f, 0x0130,       
  */
};

const u8 b_hex_to_decimal_table[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
  0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31,
  0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x60, 0x61, 0x62, 0x63,
  0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
  0x96, 0x97, 0x98, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99 
};

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

const u8 my_device_id_check[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
  0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31, 
};

const u16 hextbl1[] =    {0, 1,     2,     3,     4,     5,      6,      7,      8,      9,     0x0f,  0x0f,  0x0f,  0x0f,  0x0f,  0x0f};
const u16 hextbl10[] =   {0, 0x0a,  0x14,  0x1e,  0x28,  0x32,   0x3c,   0x46,   0x50,   0x5a,  0xf0,  0xf0,  0xf0,  0xf0,  0xf0,  0xf0};
const u16 hextbl100[] =  {0, 0x64,  0xc8,  0x12c, 0x190, 0x1f4,  0x258,  0x2bc,  0x320,  0x384, 0xf00, 0xf00, 0xf00, 0xf00, 0xf00, 0xf00};
const u16 hextbl1000[] = {0, 0x3e8, 0x7d0, 0xbb8, 0xfa0, 0x1388, 0x1770, 0x1b58, 0x1f40, 0x2328,0xf000,0xf000,0xf000,0xf000,0xf000,0xf000};


#else

#define u2EX extern

#endif

u2EX u8 uart2_tx_data_buf[100];
u2EX u8 uart2_tx_length;
u2EX u8 uart2_tx_backup;
u2EX u8 uart2_rxding_point;
u2EX u8 uart2_rxding_buf[100];
u2EX u8 uart2_rxd_buf[100];
u2EX u8 cb_send_id;

u2EX u8 cb_send_data_buf[64][50];
u2EX u8 cb_first_save_point;
u2EX u8 cb_first_save_buffer[100][2];

u2EX u8 common_data_buf[50];
u2EX u8 my_device_no;
u2EX u8 group_data_buf[100][5];

u2EX u8 bojung_flag;
u2EX u16 cb_max_count[20];

u2EX __wbits uart2_flag;
#define f_uart2_send_time		    uart2_flag.s.b0
#define f_uart2_data_send		    uart2_flag.s.b1
#define f_uart2_com_start		    uart2_flag.s.b2
#define f_uart2_frame_rx_ok     uart2_flag.s.b3
#define f_illumination_ok       uart2_flag.s.b4
#define f_main_send_condition   uart2_flag.s.b5


#endif

//=======================================================================
//=======================================================================
