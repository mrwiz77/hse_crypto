/*==================================================================================================
*   @file    hse_uart.c
*
*   @brief   HSE demo UART source.
*   @details UART helper APIs for the S32K388 HSE demo application.
*
*   This file contains sample code only. It is not part of the production code deliverables.
==================================================================================================*/

/*==================================================================================================
*                                         INCLUDE FILES
==================================================================================================*/
#include "hse_uart.h"
#include "std_typedefs.h"
#include "S32K344.h"

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
#define HSE_UART_CLOCK_KEY_1            (0x5AF0U)
#define HSE_UART_CLOCK_KEY_2            (0xA50FU)
#define HSE_UART_INSTANCE               (LPUART_6)
#define HSE_UART_MC_ME                  (MC_ME)
#define HSE_UART_SIUL2                  (SIUL2)
#define HSE_UART_BAUD_OSR               (16U)
#define HSE_UART_BAUD_SBR               (26U)
#define HSE_UART_TX_MSCR_INDEX          (16U)
#define HSE_UART_RX_MSCR_INDEX          (15U)
#define HSE_UART_RX_IMCR_INDEX          (193U)
#define HSE_UART_TX_MUX_VALUE           (5U)
#define HSE_UART_RX_INPUT_MUX_VALUE     (2U)

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void HSE_Uart_EnableClock(void);
static void HSE_Uart_ConfigPins(void);
static void HSE_Uart_ConfigController(void);

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
void HSE_Uart_Init(void)
{
    HSE_Uart_EnableClock();
    HSE_Uart_ConfigPins();
    HSE_Uart_ConfigController();
}

void HSE_Uart_WriteChar(char c)
{
    while (0U == HSE_UART_INSTANCE.STAT.B.TDRE)
    {
    }

    HSE_UART_INSTANCE.DATA.R = (uint32_t)(uint8_t)c;
}

void HSE_Uart_WriteString(const char *pString)
{
    if (NULL == pString)
    {
        return;
    }

    while ('\0' != *pString)
    {
        HSE_Uart_WriteChar(*pString);
        pString++;
    }
}

char HSE_Uart_GetCharBlocking(void)
{
    while (0U == HSE_UART_INSTANCE.STAT.B.RDRF)
    {
    }

    return (char)(uint8_t)(HSE_UART_INSTANCE.DATA.R & 0xFFU);
}

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/
static void HSE_Uart_EnableClock(void)
{
    HSE_UART_MC_ME.PRTN1_COFB2_CLKEN.B.REQ80 = 1U;
    HSE_UART_MC_ME.PRTN1_PCONF.B.PCE = 1U;
    HSE_UART_MC_ME.PRTN1_PUPD.B.PCUD = 1U;

    HSE_UART_MC_ME.CTL_KEY.B.KEY = HSE_UART_CLOCK_KEY_1;
    HSE_UART_MC_ME.CTL_KEY.B.KEY = HSE_UART_CLOCK_KEY_2;

    while (0U == HSE_UART_MC_ME.PRTN1_COFB2_STAT.B.BLOCK80)
    {
    }
}

static void HSE_Uart_ConfigPins(void)
{
    HSE_UART_SIUL2.MSCR[HSE_UART_RX_MSCR_INDEX].R = 0U;
    HSE_UART_SIUL2.MSCR[HSE_UART_RX_MSCR_INDEX].B.IBE = 1U;
    HSE_UART_SIUL2.IMCR[HSE_UART_RX_IMCR_INDEX].B.SSS = HSE_UART_RX_INPUT_MUX_VALUE;

    HSE_UART_SIUL2.MSCR[HSE_UART_TX_MSCR_INDEX].R = HSE_UART_TX_MUX_VALUE;
    HSE_UART_SIUL2.MSCR[HSE_UART_TX_MSCR_INDEX].B.OBE = 1U;
}

static void HSE_Uart_ConfigController(void)
{
    HSE_UART_INSTANCE.GLOBAL.B.RST = 1U;
    HSE_UART_INSTANCE.GLOBAL.B.RST = 0U;

    HSE_UART_INSTANCE.CTRL.R = 0U;
    HSE_UART_INSTANCE.BAUD.R = 0U;
    HSE_UART_INSTANCE.FIFO.R = 0U;
    HSE_UART_INSTANCE.WATER.R = 0U;
    HSE_UART_INSTANCE.MODIR.R = 0U;

    HSE_UART_INSTANCE.STAT.R = 0xC01FC000UL;

    HSE_UART_INSTANCE.BAUD.B.SBR = HSE_UART_BAUD_SBR;
    HSE_UART_INSTANCE.BAUD.B.OSR = (HSE_UART_BAUD_OSR - 1U);
    HSE_UART_INSTANCE.BAUD.B.SBNS = 0U;

    HSE_UART_INSTANCE.CTRL.B.M = 0U;
    HSE_UART_INSTANCE.CTRL.B.M7 = 0U;
    HSE_UART_INSTANCE.CTRL.B.PE = 0U;
    HSE_UART_INSTANCE.CTRL.B.RE = 1U;
    HSE_UART_INSTANCE.CTRL.B.TE = 1U;
}
