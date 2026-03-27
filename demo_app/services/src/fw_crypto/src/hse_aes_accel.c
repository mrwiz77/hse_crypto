/**
*   @file    hse_aes_accel.c
*
*   @brief   Example of AES_ACCEL usage with HSE.
*   @details Example of import ACE+ keys with HSE and execute AES operations with DMA requests.
*
*   @addtogroup hse_aes_accel
*   @{
*/
/*==================================================================================================
*
*   Copyright 2019-2022 NXP
*
*   This software is owned or controlled by NXP and may only be used strictly in accordance with
*   the applicable license terms. By expressly accepting such terms or by downloading, installing,
*   activating and/or otherwise using the software, you are agreeing that you have read, and that
*   you agree to comply with and are bound by, such license terms. If you do not agree to
*   be bound by the applicable license terms, then you may not retain, install, activate or
*   otherwise use the software.
==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/

#include "hse_global_variables.h"
#include "S32K388.h"
#include "hse_global_types.h"
#include "global_defs.h"
#include "hse_host_km_import_key.h"
#include "hse_host_msc_km.h"
#include "hse_host.h"
#include "hse_default_config.h"
#include "string.h"

#if (defined(HSE_S32K388) && (HSE_S32K388 == HSE_PLATFORM))
/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef uint8_t aceCryptoOperation_t;
#define ACE_VERIFY_SIGN    ((aceCryptoOperation_t)0x00U)
#define ACE_DECRYPT_DATA   ((aceCryptoOperation_t)0x01U)
#define ACE_ENCRYPT_DATA   ((aceCryptoOperation_t)0x02U)
#define ACE_GENERATE_SIGN  ((aceCryptoOperation_t)0x03U)

typedef uint8_t aceCryptoMode_t;
#define ACE_MODE_CMAC      ((aceCryptoMode_t)0x08U)
#define ACE_MODE_GCM       ((aceCryptoMode_t)0x06U)
#define ACE_MODE_OFB       ((aceCryptoMode_t)0x05U)
#define ACE_MODE_CFB       ((aceCryptoMode_t)0x04U)
#define ACE_MODE_CTR       ((aceCryptoMode_t)0x03U)
#define ACE_MODE_CBC       ((aceCryptoMode_t)0x01U)
#define ACE_MODE_ECB       ((aceCryptoMode_t)0x00U)

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
/* AES_ACCEL registers base address */
#define AES_ACCEL_DATA_IN_ADDRESS     0x44000008U
#define AES_ACCEL_STATUS_ADDRESS      0x44000100U
#define AES_ACCEL_CONTROL_ADDRESS     0x44000108U
#define AES_ACCEL_RESULT_ADDRESS      0x44000118U
#define AES_ACCEL_AUTH_TAG_ADDRESS    0x44000120U

/* AES_ACCEL NO_ERROR status */
#define ACE_STATUS_NO_ERROR           0x00000014U

#define MC_ME_BASE_ADDR                 0x402DC000U
#define PRTN1_COFB3_CLK_EN_ADDR         (MC_ME_BASE_ADDR + 0x33CU)
#define PRTN1_COFB3_CLK_EN_MASK         (0xF0000U)
#define PRTN1_COFB3_CLK_EN(ind)         (*(volatile uint32_t *)PRTN1_COFB3_CLK_EN_ADDR |= PRTN1_COFB3_CLK_EN_MASK << ind)

#define PRTN1_PCONF_ADDR                (MC_ME_BASE_ADDR + 0x300U)
#define PRTN1_PCONF_EN()                (*(volatile uint32_t *)PRTN1_PCONF_ADDR |= 0X01U)

#define PRTN1_PUPD_ADDR                 (MC_ME_BASE_ADDR + 0x304U)
#define PRTN1_PUPD_EN()                 (*(volatile uint32_t *)PRTN1_PUPD_ADDR |= 0X01U)

#define MC_ME_CTL_KEY_ADDR              (MC_ME_BASE_ADDR)
#define MC_ME_CTL_KEY_EN(key)           (*(volatile uint32_t *)MC_ME_CTL_KEY_ADDR = key)

/* Macro to enable AES_ACCEL */
#define ACE_CONTROL_ENABLE()                                                                  \
    /* Setting bit to enable ACE */                                                           \
    do{                                                                                       \
        if (((*(volatile uint32_t *)AES_ACCEL_CONTROL_ADDRESS) & 0x00000001U) != 0x00000001U) \
        {                                                                                     \
            *(volatile uint32_t *)AES_ACCEL_CONTROL_ADDRESS = 0x00000001U;                    \
        }                                                                                     \
    }while(0U)
