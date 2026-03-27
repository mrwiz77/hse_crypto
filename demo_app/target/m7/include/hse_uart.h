/*==================================================================================================
*   @file    hse_uart.h
*
*   @brief   HSE demo UART interface.
*   @details UART helper APIs for the S32K388 HSE demo application.
*
*   This file contains sample code only. It is not part of the production code deliverables.
==================================================================================================*/

#ifndef HSE_UART_H
#define HSE_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/*==================================================================================================
*                                         INCLUDE FILES
==================================================================================================*/

/*==================================================================================================
*                                     FUNCTION PROTOTYPES
==================================================================================================*/
void HSE_Uart_Init(void);
void HSE_Uart_WriteChar(char c);
void HSE_Uart_WriteString(const char *pString);
char HSE_Uart_GetCharBlocking(void);

#ifdef __cplusplus
}
#endif

#endif /* HSE_UART_H */
