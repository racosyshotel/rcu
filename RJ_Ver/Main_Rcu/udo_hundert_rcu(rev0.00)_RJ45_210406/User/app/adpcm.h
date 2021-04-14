//==================(C) COPYRIGHT 2002 INSEM Inc.========================
// processor 	  : CORETEX-M4(STM32F4xx)         	                     
// compiler      : EWARM 6.40 Compiler							    				
// program by	  : Yn.Kim									    						
// History:											    									
// 02/20/2013    : Version 1.0									   				
// copy right	  : Insem Inc.									    					
//=======================================================================
#ifndef __ADPCM_H__
#define __ADPCM_H__

void SetAdpcmIndexLoad(u16 index);
void Adpcm_Task(void);
void Timer3_Initial(void);
void AdpcmStop(void);
void DMA_Initial(void);
void Adpcm_LowLevel_DMAConfig(u32 pBuffer, u32 BufferSize, u32 Direction);
s8 AdpcmPlayCheck(s8 index);
u8 AdpcmDecoderX(void);

//=======================================================================
#define AdpcmBufLen			250					// must be defined even
#define AdpcmIndexMax		20
#define OnePlayListMax		32
#define AdpcmDirectionTX  	0
#define AdpcmDirectionRX 	1 
#define AdpcmVolStep 		64
#define VoiceBaseAddress	0x0080000
#define ProgramBaseAddress	0x0000000

#define SampleFrequncy		32000					// KHz

//=======================================================================
#pragma pack(1)
//--------------------------------------
typedef struct {
	vs16 DMAbuf[AdpcmBufLen]; 			// fix(do not move)
	vs16 LoadBuf[AdpcmBufLen]; 					 
	
	u64 StartAddress;  
	u64 TotalBytes;
	u8 vFilename[16];
	u16 Index[OnePlayListMax+1];
	u8 PlayMaxC;
	u8 WrCnt;
	u8 RdCnt;
	u8 LoadCnt;
	
	struct {
		unsigned PlayStop 	:1;
		unsigned Busy		:1;
		unsigned Play		:1;
		unsigned Stop		:1;
		unsigned apLoad	:1;		
		unsigned dmaend	:1;
	}s;
	u8 PlayNo;
}__adpcm;

//--------------------------------------
#pragma pack()
//=======================================================================
//=======================================================================
//#include "voiceindex.h"

#ifdef __ADPCM_C__
#define apEX 

const s16 playlist[OnePlayListMax][2] ={
	{ 0, -1 },
	{ 1, -1 },
	{ 2, -1 },
	{ 3, -1 },
	{ 4, -1 },
	{ 5, -1 },
	{ 6, -1 },
	{ 7, -1 },
	{ 8, -1 },
	{ 9, -1 },
	{ 10, -1 },
	{ 11, -1 },
	{ 12, -1 },
	{ 13, -1 },
	{ 14, -1 },
	{ 15, -1 },
	{ 16, -1 },
	{ 17, -1 },
	{ 18, -1 },
	{ 19, -1 },
	{ 20, -1 },
	{ 21, -1 },
	{ 22 -1 },
	{ 23, -1 },
	{ 24, -1 },
	{ 25, -1 },
	{ 26, -1 },
	{ 27, -1 },
	{ 28, -1 },
	{ 29, -1 },
	{ 30, -1 },
	{ 31, -1 },
};

#else
#define apEX extern 
apEX const s16 playlist[OnePlayListMax][2];

#endif

apEX __adpcm adpcm;
apEX float VolRate;
apEX s16 LastDac;

//=======================================================================
apEX __bits xadpcm;
#define adpcminit_f xadpcm.s.b0
#define Voiceok_f xadpcm.s.b1
#define voiceLoad_f xadpcm.s.b2



//=======================================================================

#endif