/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/



/* AES_ACCEL keys & data */
static uint8_t aes128AceKey[16] =
{
    0x10, 0xA5, 0x88, 0x69, 0xD7, 0x4B, 0xE5, 0xA3,
    0x74, 0xCF, 0x86, 0x7C, 0xFB, 0x47, 0x38, 0x59
};

static uint8_t aes128AceSecondKey[16] =
{
    0x38, 0xF5, 0x68, 0xAA, 0x32, 0xF1, 0x9A, 0x84,
    0xD7, 0x62, 0xB2, 0xC5, 0x2F, 0x64, 0x2C, 0x19
};

static uint8_t aes256AceKey[32] =
{
    0x5B, 0x30, 0xD2, 0x11, 0x4A, 0x96, 0xD2, 0x3C,
    0xB4, 0xAF, 0x57, 0xB1, 0xF6, 0x64, 0x5F, 0x5B,
    0x19, 0xCB, 0x9A, 0xC9, 0x9B, 0x86, 0x3C, 0x8C,
    0xDC, 0x6B, 0x30, 0x59, 0xE0, 0xF7, 0x46, 0xE3
};

static uint64_t aceStatus              = 0U;
static uint8_t ecbPlainText[16]        = {0x51, 0x2d, 0x74, 0xa9, 0x19, 0xab, 0xa8, 0x35, 0xeb, 0x38, 0x8d, 0x13, 0x92, 0x89, 0x05, 0x17};
static uint8_t cmacPlainText[16]       = {0x51, 0x2d, 0x74, 0xa9, 0x19, 0xab, 0xa8, 0x35, 0xeb, 0x38, 0x8d, 0x13, 0x92, 0x89, 0x05, 0x17};

static uint8_t ecbCipherOutput[16]     = { 0U };
static uint8_t cmacTagOutput[16]       = { 0U };

static uint8_t aes128EcbCipher[16]     = {0xF4, 0xD8, 0x16, 0xFF, 0x2C, 0xBD, 0xE3, 0x6E, 0x1D, 0x60, 0x48, 0x2B, 0x27, 0xDE, 0xE4, 0x03};
static uint8_t aes256EcbCipher[16]     = {0x3B, 0x6D, 0x50, 0x9D, 0x03, 0x80, 0x40, 0xC7, 0x8D, 0x77, 0xAA, 0x85, 0x90, 0x53, 0xFF, 0x3B};

