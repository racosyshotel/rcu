/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : EWARM Compiler								    */
/* program by	  : JK.Won									    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : Insem Inc.									    */
/****************************************************************************************************/
#ifndef __ADC_H__
#define __ADC_H__

void adc_control_proc(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((u32)0x4001244C)
#define SORT_MAXLEN 3

#define level_step 10
#define dlvl (4096/level_step)

/* Private define ------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef __ADC_C__
#define adEX
const u16 level_fix[] = {
	0 * dlvl,
	1 * dlvl,
	2 * dlvl,
	3 * dlvl,
	4 * dlvl,
	5 * dlvl,
	6 * dlvl,
	7 * dlvl,
	8 * dlvl,
	9 * dlvl,
	10 * dlvl,
	11 * dlvl,
	12 * dlvl,
	13 * dlvl,
	14 * dlvl,
	15 * dlvl,
	16 * dlvl,
	17 * dlvl,
	18 * dlvl,
};

#else
#define adEX extern 

#endif



adEX volatile unsigned short ADC_ConvertedValue;
adEX unsigned short sort_ad_value;

adEX unsigned short ad_hex_value;
adEX unsigned long  ad_hex_hap;
adEX unsigned short ad_hex_result;
adEX unsigned char  ad_hex_count;


adEX __bits adc;
#define channel_f						adc.bit.b0


/************************************** COPYRIGHT 2007 Insem Inc.*************************************/
#endif

