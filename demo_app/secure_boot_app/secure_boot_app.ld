/*==================================================================================================
*
*   Copyright 2020-2021 NXP.
*
*   This software is owned or controlled by NXP and may only be used strictly in accordance with 
*   the applicable license terms. By expressly accepting such terms or by downloading, installing, 
*   activating and/or otherwise using the software, you are agreeing that you have read, and that 
*   you agree to comply with and are bound by, such license terms. If you do not agree to 
*   be bound by the applicable license terms, then you may not retain, install, activate or 
*   otherwise use the software.
==================================================================================================*/


MEMORY {
    /* code flash area and data flash area for application given below resepectively */

    fix_addr_app_header     : org = 0x00451FC0, len = 0x40      /* ivt header for secure boot */
    flash_memory     : org = 0x00452000, len = 0x3000    /* after demo app, 2sectors of secure boot app */
    int_sram             : org = 0x20400000, len = 0x2000    /* 252K - 0x60 - 0x90 - 0x100 */
    APP_CODE_SRAM        : org = 0x20402000, len = 0x8000    /* Secure ram memory */
}

/**
 Program layout for starting in ROM, copying data to RAM,
 and continuing to execute out of ROM.
*/

SECTIONS
{
    
/***********************************************************************************************************************************************/
/*                              RAM SECTIONS                                                                                                   */
/***********************************************************************************************************************************************/
    .bss                                            : > int_sram
    .data                                ALIGN(8)   : > .
    .stack_main  ALIGN(16)                          : > .
     _Stack_end    = addr(.stack_main);
    _Stack_start  = addr(.stack_main)+sizeof(.stack_main);   
    
    
/***********************************************************************************************************************************************/
/*                              ROM SECTIONS                                                                                                   */
/***********************************************************************************************************************************************/
    ._app_header                                    : > fix_addr_app_header
    ._intc_vector_start_                ALIGN(8)                : > flash_memory    
    .ROM.intc_vector                      ROM(.intc_vector)     : > .
    ._intc_vector_end_                  ALIGN(8)                : > .
    
    .startup                                                    : > ./* Reset code */
    .systeminit                                                 : > ./* SystemInit */
    .text.startup                                               : > .
    .text                                                       : > .
    .rodata                                                     : > .

    ._const_hse_host_send_start_     ALIGN(8)                       : > .
    .ROM.hse_host_send_text          ROM(.hse_host_send_text)       : > .
    .ROM.hse_host_send_rodata        ROM(.hse_host_send_rodata)     : > .
    ._const_hse_host_send_end_       ALIGN(8)                       : > .
 
    
    .ROM.data                        ROM(.data) ALIGN(4)            : > .
    
    
/***********************************************************************************************************************************************/
/*                              RAM SECTIONS                                                                                                   */
/***********************************************************************************************************************************************/
    ._HSERAM_start                                        : > APP_CODE_SRAM
    .intc_vector                     ALIGN(4096)         : > .
    

    ._const_hse_host_send_ram_start_     ALIGN(8)         : > .    
    .hse_host_send_text                                   : { hse_host.o(.text)   "hse_mu*(.text)" } > .
    .hse_host_send_rodata                                 : { hse_host.o(.rodata) "hse_mu*(.rodata)"  } > .
    ._const_hse_host_send_ram_end_       ALIGN(8)         : > .


/*******************************************************************************/
/*                              SYMBOLS                                        */
/*******************************************************************************/

    INT_SRAM_START          = ADDR(int_sram);
    _bss_start              = ADDR(.bss);
    _bss_end                = ADDR(.bss) + SIZEOF(.bss);   // Init 4 bytes at a time
      
    RC_DATA_SRC             = ADDR(.ROM.data);
    RC_DATA_DEST            = ADDR(.data);
    RC_DATA_SIZE            = SIZEOF(.data);
    RC_DATA_SRC_END         = RC_DATA_SRC + RC_DATA_SIZE ;

    INT_VECTOR_DST_START_ADDR   = ADDR(.intc_vector);
    INT_VECTOR_SRC_START_ADDR   = ADDR(._intc_vector_start_);
    INT_VECTOR_SRC_END_ADDR = INT_VECTOR_SRC_START_ADDR + SIZEOF(.intc_vector);
      
    HSE_HOST_RAM_DST_START_ADDR       = addr(._const_hse_host_send_ram_start_);
    HSE_HOST_FLASH_SRC_START_ADDR     = addr(._const_hse_host_send_start_);
    HSE_HOST_FLASH_SRC_END_ADDR       = addr(._const_hse_host_send_end_);
}

