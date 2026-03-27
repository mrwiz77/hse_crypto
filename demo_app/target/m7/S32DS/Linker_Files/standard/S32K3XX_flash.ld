/*****************************************************************************
 *
 * Copyright 2020 NXP
 * All Rights Reserved
 *
 *****************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
DEMO_APP_BSS_MEM_START = 0x21002000 ;
DEMO_APP_BSS_SIZE = 56K;

IVT_START_ADDR = 0x20400000 ;
IVT_SIZE = 256;   

ADKP_START_ADDR = IVT_START_ADDR + IVT_SIZE;
ADKP_SIZE = 16;

DEMO_APP_SRAM_START_ADDR = 0x20401000;
DEMO_APP_SRAM_SIZE = 24K;
HSE_FW_VERSION_START_ADDR = DEMO_APP_SRAM_START_ADDR + DEMO_APP_SRAM_SIZE;
HSE_FW_VERSION_SIZE =32;
TEST_STATUS_START_ADDR = HSE_FW_VERSION_START_ADDR + HSE_FW_VERSION_SIZE;
TEST_STATUS_START_LENGTH = 0x4;
TEST_STATUS_EXECUTED_ADDR = TEST_STATUS_START_ADDR + TEST_STATUS_START_LENGTH;
TESTEXECUTED_LENGTH = 0x4;
TESTCOMPLETED_START_ADDR = TEST_STATUS_EXECUTED_ADDR + TESTEXECUTED_LENGTH;
TESTCOMPLETED_LENGTH = 0x1;


__STACK_SIZE = 0x1000;

/* Linker script to configure memory regions. */

MEMORY
{
  FLASH_VTABLE  (R)   : ORIGIN = 0x00402000, LENGTH = 0x1000  	/* Vector Table */
  FLASH         (RX)  : ORIGIN = 0x00403000, LENGTH = 0x2000  	/* code section for demo app */
  FLASH_P2      (RX)  : ORIGIN = 0x00405000, LENGTH = 0x17000  /* code section for demo app */
  INT_SRAM      (RW)  : ORIGIN = DEMO_APP_BSS_MEM_START, LENGTH = DEMO_APP_BSS_SIZE    /* 56KB RAM */
  IVT_ADDR      (RW)  : ORIGIN = IVT_START_ADDR, LENGTH = IVT_SIZE     /* fixed address for IVT structure */
  ADKP_KEY      (RW)  : ORIGIN = ADKP_START_ADDR, LENGTH = ADKP_SIZE      /* fixed address for key input by user */
  APP_CODE_SRAM (RWX) : ORIGIN = DEMO_APP_SRAM_START_ADDR, LENGTH = DEMO_APP_SRAM_SIZE    /* Secure ram memory */
  HSE_FW_VERSION(RW)  : ORIGIN = HSE_FW_VERSION_START_ADDR, LENGTH = HSE_FW_VERSION_SIZE
  TESTSTATUS    (RW)  : ORIGIN = TEST_STATUS_START_ADDR, LENGTH = TEST_STATUS_START_LENGTH
  TESTEXECUTED  (RW)  : ORIGIN = TEST_STATUS_EXECUTED_ADDR, LENGTH = TESTEXECUTED_LENGTH
  TESTCOMPLETED (RW)  : ORIGIN = TESTCOMPLETED_START_ADDR, LENGTH = 0x1
}

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __ecc_table_start__
 *   __ecc_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */

STACK_SIZE = DEFINED(__STACK_SIZE) ? __STACK_SIZE : 0x1000;

ENTRY(Reset_Handler)

