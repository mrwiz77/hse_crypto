/**
    @file        main.c
    @version     1.0.0

    @brief       HSE - Host Demo Application.
    @details     Sample application demonstrating host request flow using MU driver.

    This file contains sample code only. It is not part of the production code deliverables.
*/
/*==================================================================================================
*
*   Copyright 2022 NXP.
*
*   This software is owned or controlled by NXP and may only be used strictly in accordance with
*   the applicable license terms. By expressly accepting such terms or by downloading, installing,
*   activating and/or otherwise using the software, you are agreeing that you have read, and that
*   you agree to comply with and are bound by, such license terms. If you do not agree to
*   be bound by the applicable license terms, then you may not retain, install, activate or
*   otherwise use the software.
==================================================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
=============================================================================*/
#include "hse_demo_app_services.h"
#include "hse_global_variables.h"
#include "hse_common_types.h"
#include "hse_interface.h"
#include "hse_host_flashSrv.h"
#include "string.h"
#include "hse_monotonic_cnt.h"
#include "hse_sbaf_update.h"
#include "hse_uart.h"

/*=============================================================================
                            LOCAL MACROS
===========================================================================*/

/*=============================================================================
                            FILE VERSION CHECKS
=============================================================================*/

/*=============================================================================
                    LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
=============================================================================*/

/*
* ============================================================================
*                            LOCAL CONSTANTS
* ============================================================================
*/

/*
* ============================================================================
*                            LOCAL VARIABLES
* ============================================================================
*/

/*
* ============================================================================
*                               GLOBAL CONSTANTS
* ============================================================================
*/

/*
* ============================================================================
*                               GLOBAL VARIABLES
* ============================================================================
*/
volatile uint8_t gRunSecureBootType = 0U;
volatile eHSEFWAttributes gEnableIVTAuthBit = NO_ATTRIBUTE_PROGRAMMED;
volatile bool_t authentication_type[5U] = {TRUE, TRUE, TRUE, TRUE, TRUE};
volatile bool_t gADKPmasterbit = FALSE;
volatile bool_t debug_auth = FALSE;
volatile hseSrvResponse_t gsrvResponse = HSE_SRV_RSP_GENERAL_ERROR;
volatile eHSEFWAttributes gProgramAttributes = NO_ATTRIBUTE_PROGRAMMED;
hseAttrFwVersion_t gHseFwVersion = {0U};
volatile hseMUConfig_t WriteMu1Config = 0xFFU;
volatile hseMUConfig_t ReadMu1Config = 0xFFU;
/* variable to store HSE FW version number before and after requesting for HSE FW update*/
hseAttrFwVersion_t gOldHseFwVersion = {0};
volatile bool_t write_attr = FALSE;
volatile bool_t hsefwusageflag = 0x0UL;
volatile int8_t GetStartAsUserBit = 0x7F;
volatile int8_t GetEnableAdkmBit = 0x7F;
volatile bool_t ActivatePassiveBlock = FALSE;
volatile bool_t fwudpate_only = FALSE;
/*
* ============================================================================
*                        LOCAL FUNCTION PROTOTYPES
* ============================================================================
*/
static void reset_tests(void);
static void print_uart_menu(void);
static void wait_and_report_uart_char(void);

/* ============================================================================
*                              LOCAL FUNCTIONS
* ============================================================================
*/
/* ============================================================================
*                              GLOBAL FUNCTIONS
* ============================================================================
*/

/******************************************************************************
 * Function:    main
 * Description: Main function of the sample application.
 *          Calls different functionality example of requests issued to HSE:
 *          - Attribute programming
 *                     - example of an HSE request - set or get and attribute
 *                     for ex HSE FW version;
 *                   - HSE_FormatKeyCatalogs
 *                           - formats the key catalog with the default format;
 *                           - simulates configure step in the HSE life-cycle;
 *                           - needed for services involving keys;
 *                   - Cryptographic services
 *                           - example of requests sent synchronously;
 *                           - ImportKey, SymCipher;
 *                   - Secure boot
 *                           - example of base secure boot;
 *                           - Advanced secure boot;
 ******************************************************************************/
