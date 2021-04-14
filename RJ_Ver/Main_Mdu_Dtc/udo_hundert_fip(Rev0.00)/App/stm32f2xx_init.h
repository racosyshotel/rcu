/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2XX_INIT_H_
#define __STM32F2XX_INIT_H_


/******************************** Function ************************************************/
void Internal_timer_Proc(void);
void TimerSetup(uchar tmno, ushort timer); 
void Gp_initial(void);
void GPIO_port_initial(void);
void TIME2_initial(void);

void Tgbit(u32 *reg, u8 Bitno);
void Setbit(u32 *reg, u8 Bitno);
void Clrbit(u32 *reg, u8 Bitno); 
u8 Chkbit(u32 *reg, u8 Bitno);
void Iwdog_initial(u16 wtime);
void fip_led_drive(void);
void fip_led_move(void);
void dip_switch_read(void);


/******************************** Define **************************************************/
#define CLK_72MHZ         //2.4V    :240mA , 70mA           
//#define CLK_32MHZ           //2.4V     :220mA, 53mA         
//#define CLK_24MHZ         //2.4V    :220mA, 46mA  (LCD 속도 느려짐)     
//#define CLK_16MHZ         //2.4V   :220mA, 40mA  (LCD 속도 느려짐)         

//#define BK_CON
#define NORMAL
//#define STANDBY
//#define STOP_MODE


//----- PORT A ------
#define P_u2dir   GpioA->Bit.b1
#define P_u2tx    GpioA->Bit.b2
#define P_u2rx    GpioA->Bit.b3
#define P_u1dir   GpioA->Bit.b8
#define P_u1tx    GpioA->Bit.b9
#define P_u1rx    GpioA->Bit.b10

//----- PORT B ------
#define P_clock_rst    GpioB->Bit.b5
#define P_clock_clk    GpioB->Bit.b6
#define P_clock_data   GpioB->Bit.b7
#define P_u3tx    GpioB->Bit.b10
#define P_u3rx    GpioB->Bit.b11
#define P_u3dir   GpioB->Bit.b12
#define P_u4dir   GpioB->Bit.b13
#define P_u5dir   GpioB->Bit.b14

//----- PORT C ------
#define P_dip_0   GpioC->Bit.b0
#define P_dip_1   GpioC->Bit.b1
#define P_dip_2   GpioC->Bit.b2
#define P_dip_3   GpioC->Bit.b3
#define P_dip_4   GpioC->Bit.b4
#define P_dip_5   GpioC->Bit.b5
#define P_dip_6   GpioC->Bit.b6
#define P_dip_7   GpioC->Bit.b7
#define P_u4tx    GpioC->Bit.b10
#define P_u4rx    GpioC->Bit.b11
#define P_u5tx    GpioC->Bit.b12
#define P_tc_rst  GpioC->Bit.b13

//----- PORT D ------
#define P_u5rx    GpioD->Bit.b2
#define P_exsw_1  GpioD->Bit.b8
#define P_exsw_2  GpioD->Bit.b9
#define P_exsw_3  GpioD->Bit.b10
#define P_exsw_4  GpioD->Bit.b11
#define P_exsw_5  GpioD->Bit.b12
#define P_exsw_6  GpioD->Bit.b13
#define P_exsw_7  GpioD->Bit.b14
#define P_exsw_8  GpioD->Bit.b15

//----- PORT E ------
#define P_led_y0  GpioE->Bit.b0
#define P_led_y1  GpioE->Bit.b1
#define P_led_y2  GpioE->Bit.b2
#define P_led_y3  GpioE->Bit.b3
#define P_led_y4  GpioE->Bit.b4
#define P_led_y5  GpioE->Bit.b5
#define P_led_y6  GpioE->Bit.b6
#define P_led_y7  GpioE->Bit.b7
#define P_led_y8  GpioE->Bit.b8
#define P_led_y9  GpioE->Bit.b9
#define P_led_y10 GpioE->Bit.b10
#define P_led_y11 GpioE->Bit.b11
#define P_led_y12 GpioE->Bit.b12
#define P_led_y13 GpioE->Bit.b13
#define P_led_y14 GpioE->Bit.b14
#define P_led_y15 GpioE->Bit.b15