SECTIONS
{
/***********************************************************************************************************************************************/
/*                              FLASH SECTIONS                                                                                                 */
/***********************************************************************************************************************************************/
    INT_VECTOR_SRC_START_ADDR    = ORIGIN(FLASH_VTABLE);


/***********************************************************************************************************************************************/
/*                              RAM EXECUTABLE SECTION                                                                                         */
/***********************************************************************************************************************************************/
  /*._HSERAM_start                                                : {} > APP_CODE_SRAM */

  .intc_vector   : AT (INT_VECTOR_SRC_START_ADDR)
  {
      
  } > APP_CODE_SRAM
    
  INT_VECTOR_DST_START_ADDR                                     = ADDR(.intc_vector);
  INT_VECTOR_SRC_END_ADDR                                       = INT_VECTOR_SRC_START_ADDR + SIZEOF(.intc_vector);
  
/***********************************************************************************************************************************************/
/*                              FLASH SECTIONS                                                                                                 */
/***********************************************************************************************************************************************/
   
    .startup                                                    :
    {
        *(.startup)
    } > FLASH
    
    .systeminit                                                 : {} > FLASH
       
    .text.startup                                               :
    {
        *startup*.o(.text)
    }  > FLASH
   
   FLASH_LAST                                                   = .;
   
/***********************************************************************************************************************************************/
/*                              RAM EXECUTABLE SECTION                                                                                         */
/***********************************************************************************************************************************************/
   
   FLASH_DRIVER_FLASH_SRC_START_ADDRESS                         = FLASH_LAST;
  .flash_driver_text                                            : AT (FLASH_DRIVER_FLASH_SRC_START_ADDRESS)
  { 
    . = ALIGN(8);
    __flash_driver_start__ = .;
    
    *FlashErase.o(.text*)
    *FlashProgram.o(.text*)
    
    . = ALIGN(8);
    __flash_driver_end__ = .;
  } > APP_CODE_SRAM

  FLASH_DRIVER_RAM_DST_START_ADDRESS                            = __flash_driver_start__;
  FLASH_DRIVER_SIZE                                                = SIZEOF(.flash_driver_text);
  FLASH_DRIVER_FLASH_SRC_END_ADDRESS                            = FLASH_DRIVER_FLASH_SRC_START_ADDRESS + FLASH_DRIVER_SIZE; 
   
   
   HSE_HOST_FLASH_SRC_START_ADDR                                = FLASH_DRIVER_FLASH_SRC_END_ADDRESS;
  .hse_host_send_text                                           : AT (HSE_HOST_FLASH_SRC_START_ADDR)
  {
      . = ALIGN(8);
    __host_driver_start__ = .;
    
    *hse_host.o(.text*)
    *hse_host.o(.rodata*)
    *hse_mu.o(.text*)
    *hse_mu.o(.rodata*)
    
    . = ALIGN(8);
    __host_driver_end__ = .;
  } > APP_CODE_SRAM
  
  HSE_HOST_RAM_DST_START_ADDR                                   = __host_driver_start__;
  HSE_HOST_FLASH_SRC_END_ADDR                                   = HSE_HOST_FLASH_SRC_START_ADDR + SIZEOF(.hse_host_send_text);
    
/***********************************************************************************************************************************************/
/*                              FLASH SECTIONS                                                                                                 */
/***********************************************************************************************************************************************/
   
    .text                                                       :
    {
        *(.text)                 
        *(.text*)                   
    } > FLASH_P2
    
    .rodata                                                     :
    {
          *(.rodata)               
            *(.rodata*)              
    } > FLASH_P2
          
    RC_DATA_SRC = .;

/***********************************************************************************************************************************************/
/*                              RAM SECTIONS                                                                                                   */
/***********************************************************************************************************************************************/

  .bss                                                          : 
  {
    *(.bss)
    *(.bss.*)
  } > INT_SRAM
  
  .data : AT(RC_DATA_SRC)
  {
  . = ALIGN(4);
  __data_start__ = .;
  
    *(.data)
    *(.data.*)
  
  . = ALIGN(4);
  __data_end__ = .;
  } > INT_SRAM
  
  RC_DATA_DEST                                                  = __data_start__;
  RC_DATA_SIZE                                                  = (__data_end__ - __data_start__);
  RC_DATA_SRC_END                                               = RC_DATA_SRC + RC_DATA_SIZE ;
  
  .stack_main                                                   ALIGN(16)   : {} > INT_SRAM
  
  _Stack_end                                                    = ADDR(.stack_main);
  _Stack_start                                                  = ADDR(.stack_main) + STACK_SIZE;   

/*******************************************************************************/
/*                              RAM SECTION FOR ADKP KEY                       */
/*******************************************************************************/
    ._adkp_key                                                  : {} > ADKP_KEY
    
    ADKP_RAM_START                                              = ADDR(._adkp_key);
    ADKP_DATA_SIZE                                              = 16;
    ADKP_RAM_END                                                = ADKP_RAM_START + ADKP_DATA_SIZE; 
    
/*******************************************************************************/
/*                              RAM SECTION FOR IVT                           */
/*******************************************************************************/
    .ivt                                                        : {} > IVT_ADDR
    IVT_RAM_START                                               = ADDR(.ivt);
    IVT_AND_APP_DATA_SIZE                                       = 0x100 + 0x1000;
    APP_RAM_END                                                 = IVT_RAM_START + IVT_AND_APP_DATA_SIZE; 

/*******************************************************************************/
/*                              RAM SECTION FOR FW VERSION                     */
/*******************************************************************************/
    .fwversion                                                  : {} > HSE_FW_VERSION 
/*******************************************************************************/
/*                              RAM SECTION FOR STATUS UPDATE                  */
/*******************************************************************************/
    .teststatus                                                  : {} > TESTSTATUS
    .testexecuted                                                : {} > TESTEXECUTED
    .testcompleted                                               : {} > TESTCOMPLETED
/***********************************************************************************************************************************************/
/*                              SYMBOLS                                                                                                        */
/***********************************************************************************************************************************************/

    INT_SRAM_START                                              = ADDR(.bss);
    _bss_start                                                  = ADDR(.bss);
    _bss_end                                                    = ADDR(.bss) + SIZEOF(.bss);
}