int main(void)
{
    testStatus = NO_TEST_EXECUTED;
    bool_t key_status = FALSE;

    /*
     * UART test configuration
     * - Instance  : LPUART6
     * - Mode      : Polling TX/RX
     * - TX pin    : PTA16
     * - RX pin    : PTA15
     * - Format    : 115200 bps, 8N1
     * - Baud base : 48 MHz source clock assumption in hse_uart.c
     *               (OSR = 16, SBR = 26)
     *
     * TX usage
     * - HSE_Uart_WriteChar('A');
     * - HSE_Uart_WriteString("UART init ok\r\n");
     *
     * RX usage
     * - char receivedChar = HSE_Uart_GetCharBlocking();
     * - HSE_Uart_GetCharBlocking() waits until one byte is received
     *   and then returns the received character.
     *
     * Note
     * - The while(1) loop above is a UART TX/RX verification path.
     * - While this loop is enabled, the original HSE demo flow below is
     *   not executed.
     */


    /* Mandatory UART initialization for LPUART6 before any TX/RX test. */
    HSE_Uart_Init();

    /* Temporary UART verification sequence starts here. */
    HSE_Uart_WriteString("UART init ok\r\n");
    HSE_Uart_WriteChar('A');

    print_uart_menu();

    while(1)
    {
    	 wait_and_report_uart_char();
    }

    /* Temporary UART verification sequence END here. */



    /* Reserved bit checked for OTA_E to OTA_E update */
    fwversion[0].reserved = gHseFwVersion.reserved;

    /* Check if HSE FW usage flag is already enabled,
     * if not enabled then do not proceed */
    WaitandSetFWEnablefeatureflag();

    /* Check Fw Install Status */
    WaitForHSEFWInitToFinish();

    /* Get HSE_FW Version */
    ASSERT(HSE_SRV_RSP_OK == HSE_GetVersion_Example(&gHseFwVersion));

    /* Check HSE_FW Type is matching with interface used */
    ASSERT(gHseFwVersion.fwTypeId == HSE_FWTYPE);

    /* Initialize Flash Driver */
    ASSERT(FLS_JOB_OK == HostFlash_Init());

    /* Check if NVM and RAM keys already formatted */
    /* format NVM and RAM key catalog */
    /* import keys for cryptographic operation and secure boot */
    HSE_DemoAppConfigKeys();



    /* After running test cases, run in infinite loop */
    while(1)
    {
        /* while(1) loop until any specific test is request to run from user */
        while (!gRunExampleTest)
        {

        }

        /* User Requested Monotonic Counter Operation */
        if (MONOTONIC_COUNTER == gProgramAttributes)
        {
            #ifdef HSE_SPT_MONOTONIC_COUNTERS
            monotonicCounterService();
            #endif
        }

        /* User requested to erase keys */
        if (APP_ERASE_NVM_KEYS == gRunExampleTest)
        {
            gsrvResponse = HSE_EraseKeys();
            if (HSE_SRV_RSP_OK == gsrvResponse)
            {
                testStatus |= NVM_DATA_ERASED;
                HSE_DemoAppConfigKeys();
            }
        }

        #ifdef HSE_SPT_SHE
        /* User Requested SHE Services Operation */
        if (SHE_COMMAND_APP_SERVICE == gProgramAttributes)
        {
            gsrvResponse = SHE_CommandApp_Service();
            if (HSE_SRV_RSP_OK == gsrvResponse)
            {
                testStatus |= SHE_SERVICES_SUCCESS;
            }
        }
        #else
            testStatus |= SHE_SERVICE_DISABLED;
        #endif

        /*
        * ADKP needs to be programmed for GMAC generation during
        * debug and boot authorization hence checking if ADKP is
        * programmed or not
        */
        if ((testStatus | ADKP_PROGRAMMING_SUCCESS) != ADKP_PROGRAMMING_SUCCESS)
        {
            key_status = check_debug_password_programmed_status();
            if (TRUE == key_status)
            {
                testStatus |= ADKP_PROGRAMMING_SUCCESS;
            }
        }

        /* User has requested attribute programming */
        if (APP_PROGRAM_HSE_ATTRIBUTE == gRunExampleTest)
        {
            attributeProgrammingService(gProgramAttributes);
        }

        /*
        * User has requested to execute all cryptographic services.
        * Keys shall be formatted prior to executing any cryptographic services.
        */
        if (APP_RUN_HSE_CRYPTOGRAPHIC_SERVICES == gRunExampleTest)
        {
            /*HSE crypto examples: sym/asym services; sync/async operation mode*/
            gsrvResponse = HSE_Crypto();
        }

        #if (defined(HSE_S32K388) && (HSE_S32K388 == HSE_PLATFORM)) 
        if(APP_RUN_ACE_TESTS == gRunExampleTest)
        {
            /* HSE example for using AES_ACCEL: import AES_ACCEL keys with HSE and execute AES crypto operations with AES_ACCEL*/
            HSE_AesAccel();
        }
        #endif

        /*
        * User requested firmware update, following sequence for fw update:
        * 1) Read firmware version of current HSE FW in the device
        * 2) Save this firmware version value
        * 3) Issue fw update request along with new address where encrypted fw
        *    image is kept that has to be updated
        * 4) Poll for HSE_INSTALL_OK bit to make sure new HSE hse installed
        * 5) Read again firmware version of newly updated HSE FW.
        * 6) Compare the saved hse fw version with newly updated HSE fw version
        * 7) If the latter is greater or equal to the former, we confirm that
        *    HSE FW update request is successful.
        */
        if (APP_HSE_FW_UPDATE == gRunExampleTest)
        {
            firmwareUpdateService();
        }
        #if defined(HSE_SPT_SMR_CR)
        if (APP_SECURE_BOOT_CONFIGURED == gRunExampleTest)
        {
            SecureBootService();
        }
        #endif /* defined(HSE_SPT_SMR_CR) */

        if(APP_SBAF_UPDATE == gRunExampleTest)
        {
            gsrvResponse = sbaf_update();
            if (HSE_SRV_RSP_OK == gsrvResponse)
            {
                testStatus |= SBAF_UPDATE_SUCCESS;
            }
        }

        /* this variable will be set by scripts when test output have been checked */
        while (!allTestExecuted)
        ;

        /* reset all variables to avoid re-run */
        reset_tests();
    }
}

