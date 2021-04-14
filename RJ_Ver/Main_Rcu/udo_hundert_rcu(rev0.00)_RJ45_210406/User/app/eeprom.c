/*=====================(C) COPYRIGHT 2014 PLUS-H.=========================
program		    : CONTROL BOX(RCS-300-RCU)
processor 	  : STM32F103xx
compiler		  : IAR 6.2A Compiler 
program BY	  : H.H.Hwang
date			    : 2014.	 .
copy right	  : P L U S  -  H. 
============================================================================*/
#define __EEPROM_H__

#include "stm32f10x_lib.h"
#include "main.h"

//========================================================================
//========================================================================
void eeprom_control_proc(void) {
u16 ix, sum;
static u16 eepwr_size;

	switch(eepctrl) {
		case 0:
			if(eepwrite_f &&tm6fg) eepwrite_f =0, eepctrl++;			// eeprom write ???
			else if(eepread_f) eepctrl =7;									// eeprom reading ???	

			break;
			
//========================================================================
//========================================================================
// eeprom write1
		case 1:
			if(zin.msel &0x80) __xcprintf("data>>Eeprom write start !!!\r\n");
			
			memcpy(eepmem.buf, mem.buf, sizeof(_mem));
			eepmem.s.id_code =0x1401;
			for(ix=sum=0; ix<sizeof(_mem)-2; ix++) sum +=eepmem.buf[ix];
			eepmem.s.checksum =sum;
			
			eep_address =0;
			rw_address =eep_address *sizeof(_mem);
			eepwr_size =sizeof(_mem);
			rw_index =0;
			Extm.s.eeprom =10;
			eepctrl++;
			break;
			
		case 2:
			if(Extm.s.eeprom ==0) { Extm.s.eeprom =10;
				if((eepwr_size-rw_index) >eeprom_pagelength) {
					i2c_pagewrite (Slave_Address, rw_address+rw_index, eepmem.buf+rw_index, eeprom_pagelength);
					rw_index +=eeprom_pagelength;
				}
				else {
					if(rw_index !=eepwr_size) {
						i2c_pagewrite(Slave_Address, rw_address+rw_index, eepmem.buf+rw_index, eepwr_size-rw_index);
					}
					rw_index =0;
					eepctrl++;
				}
			}
			break;	

// eeprom write2
		case 3:
			if(Extm.s.eeprom ==0) { Extm.s.eeprom =10;
				if((eepwr_size-rw_index) >eeprom_pagelength) {
					i2c_pagewrite (Slave_Address, rw_address+Half_Address+rw_index, eepmem.buf+rw_index, eeprom_pagelength);
					rw_index +=eeprom_pagelength;
				}
				else {
					if(rw_index !=eepwr_size) {
						i2c_pagewrite(Slave_Address, rw_address+Half_Address+rw_index, eepmem.buf+rw_index, eepwr_size-rw_index);
					}
					eepctrl++;
				}
			}
			break;

		case 4:
			if(Extm.s.eeprom ==0) {
				eepctrl =0;
				eepdata_display();
			}
			break;

//========================================================================
// eeprom read1
		case 7:
			eep_address =0;
			rw_address =eep_address *sizeof(_mem);
			eepwr_size =sizeof(_mem);
			rw_index =0;
   		Extm.s.eeprom =10;
			eepctrl++;
         break;

		case 8:
			if(Extm.s.eeprom) return;
			if((eepwr_size-rw_index) >eeprom_pagelength) {
				if(i2c_pageread(Slave_Address, rw_address+rw_index, eepmem.buf+rw_index, eeprom_pagelength)) {
					rw_index +=eeprom_pagelength;
					Extm.s.eeprom =3;
				}
			}
			else {
				if(rw_index !=eepwr_size) {
					if(i2c_pageread(Slave_Address, rw_address+rw_index, eepmem.buf+rw_index, eepwr_size-rw_index)) {
					}
					else return;
				}
				Extm.s.eeprom =2;

				rw_index =0;						
				for(ix=sum=0; ix<eepwr_size-2; ix++) sum +=eepmem.buf[ix];

				if(eepmem.s.id_code !=0x1401) { eepans =1;
					if(zin.msel &0x80) { 
						__xcprintf("EEPROM Frame ID Error1 !!!\r\n");
					}
					eepctrl++;
				}
				else if(eepmem.s.checksum !=sum) { eepans =2;

					if(zin.msel &0x80) { 
               	sprintf(xdebug, "EEPROM Data Verify Error1 [eep:%04x][make:%04x]\r\n", eepmem.s.checksum, sum); mm;
					}
					eepctrl++;
				}
				else { eepans =0;
					eepread_f =0;
					memcpy(mem.buf, eepmem.buf, eepwr_size);			// read data move

					if(reteepctrl) eepctrl =reteepctrl, reteepctrl =0;
					else eepctrl =0;
				}
			}
			break;

	// eeprom read2
		case 9:
			if(Extm.s.eeprom) return;
			if((eepwr_size-rw_index) >eeprom_pagelength) {
				if(i2c_pageread(Slave_Address, rw_address+Half_Address+rw_index, eepmem.buf+rw_index, eeprom_pagelength)) {
					rw_index +=eeprom_pagelength;
					Extm.s.eeprom =3;
 				}
			}
			else {
				if(rw_index !=eepwr_size) {
					if(i2c_pageread(Slave_Address, rw_address+Half_Address+rw_index, eepmem.buf+rw_index, eepwr_size-rw_index)) {
						Extm.s.eeprom =2;
					}
					else return;
				}
				
				rw_index =0;						
				for(ix=sum=0; ix<eepwr_size-2; ix++) sum +=eepmem.buf[ix];

				if(eepmem.s.id_code !=0x1401) { eepans =1;

					if(zin.msel &0x80) { 
						__xcprintf("EEPROM Frame ID Error2 !!!\r\n");
					}
				}
				else if(eepmem.s.checksum !=sum) { eepans =2;

					if(zin.msel &0x80) { 
               	sprintf(xdebug, "EEPROM Data Verify Error2 [eep:%04x][make:%04x]\r\n", eepmem.s.checksum, sum); mm;
					}
				}
				else { eepans =0;
					memcpy(mem.buf, eepmem.buf, eepwr_size);			// read data move
				}

				eepread_f =0;
				if(reteepctrl) eepctrl =reteepctrl, reteepctrl =0;
				else eepctrl =0;
			}
			break;

//========================================================================
//========================================================================
// powerup start
		case 15: 
			M24LCXX_initial();			// iic initial
			if(zin.msel &0x80) __xcprintf("Eeprom Initial !!!\r\n");
			eepercnt =3;

			eepctrl++;
			break;

		case 16:
			eepread_f =1;	
			eepretproc(0, 17);
			break;

		case 17:
			if(eepread_f ==0) {
				if(eepans) {
					if(--eepercnt) eepctrl =16;
					else {
						if(zin.msel &0x80) __xcprintf("\r\nEeprom Read Error !!!\r\n");

						eepercnt =3;
						eepload_f =1;	eepfail_f =1;  
						eepdata_display();
						eepctrl =0;
					}
				}
				else {				// ok
    				eepercnt =3;
					eepdata_display();
					eepload_f =1;
					eepctrl =0;
				}
			}
			break;
	
		default:		eepctrl =0;		break;
	}
}

void eepdata_display(void) {

	if(zin.msel &0x80) {
		sprintf(xdebug, "\r\nEEPROM data ..(size:%d)...\r\n", sizeof(_mem)); mm;


		__linefeed();	
	}
}

void eeprom_write_go(u16 time) {

	eepdisp_f =0, eepwrite_f =1;
	if(time ==0) eepdisp_f =1, Timer_setup(6, Eep_Waittime);			// eeprom write setup
	else if(time ==0xffff);
	else eepdisp_f =1, Timer_setup(6, time); 		                  // eeprom write setup

 //  eepdisp_f =0;
}

void eepretproc(u8 setaddr, u8 retaddr) {

	eepctrl =setaddr, reteepctrl =retaddr;
}

//========================================================================

                              
