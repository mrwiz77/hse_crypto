/**
 *   @file    global_defs.h
 *
 *   @brief   Contains the global defined macros.
 *
 *   @addtogroup security_installer
 *   @{
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

#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "hse_global_types.h"
#include "hse_interface.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                                          CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define HSE_IF_RESP_OK_ERR_GOTO_EXIT(response)    \
        if(HSE_SRV_RSP_OK != response)            \
     {                                            \
          goto Exit;                              \
     }

#define ASSERT(condition) \
    do                    \
    {                     \
        if (!(condition)) \
            while (1)     \
                ;         \
    } while (0)

#define ASSERT_RETRY(condition) \
    while (!(condition))        \
        ;

#ifdef DEBUG
#define DEBUG_LOOP(var) while (!(var))
#else
#define DEBUG_LOOP(var)
#endif

#define ALIGN(value, alignment) (((value) + (alignment)-1) & (~((alignment)-1)))
#define ADDR_NOT_NULL(addr) ((0x0UL != (uintptr_t)(addr)) && (0xFFFFFFFFUL != (uintptr_t)(addr)))

#define MU0 (0U)
#define MU1 (1U)
#define MU2 (2U)
#define MU3 (3U)

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                    STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                    GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

#if (defined(HSE_S32K388) && (HSE_S32K388 == HSE_PLATFORM))
    /* Keys for Managed Security Component */
    #define RAM_AES128_MSC_KEY0                 GET_KEY_HANDLE(HSE_KEY_CATALOG_ID_RAM, 1, 0)
    #define RAM_AES128_MSC_KEY1                 GET_KEY_HANDLE(HSE_KEY_CATALOG_ID_RAM, 1, 1)
    #define RAM_AES128_MSC_KEY2                 GET_KEY_HANDLE(HSE_KEY_CATALOG_ID_RAM, 1, 2)
    #define RAM_AES256_MSC_KEY3                 GET_KEY_HANDLE(HSE_KEY_CATALOG_ID_RAM, 2, 0)

    /** @brief    HSE Key Handle Translation Table (KHTT) */
    #define HSE_KHTT_CFG  \
        /* Key Handle           MSC Key Slot    MSC Instance    ACE DID Flags    Restriction Flags          Reserved  */    \
        { RAM_AES128_MSC_KEY0,      0U,             0U,           0xFFFFU,              0U,                  { 0U }   },    \
        { RAM_AES256_MSC_KEY3,      1U,             0U,           0xFFFFU,              0U,                  { 0U }   },    \
        { RAM_AES128_MSC_KEY1,      3U,             0U,           0xFFFFU,              0U,                  { 0U }   },    \
        { RAM_AES128_MSC_KEY2,      3U,             0U,           0xFFFFU,     HSE_KHTT_RESTRICT_PUSH_MANY,  { 0U }   }

#endif

#ifdef __cplusplus
}
#endif

#endif /* GLOBAL_DEFS_H */

/** @} */