//----- PORT F ------
#define P_dip_10  GpioF->Bit.b0
#define P_dip_11  GpioF->Bit.b1
#define P_dip_12  GpioF->Bit.b2
#define P_dip_13  GpioF->Bit.b3
#define P_dip_14  GpioF->Bit.b4
#define P_dip_15  GpioF->Bit.b5
#define P_dip_16  GpioF->Bit.b6
#define P_dip_17  GpioF->Bit.b7
#define P_led1    GpioF->Bit.b8
#define P_led2    GpioF->Bit.b9
#define P_led3    GpioF->Bit.b10
#define P_led4    GpioF->Bit.b11
#define P_led5    GpioF->Bit.b12
#define P_led6    GpioF->Bit.b13
#define P_led7    GpioF->Bit.b14
#define P_led8    GpioF->Bit.b15

//----- PORT G ------
#define P_led_x0  GpioG->Bit.b0
#define P_led_x1  GpioG->Bit.b1
#define P_led_x2  GpioG->Bit.b2
#define P_led_x3  GpioG->Bit.b3
#define P_led_x4  GpioG->Bit.b4
#define P_led_x5  GpioG->Bit.b5
#define P_led_x6  GpioG->Bit.b6
#define P_led_x7  GpioG->Bit.b7
#define P_led_x8  GpioG->Bit.b8
#define P_led_x9  GpioG->Bit.b9
#define P_led_x10 GpioG->Bit.b10
#define P_led_x11 GpioG->Bit.b11
#define P_led_x12 GpioG->Bit.b12
#define P_led_x13 GpioG->Bit.b13
#define P_led_x14 GpioG->Bit.b14
#define P_led_x15 GpioG->Bit.b15


#define b_sram_check_io_base    0x0100
#define b_sram_clean_base       0x0200
#define b_sram_global_data_base 0x0300
#define b_sram_room_base        0x0400

/******************************** structure ***********************************************/

typedef struct {
	u8 u1c;
	u8 u2c;
	u8 u3c;
	u8 u4c;
	u8 u4ctmp;
	u8 u5c;
  	u8 m25wc;
	u8 pcu1c;
	u8 pc;
	u8 eeprom;
}_ltm;

typedef union {
	_ltm s;
	u8 buf[sizeof(_ltm)];
}__extm;


/******************************** global parameter ****************************************/
#ifdef  __stm32f2xx_init__
#define itEX

GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
RCC_ClocksTypeDef RCC_ClockFreq;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;
SPI_InitTypeDef SPI_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
I2C_InitTypeDef I2C_InitStructure; 

#else
#define itEX    extern
itEX GPIO_InitTypeDef GPIO_InitStructure;
itEX GPIO_InitTypeDef GPIO_InitStructure;
itEX RCC_ClocksTypeDef RCC_ClockFreq;
itEX NVIC_InitTypeDef NVIC_InitStructure;
itEX USART_InitTypeDef USART_InitStructure;
itEX SPI_InitTypeDef SPI_InitStructure;
itEX TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
itEX I2C_InitTypeDef I2C_InitStructure; 

#endif

itEX u16 timer_buf[16];
itEX __tm tm;
itEX __extm extm;
itEX u8 w8;
itEX u16 w16, Wdtcnt;
itEX u32 w32;
itEX u16 uart3_call_timer;
itEX u8 led_scan_count;
itEX u8 led_scan_time;
itEX u16 led_y_buf[16];
itEX u8 led_shift_buf;
itEX u8 led_kind_buf[16][16];
itEX u8 led_drive_time;
itEX u8 dip_switch_buf_2;
itEX u8 dip_switch_buf_2_back;

itEX u8 dip_function;   //Dip switch 2번
itEX u8 dip_function_2; //Dip switch 1번 
itEX u8 dong_code;

itEX __wbits watchdog;
/******************************** Bit Define **********************************************/
itEX __bits stm32_init;
#define tm1ms_f         stm32_init.bit.b0
#define Wdtdis_f        stm32_init.bit.b1






#endif


/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/

