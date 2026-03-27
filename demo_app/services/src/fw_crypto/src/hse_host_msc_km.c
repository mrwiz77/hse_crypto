/**
*   @file    hse_host_msc_km.c
*
*   @brief   This file implements wrappers for MSC(Managed Security Component) key management services.
*
*   @addtogroup [HOST_FRAMEWORK]
*   @{
*/
/*==================================================================================================
*
*   Copyright 2022 NXP
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

/*==================================================================================================
 *                                        INCLUDE FILES
 ==================================================================================================*/

#include "string.h"
#include "global_defs.h"
#include "hse_default_config.h"
#include "hse_host.h"
#include "hse_host_km_utils.h"
#include "hse_demo_app_services.h"

#if (defined(HSE_S32K388) && (HSE_S32K388 == HSE_PLATFORM))
/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 ==================================================================================================*/

/*==================================================================================================
 *                                       LOCAL MACROS
 ==================================================================================================*/

/*==================================================================================================
 *                                      LOCAL CONSTANTS
 ==================================================================================================*/
/**< @brief The Key Handle Translation Table (KHTT) used to configure the HSE KHTT */
static const hseKHTTEntry_t gKHTT[] __attribute__((aligned)) = {HSE_KHTT_CFG};
/*==================================================================================================
 *                                      LOCAL VARIABLES
 ==================================================================================================*/
static uint8_t muChannelIdx = 1U; /* channel 1 */
/*==================================================================================================
 *                                      GLOBAL CONSTANTS
 ==================================================================================================*/

/*==================================================================================================
 *                                      GLOBAL VARIABLES
 ==================================================================================================*/

/*==================================================================================================
 *                                   LOCAL FUNCTION PROTOTYPES
 ==================================================================================================*/

/*==================================================================================================
 *                                       LOCAL FUNCTIONS
 ==================================================================================================*/

/*==================================================================================================
 *                                       GLOBAL FUNCTIONS
 ==================================================================================================*/

/*************************************************************************************************
* Description:  Configures the Key Handle Translation Table
************************************************************************************************/

static hseKHTTEntry_t KHTT[] =
{
    HSE_KHTT_CFG
};

hseSrvResponse_t GetMscKeySlotIdxFromKhtt(hseKeyHandle_t keyHandle, uint8_t *pMscKeySlotIdx)
{
    hseSrvResponse_t status = HSE_SRV_RSP_INVALID_PARAM;
    uint16_t index          = 0U;

    /* Go through the configured KHTT entries */
    for(index = 0U; index < (sizeof(KHTT)/sizeof(KHTT[0])); index++)
    {
        /* Check for the key handle */
        if(KHTT[index].hseKeyHandle == keyHandle)
        {
            /* Get the key slot index */
            *pMscKeySlotIdx = KHTT[index].mscKeySlotIdx;
            status = HSE_SRV_RSP_OK;
            break;
        }
    }

    return status;
}

hseSrvResponse_t EraseAceKey(hseKeyHandle_t keyHandle, hseEraseKeyOptions_t eraseKeyOptions)
{
    hseSrvResponse_t  hseStatusEraseKey     = HSE_SRV_RSP_GENERAL_ERROR;
    hseSrvDescriptor_t *pHseSrvDesc         = &gHseSrvDesc[0][0];
    hseEraseKeySrv_t *pEraseKey             = &pHseSrvDesc->hseSrv.eraseKeyReq;

    /* Set to 0 in case other test cases modified them */
    memset(pHseSrvDesc, 0, sizeof(hseSrvDescriptor_t));

    /* Send request to HSE */
    pEraseKey->keyHandle       = keyHandle;
    pEraseKey->eraseKeyOptions = eraseKeyOptions;

    pHseSrvDesc->srvId = HSE_SRV_ID_ERASE_KEY;

        /* Send request to HSE */
    hseStatusEraseKey = HSE_Send(0U, 1U, gSyncTxOption, pHseSrvDesc);
    if(HSE_SRV_RSP_OK != hseStatusEraseKey)
    {
        goto exit;
    }

exit:
    return hseStatusEraseKey;
}

hseSrvResponse_t HSE_ConfigKHTT(void)
{
    hseSrvDescriptor_t *pHseSrvDesc;
    hseConfigKHTTSrv_t *pConfigKHTTReq;
    hseSrvResponse_t srvResponse = HSE_SRV_RSP_GENERAL_ERROR;

    /* Clear the service descriptor placed in shared memory */
    pHseSrvDesc = &gHseSrvDesc[MU0][muChannelIdx];
    pConfigKHTTReq = &(pHseSrvDesc->hseSrv.configKHTT);
    memset(pHseSrvDesc, 0, sizeof(hseSrvDescriptor_t));

    /* Fill the service descriptor */
    pHseSrvDesc->srvId = HSE_SRV_ID_CONFIG_KHTT;

    /* Fill the number of entries in KHTT and the pointer to the KHTT */
    pConfigKHTTReq->numOfKHTTEntries = (uint16_t)(sizeof(gKHTT)/sizeof(gKHTT[0]));
    pConfigKHTTReq->pKHTT = PTR_TO_HOST_ADDR(gKHTT);

    /* Send the request synchronously */
    srvResponse = HSE_Send(MU0, muChannelIdx, gSyncTxOption, pHseSrvDesc);

    return srvResponse;
}

/*************************************************************************************************
* Description:  Push one or more HSE keys in ACE key store
************************************************************************************************/
hseSrvResponse_t HSE_PushMscKey(hseKeyHandle_t keyHandle)
{
    hseSrvDescriptor_t *pHseSrvDesc;
    hsePushMscKeySrv_t *pPushMscKeyReq;
    hseSrvResponse_t srvResponse = HSE_SRV_RSP_GENERAL_ERROR;

    /* Clear the service descriptor placed in shared memory */
    pHseSrvDesc = &gHseSrvDesc[MU0][muChannelIdx];
    pPushMscKeyReq = &(pHseSrvDesc->hseSrv.pushMscKey);
    memset(pHseSrvDesc, 0, sizeof(hseSrvDescriptor_t));

    /* Fill the service descriptor */
    pHseSrvDesc->srvId = HSE_SRV_ID_PUSH_MSC_KEYS;

    /* Set the key handle to be pushed to MSC keystore */
    pPushMscKeyReq->hseKeyHandle = keyHandle;

    /* Send the request synchronously */
    srvResponse = HSE_Send(MU0, muChannelIdx, gSyncTxOption, pHseSrvDesc);

    return srvResponse;
}

#endif /* HSE_PLATFORM == HSE_S32K388 */

#ifdef __cplusplus
}
#endif

/** @} */