static uint8_t aes128CmacTag[16]       = {0xE2, 0xD2, 0x75, 0xC1, 0xAC, 0x02, 0x44, 0x02, 0x6C, 0x55, 0x2A, 0xCD, 0xF0, 0x14, 0x0A, 0xC9};
static uint8_t aes128CmacSecondTag[16] = {0xF6, 0x90, 0x43, 0x94, 0x1B, 0x43, 0x96, 0xD5, 0x01, 0xE2, 0xF5, 0x68, 0xD4, 0x6F, 0x05, 0x05};
static uint8_t aes256CmacTag[16]       = {0x83, 0x69, 0x9C, 0x32, 0xAD, 0x4C, 0x46, 0x4A, 0x4B, 0x7D, 0x3B, 0x8E, 0x77, 0x0E, 0xBD, 0xDD};

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/
exampleState_t gAesAccelExample = NOT_RUN;

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*************************************************************************************************
* Description:  Function for sending a DMA request to execute a AES crypto operation with AES_ACCEL
************************************************************************************************/
static void RunAceAesOperationWithDMA
(
    aceCryptoOperation_t operation,
    aceCryptoMode_t mode,
    uint8_t aceKeySlotIndex,
    uint8_t *pIn,
    uint16_t inLen,
    uint8_t *pOut,
    uint16_t outLen,
    uint64_t *pAceStatus
)
{
    memset(pOut, 0U, outLen);
    memset(pAceStatus, 0U, sizeof(pAceStatus));

    /* Setting Application parameters for Param 0
       No Interrupt is enabled for FEED/RESULT DMA
       Timer is also not enabled here */
    AES__AES_ACCEL_LEN0   = (HSE_BYTES_TO_BITS(inLen) & 0x0007FFFFU); // TLEN - 19 bits
    AES__AES_ACCEL_CRYPT0 = (uint32_t)((aceKeySlotIndex << 16U) | (operation << 12U) | (mode << 8U));

    /* Enable channel linking and did replication on feed
       DMA-MP-CSR register */
    AES__FEED_DMA_MP_CSR |= 0x000000C0U;

    /**********************************************************************
        Setting up TCD0 for feed DMA
    **********************************************************************/

    /* Clearing Done bit incase in it used before */
    AES__FEED_DMA_TCD_CH0_CSR |= 0x40000000U;

    /* Enable ID replication */
    AES__FEED_DMA_TCD_CH0_SBR |= 0x00010000U;

    /* Setting SADDR and DADDR */
    AES__FEED_DMA_TCD_TCD0_SADDR = (uint32_t)pIn;
    AES__FEED_DMA_TCD_TCD0_DADDR = AES_ACCEL_DATA_IN_ADDRESS;

    /* Setting DSIZE, SSIZE */
    AES__FEED_DMA_TCD_TCD0_ATTR = 0x0000U;

    /* Setting NBYTES */
    AES__FEED_DMA_TCD_TCD0_NBYTES_MLOFFNO = inLen;

    /* Setting CITER, BITER */
    AES__FEED_DMA_TCD_TCD0_CITER_ELINKNO = 0x0001U;
    AES__FEED_DMA_TCD_TCD0_BITER_ELINKNO = 0x0001U;

    // Setting SOFF, DOFF
    AES__FEED_DMA_TCD_TCD0_SOFF = 0x0001U;
    AES__FEED_DMA_TCD_TCD0_DOFF = 0x0000U;

    /* Setting DREQ of Feed DMA */
    AES__FEED_DMA_TCD_TCD0_CSR |= 0x0008U;

    /**********************************************************************
        Setting up TDC0 for Result DMA
    **********************************************************************/
    /* Enable channel linking and did replication on feed
       DMA-MP-CSR register */
    AES__RESULT_DMA_MP_CSR |= 0x000000C0U;

    /* Clearing Done bit incase in it used before */
    AES__RESULT_DMA_TCD_CH0_CSR |= 0x40000000U;

    /* Enable ID replication */
    AES__RESULT_DMA_TCD_CH0_SBR |= 0x00010000U;

    /* Setting SADDR and DADDR */
    if((ACE_ENCRYPT_DATA == operation) || (ACE_DECRYPT_DATA == operation))
    {
        AES__RESULT_DMA_TCD_TCD0_SADDR = AES_ACCEL_RESULT_ADDRESS;
        AES__RESULT_DMA_TCD_TCD0_DADDR = (uint32_t)pOut;
    }
    if(ACE_GENERATE_SIGN == operation)
    {
        AES__RESULT_DMA_TCD_TCD0_SADDR = AES_ACCEL_AUTH_TAG_ADDRESS;
        AES__RESULT_DMA_TCD_TCD0_DADDR = (uint32_t)pOut;
    }
    if(ACE_VERIFY_SIGN == operation)
    {
        AES__RESULT_DMA_TCD_TCD0_SADDR = (uint32_t)pOut; /* AUTH_TAG from memory */
        AES__RESULT_DMA_TCD_TCD0_DADDR = AES_ACCEL_AUTH_TAG_ADDRESS;
    }

    /* Setting DSIZE, SSIZE */
    AES__RESULT_DMA_TCD_TCD0_ATTR = 0x0000U;

    /* Setting NBYTES */
    AES__RESULT_DMA_TCD_TCD0_NBYTES_MLOFFNO = outLen;

    /* Setting CITER, BITER */
    AES__RESULT_DMA_TCD_TCD0_CITER_ELINKNO = 0x0001U;
    AES__RESULT_DMA_TCD_TCD0_BITER_ELINKNO = 0x0001U;

    /* Setting SOFF, DOFF */
    AES__RESULT_DMA_TCD_TCD0_SOFF = 0x0000U;
    AES__RESULT_DMA_TCD_TCD0_DOFF = 0x0001U;

    /* Setting DREQ of Feed DMA , Enablng Major Ch linking with CH 8 */
    AES__RESULT_DMA_TCD_TCD0_CSR |= 0x0828U;

    /* Enabling H/W request */
    AES__RESULT_DMA_TCD_CH0_CSR |= 0x00000001U;

    /**********************************************************************
        Setting up TDC8 for Result DMA (to store status info)
    **********************************************************************/

    /* Clearing Done bit incase in it used before */
    AES__RESULT_DMA_TCD_CH8_CSR |= 0x40000000U;

    /* Enable ID replication */
    AES__RESULT_DMA_TCD_CH8_SBR |= 0x00010000U;

    /* Setting SADDR and DADDR */
    AES__RESULT_DMA_TCD_TCD8_SADDR = AES_ACCEL_STATUS_ADDRESS;
    AES__RESULT_DMA_TCD_TCD8_DADDR = (uint32_t)pAceStatus;

    /* Setting DSIZE, SSIZE */
    AES__RESULT_DMA_TCD_TCD8_ATTR = 0x0000U;

    /* Setting NBYTES */
    AES__RESULT_DMA_TCD_TCD8_NBYTES_MLOFFNO = 0x00000008U;

    /* Setting CITER, BITER */
    AES__RESULT_DMA_TCD_TCD8_CITER_ELINKNO = 0x0001U;
    AES__RESULT_DMA_TCD_TCD8_BITER_ELINKNO = 0x0001U;

    /* Setting SOFF, DOFF */
    AES__RESULT_DMA_TCD_TCD8_SOFF = 0x0000U;
    AES__RESULT_DMA_TCD_TCD8_DOFF = 0x0000U;

    /* Setting DREQ of Result DMA */
    AES__RESULT_DMA_TCD_TCD8_CSR |= 0x0008U;

    /* Soft Start Feed DMA */
    AES__FEED_DMA_TCD_TCD0_CSR |= 0x0001U;

    /* Wait for AES_FEED_DMA and AES_RESULT_DMA to reach the Idle state */
    while((AES__FEED_DMA_MP_CSR & 0x80000000) == 0x80000000);
    while((AES__RESULT_DMA_MP_CSR & 0x80000000) == 0x80000000);
}