static void reset_tests(void)
{
    gProgramAttributes = NO_ATTRIBUTE_PROGRAMMED;
    gsrvResponse = HSE_SRV_RSP_GENERAL_ERROR;
    gRunExampleTest = APP_NO_TEST_RUNNING;
    gEnableIVTAuthBit = NO_ATTRIBUTE_PROGRAMMED;
    gADKPmasterbit = FALSE;
    ReadMu1Config = 0U;
    WriteMu1Config = 0U;
    GetStartAsUserBit = 0x7F;
    GetEnableAdkmBit = 0x7F;
    allTestExecuted = 0U;
    write_attr = FALSE;
    fwudpate_only = FALSE;
    ActivatePassiveBlock = FALSE;
    testStatus = (testStatus_t)(NO_TEST_EXECUTED|HSE_FW_USAGE_ENABLED);
}

static void print_uart_menu(void)
{
    HSE_Uart_WriteString("\r\n=== UART Menu ===\r\n");
    HSE_Uart_WriteString("Type any character to verify UART RX.\r\n");
    HSE_Uart_WriteString("> ");
}

static void wait_and_report_uart_char(void)
{
    char receivedChar = HSE_Uart_GetCharBlocking();

    HSE_Uart_WriteString("\r\nInput [");
    HSE_Uart_WriteChar(receivedChar);
    HSE_Uart_WriteString("] received.\r\n> ");
}

#ifdef __cplusplus
}
#endif
