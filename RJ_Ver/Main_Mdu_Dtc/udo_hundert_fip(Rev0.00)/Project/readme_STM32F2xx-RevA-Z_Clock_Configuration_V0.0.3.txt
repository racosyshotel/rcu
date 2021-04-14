/******************************************************************************/
/*                  STM32F2xx-RevA-Z_Clock_Configuration                      */
/*                                V0.0.3                                      */
/******************************************************************************/

============
Introduction
============
The aim of this xls tool is to:
    - Configure the system clock source and output frequency
    - Configure the Flash settings (wait state, Prefetch Buffer, Data cache and Instruction cache)
    - AHB, APB1 and APB2 frequencies
    - And finally generate ready-to-use "system_stm32f2xx.c" file with all settings
      above. Then you have just to add this file to your application sources.

NOTE: 
====
  - This version supports Silicon RevA (Cut1.0)and RevZ (Cut1.1)
  - !!!  This program work only if "macros" are enabled in "EXCEL"  !!!  
     
==============
How to proceed
==============
Tow configuration modes are available:

 -----------
 Wizard mode
 -----------
 You have to proceed as follows:
    1. Select whether to enable or not the Prefetch Buffer, Data cache and/or Instruction cache 
    2. If the HSE is used in your application, adjust its frequency (in 4-25MHz range)
    3. Specify if it's required to have 48MHz clock for USB OTG FS, SDIO and/or RNG (this will
       add a constraint on the PLL parameters configuration) 
    4. Set the desired SYSCLK frequency (max 120 MHz, depending on the application supply voltage )
    5. Adjust the AHB, APB1 and APB2 prescalers
    6. Click on "RUN" button, to check if the requirements you defined above can be achieved:
         - If yes, a message box will appear and you will be asked to select the PLL
           clock source (can be HSE or HSI).
         - Otherwise, a message will be displayed indicating the conflict preventing
           to meet the defined requirements. Then you have to change the requirement
           to resolve this conflict. 
    7. Finally click on "Generate" button to generate automatically  the "system_stm32f2xx.c" file  
       
 -----------
 Expert mode
 -----------  
 This mode provides more flexibility in the configuration but it's up to the user
 to make sure that what it is configured is correct. 
 You have to proceed as follows:
    1. Select whether to enable or not the Prefetch Buffer, Data cache and/or Instruction cache 
    2. If the HSE is used in your application, adjust its frequency (in 4-25MHz range)
    3. Configure the System clock source (PLL, HSE or HSI). In case of the PLL is used 
       as System clock source you have to configure the parameters: N, P, Q and R. 
    4. Set the Flash wait states  
    5. Adjust the AHB, APB1 and APB2 prescalers
    6. Finally click on "Generate" button to generate automatically the "system_stm32f2xx.c" file

================
Known limitation
================
 1. The CPU maximum frequency (and Flash wait states) setting vs. supply voltage 
    is not correctly managed by this version.

    Workaround: user should make sure that the target CPU frequency is within the 
    ----------  allowed frequency range relative to the application supply voltage. 
    
    The number of wait states must be correctly programmed according to the frequency
    of the CPU clock (HCLK) and the supply voltage of the device:
 ---------------------------------------------------------------------------------     
 | Latency       |                HCLK clock frequency (MHz)                      |
 |               |----------------------------------------------------------------|     
 |               | voltage range  | voltage range | voltage range | voltage range |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V | 2.1 V - 2.4 V | 1.62 V - 2.1 V|
 |---------------|----------------|---------------|---------------|---------------|              
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24 |0 < HCLK <= 18 |0 < HCLK <= 16 |
 |---------------|----------------|---------------|---------------|---------------|   
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48|18 < HCLK <= 36|16 < HCLK <= 32| 
 |---------------|----------------|---------------|---------------|---------------|   
 |2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72|36 < HCLK <= 54|32 < HCLK <= 48|
 |---------------|----------------|---------------|---------------|---------------| 
 |3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96|54 < HCLK <= 72|48 < HCLK <= 64|
 --------------------------------------------------------------------------------- 
