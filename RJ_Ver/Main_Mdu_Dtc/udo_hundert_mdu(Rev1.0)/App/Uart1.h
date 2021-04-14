/*******************************(C) COPYRIGHT 2013 PLUS - H.****************************************/
/* processor 	    : CORETEX-M3(STM32F207)         		    				                                 */
/* compiler       : EWARM Compiler								                                                 */
/* program by	    : H. H. Hwang									                                                   */
/* History:											                                                                   */
/* 11/10/2013     : Version 1.0									                                                   */
/* copy right	    : PLUS - H.									                                                     */
/****************************************************************************************************/
#ifndef __UART1_H__
#define __UART1_H__

//=======================================================================

void UART1_control_proc(void);
void UART1_initial(void);

u8 u1_byteload(void);
void __lineclr(u8 len);
void screen_clear(void);
void line_clr(u8 col);
void __printf(char *str);
void __xcprintf(const char *str);
void _xdprintf(char *str);
void __xdprintf(char *str);
void factory_initial(void);
void hyper_clear(void) ;
void __linefeed(void);
//=======================================================================



#define debug_sp1



//=======================================================================

#ifdef __UART1_C__
#define u1EX 
extern u16 wdtcnt;

#else
#define u1EX extern

#endif

#endif

//=======================================================================
//=======================================================================

