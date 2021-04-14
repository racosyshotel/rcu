/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : EWARM Compiler								    */
/* program by	  : JK.Won									    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : Insem Inc.									    */
/****************************************************************************************************/
#define __ADC_C__

#include "stm32f10x_lib.h"
#include "main.h"

ushort Sort_buf[SORT_MAXLEN];
/******************************************************************************************************/
/* Sort_listing	:									              */												
/******************************************************************************************************/
void Sort_listing(ushort sortdata) {
uchar Count,Index;

	Count =0;
	do	{
		if(Sort_buf[Count] <sortdata) {
			for( Index =0; Count < SORT_MAXLEN-1; Count++, Index++) 
				Sort_buf[SORT_MAXLEN-1-Index] =Sort_buf[SORT_MAXLEN-2-Index];
			Sort_buf[SORT_MAXLEN-1-Index] =sortdata;
		}
	}
	while (++Count <SORT_MAXLEN);
}
/******************************************************************************************************/
/* ADC_initial :		channel 14 (PC.04)									*/ 											
/******************************************************************************************************/
void ADC_initial(void){
ADC_InitTypeDef ADC_InitStructure;
//DMA_InitTypeDef DMA_InitStructure;

	/* Enable ADC1 and GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure PA.0 (ADC Channel0) as analog input -------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel0 configuration */ 
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

#ifdef ADC_DMA  
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
#endif

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
}


/******************************************************************************************************/
/* adc_control_proc	:									      */												
/******************************************************************************************************/
uchar adctrl =7;
void adc_control_proc(void){
static u8 adcnt;

    switch(adctrl){
      case 0:
			memset(Sort_buf, 0x00, sizeof(Sort_buf));					// sort buffer all clear
			adcnt=0;
			adctrl++;    
			break;
    
      case 1:
			/* Start ADC1 Software Conversion */ 
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			adctrl++;
			break;
      
      case 2:

			if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)){    				/* adc complete flag */
				ADC_ClearFlag(ADC1,ADC_FLAG_EOC);

				Sort_listing(ADC_GetConversionValue(ADC1));   

				if(++adcnt >=SORT_MAXLEN) {									// sorting complete
//					sort_data =Sort_buf[SORT_MAXLEN/2] ;
					adctrl =0;
          ad_hex_value = Sort_buf[SORT_MAXLEN/2];
          ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
          if(++ad_hex_count >= 200){
            ad_hex_hap += ad_hex_value;
            ad_hex_result = ad_hex_hap / 200;
            ad_hex_hap = 0;
            ad_hex_count = 0;
            
            if(!main_comm_rx_ok) //메인통신 연결이 안되었나?
            {
              //room_data_buf[79] = (ad_hex_result/100); //volume data    
            }            
          }
          else{
            ad_hex_hap += ad_hex_value;
          }
    
          
				}  
				else adctrl--;
			}
      	break;
   
	// powerup start
      case 7:
			ADC_initial();

			/* Enable ADC1 reset calibaration register */	
			ADC_ResetCalibration(ADC1);
			/* Check the end of ADC1 reset calibration register */
			while(ADC_GetResetCalibrationStatus(ADC1));			
			
			/* Start ADC1 calibaration */
			ADC_StartCalibration(ADC1);
			/* Check the end of ADC1 calibration */
			while(ADC_GetCalibrationStatus(ADC1)); 
			

      default:		adctrl=0;		break;
    }
}
 
/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/
