/*========================(C) COPYRIGHT 2008 Insem Inc.=========================
program			: BROAD-CASTING CONTROLLER SYSTEM
processor 		: ARM966E-S	
compiler			: EWARM Compiler	
program BY		: YN.Kim
date				: 2008.    .
copy right		: Insem Inc.
===========================================================================*/
#define __SPI2_H__

#include "stm32f10x_lib.h"
#include "main.h"

//==========================================================================
u8 __SPI_FLASH_WaitForWriteEnd(void) {
u8 FLASH_Status =0;

	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();

	/* Send "Read Status Register" instruction */
	SPI_FLASH_SendByte(RDSR);

	/* Loop as long as the memory is busy with a write cycle */
	FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
	if((FLASH_Status & WIP_Flag) == SET) FLASH_Status =0;				 /* Write in progress */
	else FLASH_Status =1;			

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	return FLASH_Status;
}

//==========================================================================
u8 stest, spirxbuf[256], spitxbuf[256];
u32 spiaddr;
u16 sercnt, sokcnt;
u8 sfsel;

u8 sfcctrl=7;
void sf_control_proc(void) {

	 switch(sfcctrl) {
		case 0:
			memset(spitxbuf, stest, sizeof(spitxbuf));
			SPI_FLASH_PageWrite(spitxbuf, spiaddr, sizeof(spitxbuf)); 
//			SPI_FLASH_BufferWrite(spirxbuf, spiaddr, sizeof(spirxbuf)); 
			sfcctrl++;
			break;					

		case 1:
			if(__SPI_FLASH_WaitForWriteEnd()) {			// flash write time wait checking ???
				memset(spirxbuf, 0, sizeof(spirxbuf));			
//				SPI_FLASH_BufferRead(spirxbuf, spiaddr, sizeof(spirxbuf));
				SPI_FLASH_BufferRead(spirxbuf, spiaddr, sizeof(spirxbuf));

				for(w16=0; w16<sizeof(spirxbuf); w16++) {
					if(spirxbuf[w16] !=stest) { sercnt++;
						break;
					}
				}
				if(w16 ==sizeof(spirxbuf)) sokcnt++;
				
				if(zin.msel &0x80 &&!Extm.s.m25wc) { 
					Extm.s.m25wc =50;
					sprintf(xdebug, "[ERR]%d  [OK]%d [Addr]0x%04x [W]%02x [R]%02x \r", sercnt, sokcnt, spiaddr, stest, spirxbuf[0]); mm;
				}

				stest++;
				spiaddr +=0x0100;
				if(spiaddr >=m25p_fullsize) { spiaddr =0;
					__lineclr(50);

					if(++sfsel >=4) sfsel =0;
					if(sfsel ==0) sfcctrl =9;
					else if(sfsel ==1) sfcctrl =9;
					else if(sfsel ==2) sfcctrl =13;
					else {
						sfcctrl--;
					 		if(zin.msel &0x80)  __xcprintf("No Erase \r\n");
					}
				}
				else {
					sfcctrl--;
				}
			}
			break;

		case 2:
			if(__SPI_FLASH_WaitForWriteEnd()) sfcctrl--;
			break;
	 
// powerup start
      case 7:
			Timer_setup(5, 100);
			sfcctrl++;
		case 8:
			if(tm5fg) {
					if(zin.msel &0x80)  __xcprintf("SPI Initial !!!\r\n");
   	   	SPI_FLASH_Init(); 				 //use spi1 for adpcm 
		 		spiaddr =0;
		 		sfcctrl =15;
			}
			break;

		case 15:
			break;
			
			w32 =SPI_FLASH_ReadID();	
			sprintf(xdebug, "Read ID : 0x%lx\r\n", w32); mm;
			sfcctrl =9;
			break;

		case 9:			
			sprintf(xdebug, "%d Sector Erase\r", spiaddr>>16); mm;
			SPI_FLASH_SectorErase(spiaddr);
			sfcctrl++;
			break;
			
		case 10:
			if(__SPI_FLASH_WaitForWriteEnd()) {  
				spiaddr +=m25p_sectorsize;
				if((spiaddr>>16) >=(m25p_fullsize/m25p_sectorsize)) {
					spiaddr =0;
					sfcctrl =0;
					__linefeed();
				}
				else sfcctrl--;
			}
         break;

		case 11:			
			sprintf(xdebug, "%d Sub Sector Erase\r", spiaddr>>12); mm;
			SPI_FLASH_SubSectorErase(spiaddr);
			sfcctrl++;
			break;
			
		case 12:
			if(__SPI_FLASH_WaitForWriteEnd()) {  
				spiaddr +=m25p_Subsectorsize;
				if((spiaddr>>12) >=(m25p_fullsize/m25p_Subsectorsize)) {
					spiaddr =0;
					sfcctrl =0;
					__linefeed();
				}
				else sfcctrl--;
			}
         break;

		case 13: 		
			sprintf(xdebug, "Bulk Erase\r"); mm;
			SPI_FLASH_BulkErase();
			sfcctrl++;
			break;
			
		case 14:
			if(__SPI_FLASH_WaitForWriteEnd()) {  
				spiaddr =0;
				sfcctrl =0;
				__linefeed();
			}
			break;

      default: 	  sfcctrl =0;	      break;
	}
	 watchdog.s.b15 =1;					 // watchdog setup
}

//========================(C) COPYRIGHT 2008 Insem Inc.=========================