/*************************************************************************************************
* Description:  Example of ECB encrypt with ACE+ using DMA request
************************************************************************************************/
static hseSrvResponse_t HSE_AesAccel_ECB_Example(void)
{
    hseKeyHandle_t aesAccelKeyHandle128 = HSE_INVALID_KEY_HANDLE;
    hseKeyHandle_t aesAccelKeyHandle256 = HSE_INVALID_KEY_HANDLE;
    hseSrvResponse_t srvResponse = HSE_SRV_RSP_GENERAL_ERROR;
    uint8_t keySlotIndex = HSE_ACE_KEYSTORE_MAX_SLOTS;

    /*--------- Config KHTT ---------*/

    /* Configure the KHTT */
    srvResponse = HSE_ConfigKHTT();

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /*--------- Import Key Request ---------*/

    /* Import AES 128 bits NVM key for ECB  */
    aesAccelKeyHandle128 = RAM_AES128_MSC_KEY0;

    /* Import the AES key for cipher operations on data (ENC/DEC) */
    srvResponse = HSE_ImportSymKey(aesAccelKeyHandle128, HSE_KEY_TYPE_AES,
        (HSE_KF_USAGE_ENCRYPT|HSE_KF_USAGE_DECRYPT), aes128AceKey, sizeof(aes128AceKey));

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Push the Keys to ACE MSC */
    srvResponse = HSE_PushMscKey(aesAccelKeyHandle128);
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Import AES 256 bits NVM key for ECB */
    aesAccelKeyHandle256 = RAM_AES256_MSC_KEY3;

    /* Import the AES key for cipher operations on data (ENC/DEC) */
    srvResponse = HSE_ImportSymKey(aesAccelKeyHandle256, HSE_KEY_TYPE_AES,
        (HSE_KF_USAGE_ENCRYPT|HSE_KF_USAGE_DECRYPT), aes256AceKey, sizeof(aes256AceKey));

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Push the Keys to ACE MSC */
    srvResponse = HSE_PushMscKey(aesAccelKeyHandle256);
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }


    /*--------- AES_ACCEL ECB encrypt ---------*/

    /* Get the associated MSC key slot index for the key handle */
    srvResponse = GetMscKeySlotIdxFromKhtt(aesAccelKeyHandle128, &keySlotIndex);
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Send DMA request for ECB encrypt with 128 bits key */
    RunAceAesOperationWithDMA(ACE_ENCRYPT_DATA, ACE_MODE_ECB, keySlotIndex,
                              ecbPlainText, sizeof(ecbPlainText), ecbCipherOutput, sizeof(ecbCipherOutput), &aceStatus);


    /* Check the results */
    if((ACE_STATUS_NO_ERROR != aceStatus) ||
       (0 != memcmp(ecbCipherOutput, aes128EcbCipher, sizeof(aes128EcbCipher))))
    {
        srvResponse = HSE_SRV_RSP_GENERAL_ERROR;
        goto exit;
    }

    srvResponse = GetMscKeySlotIdxFromKhtt(aesAccelKeyHandle256, &keySlotIndex);
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Send DMA request for ECB encrypt with 256 bits key */
    RunAceAesOperationWithDMA(ACE_ENCRYPT_DATA, ACE_MODE_ECB, keySlotIndex,
                              ecbPlainText, sizeof(ecbPlainText), ecbCipherOutput, sizeof(ecbCipherOutput), &aceStatus);

    /* Check the results */
    if((ACE_STATUS_NO_ERROR != aceStatus) ||
       (0 != memcmp(ecbCipherOutput, aes256EcbCipher, sizeof(aes256EcbCipher))))
    {
        srvResponse = HSE_SRV_RSP_GENERAL_ERROR;
        goto exit;
    }

