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

#include "hse_interface.h"
#include "hse_host.h"
#include "string.h"
/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/
#ifdef GHS
#pragma ghs section const = "._app_header"
    const unsigned int app_header[] = {
#pragma ghs section const = default
#else
unsigned int __attribute__((section("._app_header"))) app_header[16] = {
#endif
    0x60FFFFD5U,
    0x0U,
    0x00452000U,
    0x00003000U,
    0x0U,
    0x0U,
    0x0U,
    0x0U,
    0x0U,
    0x0U,
    0x0U,
    0x0U,
    0x0U,
    0x0U,
    0x0U,
    0x0U};

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
unsigned int counter = 0U;
unsigned int secure_boot_status = 0U;
/*typedef struct
    - {
    -     unsigned int smrStatus[2U];         @/@*$*< @brief  0-31 bit will represent 32 SMR table entries (applicable when SMR is present/enabled).<br>
    -                                                 - smrStatus[0].bit : 0 - SMR Not Verified
    -                                                 - smrStatus[0].bit : 1 - SMR Verified
    -                                                 - smrStatus[1].bit : 0 - SMR verification fail
    -                                                 - smrStatus[1].bit : 1 - SMR verification pass$*@/@
    -     unsigned int coreBootStatus[2U];   @/@*$*< @brief   0-31 bit will represent CORE-ID (0-31).<br>
    -                                                 - coreBootStatus[0].bit : 1 - Core booted
    -                                                 - coreBootStatus[0].bit : 0 - Core Not booted
    -                                                 - coreBootStatus[1].bit : 1 - Core booted with pass/primary reset address
    -                                                 - coreBootStatus[1].bit : 0 - Core booted with alternate/backup reset address $*@/@
    - } hseAttrSmrCoreStatus_t;*/

/*verify if secure boot is success*/
hseAttrSmrCoreStatus_t smrCoreStatus_Get;
extern uint32_t HSE_HOST_RAM_DST_START_ADDR[];
extern uint32_t HSE_HOST_FLASH_SRC_START_ADDR[];
extern uint32_t HSE_HOST_FLASH_SRC_END_ADDR[];
/*==================================================================================================

                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static unsigned int GetAttr(hseAttrId_t attrId, unsigned int attrLen, void *pAttr);
/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL FUNCTIONS
==================================================================================================*/

/**
 * @brief        Main function of the sample application.
 * @details      Calls different functionality example of requests issued to HSE:
 *                   - HSE_GetVersion_Example
 *                           - example of an HSE request - retrieving the FW version;
 *                   - HSE_FormatKeyCatalogs
 *                           - formats the key catalog with the default format;
 *                           - simulates configure step in the HSE life-cycle;
 *                           - needed for services involving keys;
 *                   - HSE_SyncReqExample
 *                           - example of requests sent synchronously;
 *                           - ImportKey, SymCipher;
 *                   - HSE_AsyncReqExample
 *                           - example of requests sent asynchronously;
 *                           - Hash;
 */
int main(void)
{
    unsigned int srvResponse = 0U;
    // this is added to avoid S32DS optimization
    unsigned int appheader = app_header[0];
    while (appheader != 0x60FFFFD5U)
        ;

    srvResponse = GetAttr(HSE_SMR_CORE_BOOT_STATUS_ATTR_ID, sizeof(hseAttrSmrCoreStatus_t), (void *)(&smrCoreStatus_Get));
    if (0x55A5AA33UL == srvResponse)
    {
        while (1)
        {
            counter++;
            if (counter != 0U)
            {
                secure_boot_status = 1U;
            }
            else
            {
                secure_boot_status = 0U;
            }
        }
    }
}

static unsigned int GetAttr(hseAttrId_t attrId, unsigned int attrLen, void *pAttr)
{
    unsigned int hseSrvResponse = 0x55A5C565UL;
    hseSrvDescriptor_t *pHseSrvDesc = &gHseSrvDesc[0][1];

    memset(pHseSrvDesc, 0, sizeof(hseSrvDescriptor_t));

    pHseSrvDesc->srvId = HSE_SRV_ID_GET_ATTR;
    pHseSrvDesc->hseSrv.getAttrReq.attrId = attrId;
    pHseSrvDesc->hseSrv.getAttrReq.attrLen = attrLen;
    pHseSrvDesc->hseSrv.getAttrReq.pAttr = (HOST_ADDR)pAttr;

    hseSrvResponse = HSE_Send(0, 1, gSyncTxOption, pHseSrvDesc);
    return hseSrvResponse;
}

void CopyFlashToSRAMHseHost(void)
{
    volatile uint32_t *FlashRamAddr = 0U;
    volatile uint32_t *FlashSrcAddr = 0U;
    volatile uint32_t *FlashSrcEndAddr = 0U;
    uint32_t i = 0U;

    FlashRamAddr = HSE_HOST_RAM_DST_START_ADDR;
    FlashSrcAddr = HSE_HOST_FLASH_SRC_START_ADDR;
    FlashSrcEndAddr = HSE_HOST_FLASH_SRC_END_ADDR;

    /* Copying the Flash Driver Code from flash to RAM to avoid read while write violation */
    for (i = 0U; i < (((uint32_t)FlashSrcEndAddr - (uint32_t)FlashSrcAddr) / 4U); i++)
    {
        FlashRamAddr[i] = FlashSrcAddr[i];
    }
}
