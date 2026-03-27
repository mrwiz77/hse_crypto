/**
 *   @file    hse_global_defs.h
 *
 *   @brief   HSE global definitions.
 *   @details Define the HSE internal macros used globally
 *
 *   @addtogroup hse_global_defs.h
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
 *  ==============================================================================================*/

#ifndef HSE_GLOBAL_DEFS_H
#define HSE_GLOBAL_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
*  ==============================================================================================*/

#include "std_typedefs.h"

/*==================================================================================================
*                                          CONSTANTS
*  ==============================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
*  ==============================================================================================*/

#ifndef PASS
#define PASS (0x01U)
#endif

#ifndef FAIL
#define FAIL (0x00U)
#endif

#ifndef ENABLED
#define ENABLED (0x1U)
#endif

#ifndef DISABLED
#define DISABLED (0x0U)
#endif

#ifndef NO_ERROR_PRESENT
#define NO_ERROR_PRESENT (0x0U)
#endif

#ifndef ERROR_PRESENT
#define ERROR_PRESENT (0x1U)
#endif

#ifndef BIT_HIGH
#define BIT_HIGH (0x1U)
#endif

#ifndef BIT_LOW
#define BIT_LOW (0x0U)
#endif

/*==================================================================================================
*                                             ENUMS
*  ==============================================================================================*/

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
*  ==============================================================================================*/

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
*  ==============================================================================================*/

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
*  ==============================================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* HSE_GLOBAL_DEFS_H */

/** @} */