exit:
    return srvResponse;
}

/*************************************************************************************************
* Description:  Example of CMAC generate with ACE+ using DMA request
************************************************************************************************/
static hseSrvResponse_t HSE_AesAccel_CMAC_Example(void)
{
    hseKeyHandle_t aesAccelKeyHandle = HSE_INVALID_KEY_HANDLE;
    hseSrvResponse_t srvResponse = HSE_SRV_RSP_GENERAL_ERROR;

    /*--------- Config KHTT ---------*/

    /* Configure the KHTT */
    srvResponse = HSE_ConfigKHTT();

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /*--------- Import Key Request ---------*/

    /* Import AES 128 bits RAM key for CMAC  */
    aesAccelKeyHandle = RAM_AES128_MSC_KEY0;

    /* Import the AES key for cipher operations on data (ENC/DEC) */
    srvResponse = HSE_ImportSymKey(aesAccelKeyHandle, HSE_KEY_TYPE_AES,
        (HSE_KF_USAGE_SIGN|HSE_KF_USAGE_VERIFY), aes128AceKey, sizeof(aes128AceKey));

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Import AES 256 bits RAM key for CMAC */
    aesAccelKeyHandle = RAM_AES256_MSC_KEY3;

    /* Import the AES key for cipher operations on data (ENC/DEC) */
    srvResponse = HSE_ImportSymKey(aesAccelKeyHandle, HSE_KEY_TYPE_AES,
        (HSE_KF_USAGE_SIGN|HSE_KF_USAGE_VERIFY), aes256AceKey, sizeof(aes256AceKey));

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /*--------- AES_ACCEL CMAC generate ---------*/

    /* Send DMA request for CMAC generate with 128 bits key */
    RunAceAesOperationWithDMA(ACE_GENERATE_SIGN, ACE_MODE_CMAC, 0U,
                              cmacPlainText, sizeof(cmacPlainText), cmacTagOutput, sizeof(cmacTagOutput), &aceStatus);

    /* Check the results */
    if((ACE_STATUS_NO_ERROR != aceStatus) ||
       (0 != memcmp(cmacTagOutput, aes128CmacTag, sizeof(aes128CmacTag))))
    {
        srvResponse = HSE_SRV_RSP_GENERAL_ERROR;
        goto exit;
    }

    /* Send DMA request for CMAC generate with 256 bits key */
    RunAceAesOperationWithDMA(ACE_GENERATE_SIGN, ACE_MODE_CMAC, 1U,
                              cmacPlainText, sizeof(cmacPlainText), cmacTagOutput, sizeof(cmacTagOutput), &aceStatus);

    /* Check the results */
    if((ACE_STATUS_NO_ERROR != aceStatus) ||
       (0 != memcmp(cmacTagOutput, aes256CmacTag, sizeof(aes256CmacTag))))
    {
        srvResponse = HSE_SRV_RSP_GENERAL_ERROR;
        goto exit;
    }

exit:
    return srvResponse;
}

/*************************************************************************************************
* Description:  Example of push MSC keys to the same key slot and generate CMAC
************************************************************************************************/
static hseSrvResponse_t HSE_AesAccel_CMAC_With_Push_Key_Example(void)
{
    hseKeyHandle_t aesAccelKeyHandle = HSE_INVALID_KEY_HANDLE;
    hseSrvResponse_t srvResponse = HSE_SRV_RSP_GENERAL_ERROR;

    /*--------- Config KHTT ---------*/

    /* Configure the KHTT */
    srvResponse = HSE_ConfigKHTT();

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /*--------- Import Key Request ---------*/

    /* Import AES 128 bits RAM key for CMAC  */
    aesAccelKeyHandle = RAM_AES128_MSC_KEY1;

    /* Import the AES key for cipher operations on data (ENC/DEC) */
    srvResponse = HSE_ImportSymKey(aesAccelKeyHandle, HSE_KEY_TYPE_AES,
        (HSE_KF_USAGE_SIGN|HSE_KF_USAGE_VERIFY), aes128AceKey, sizeof(aes128AceKey));

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Import AES 256 bits RAM key for CMAC */
    aesAccelKeyHandle = RAM_AES128_MSC_KEY2;

    /* Import the AES key for cipher operations on data (ENC/DEC) */
    srvResponse = HSE_ImportSymKey(aesAccelKeyHandle, HSE_KEY_TYPE_AES,
        (HSE_KF_USAGE_SIGN|HSE_KF_USAGE_VERIFY), aes128AceSecondKey, sizeof(aes128AceSecondKey));

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /*--------- AES_ACCEL CMAC generate ---------*/

    /* Push ACE key */
    srvResponse = HSE_PushMscKey(RAM_AES128_MSC_KEY1);

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Send DMA request for CMAC generate with 128 bits key */
    RunAceAesOperationWithDMA(ACE_GENERATE_SIGN, ACE_MODE_CMAC, 3U,
                              cmacPlainText, sizeof(cmacPlainText), cmacTagOutput, sizeof(cmacTagOutput), &aceStatus);

    /* Check the results */
    if((ACE_STATUS_NO_ERROR != aceStatus) ||
       (0 != memcmp(cmacTagOutput, aes128CmacTag, sizeof(aes128CmacTag))))
    {
        srvResponse = HSE_SRV_RSP_GENERAL_ERROR;
        goto exit;
    }

    /* Push ACE key */
    srvResponse = HSE_PushMscKey(RAM_AES128_MSC_KEY2);

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Send DMA request for CMAC generate with 256 bits key */
    RunAceAesOperationWithDMA(ACE_GENERATE_SIGN, ACE_MODE_CMAC, 3U,
                              cmacPlainText, sizeof(cmacPlainText), cmacTagOutput, sizeof(cmacTagOutput), &aceStatus);

    /* Check the results */
    if((ACE_STATUS_NO_ERROR != aceStatus) ||
       (0 != memcmp(cmacTagOutput, aes128CmacSecondTag, sizeof(aes128CmacSecondTag))))
    {
        srvResponse = HSE_SRV_RSP_GENERAL_ERROR;
        goto exit;
    }

exit:
    return srvResponse;
}

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*************************************************************************************************
* Description:  Example of AES_ACCEL usage.
* Operation: AES ECB Encrypt & AES CMAC Generate.
* Requests type: ImportKey.
************************************************************************************************/
void HSE_AesAccel(void)
{
    hseSrvResponse_t srvResponse = HSE_SRV_RSP_GENERAL_ERROR;

    gAesAccelExample = RUNNING;

   /* Enable Peripheral clk */
    PRTN1_PCONF_EN();
    PRTN1_COFB3_CLK_EN(0);
    PRTN1_COFB3_CLK_EN(4);
    PRTN1_COFB3_CLK_EN(8);
    PRTN1_COFB3_CLK_EN(12);
    PRTN1_PUPD_EN();

    MC_ME_CTL_KEY_EN(0x5AF0U);
    MC_ME_CTL_KEY_EN(0xA50FU);

    /* Enable ACE */
    ACE_CONTROL_ENABLE();

    /* Run the AES_ACCEL ECB example */
    srvResponse = HSE_AesAccel_ECB_Example();

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Run the AES_ACCEL CMAC example */
    srvResponse = HSE_AesAccel_CMAC_Example();

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    /* Run the AES_ACCEL CMAC example with push key */
    srvResponse = HSE_AesAccel_CMAC_With_Push_Key_Example();

    /* Check the response */
    if(HSE_SRV_RSP_OK != srvResponse)
    {
        goto exit;
    }

    testStatus |= ACE_SERVICE_SUCCESS;
    return;

exit:
    ASSERT(FALSE);
}
#endif /* HSE_PLATFORM == HSE_S32K388 */

#ifdef __cplusplus
}
#endif

/** @} */
