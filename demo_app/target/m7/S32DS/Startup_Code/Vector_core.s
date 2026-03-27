/*==================================================================================================
*   @file    Vector_core.s
*   @version M4_SRC_SW_VERSION_MAJOR.M4_SRC_SW_VERSION_MINOR.M4_SRC_SW_VERSION_PATCH
*
*   @brief   AUTOSAR M4_SRC_MODULE_NAME - Startup file for PowerPC VLE.
*   @details Startup file for PowerPC VLE.
*            This file contains sample code only. It is not part of the production code deliverables
*   @addtogroup BUILD_ENV
*   @{
*/
/*==================================================================================================

==================================================================================================*/

.global VTABLE
.global undefined_handler
.global _Stack_start              /* Top of Stack for Initial Stack Pointer */
.global Reset_Handler             /* Reset Handler */
.global NMI_Handler               /* NMI Handler */
.global HardFault_Handler         /* Hard Fault Handler */
.global MemManage_Handler         /* Reserved */
.global BusFault_Handler          /* Bus Fault Handler */
.global UsageFault_Handler        /* Usage Fault Handler */
.global SVC_Handler               /* SVCall Handler */
.global DebugMon_Handler          /* Debug Monitor Handler */
.global PendSV_Handler            /* PendSV Handler */
.global SysTick_Handler           /* SysTick Handler */ /* 15*/
.global mu0ReceiveInterrupt           /* MU0 RX */ /* 193 */
.global mu1ReceiveInterrupt           /* MU1 RX */ /* 194 */
.global mu0GeneralPurposeInterrupt    /* MU0 GPR */ /* 196*/
.global mu1GeneralPurposeInterrupt    /* MU1 GPR */ /* 197*/
.global stm_irq                        /* STM0 */ /* 55 */

.section  ".intc_vector","ax"
.align 2
.thumb

VTABLE:

.long _Stack_start              /* Top of Stack for Initial Stack Pointer */
.long Reset_Handler             /* Reset Handler */
.long NMI_Handler               /* NMI Handler */
.long HardFault_Handler         /* Hard Fault Handler */
.long MemManage_Handler         /* Reserved */
.long BusFault_Handler          /* Bus Fault Handler */
.long UsageFault_Handler        /* Usage Fault Handler */
.long 0                         /* Reserved */
.long 0                         /* Reserved */
.long 0                         /* Reserved */
.long 0                         /* Reserved */
.long SVC_Handler               /* SVCall Handler */
.long DebugMon_Handler          /* Debug Monitor Handler */
.long 0                         /* Reserved */
.long PendSV_Handler            /* PendSV Handler */
.long SysTick_Handler           /* SysTick Handler */ /* 15*/

.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler         /* 19 */
.long undefined_handler         /* 20 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler          /* 30 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler         /* 40 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler         /* 50 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long stm_irq                      /* 55 */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*64*/
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*80*/
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*96*/
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*112*/
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*112*/
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*128*/
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*144*/
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*160*/
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*192*/
.long mu0ReceiveInterrupt        /* 193 - MU0 receive interrupt */
.long mu0GeneralPurposeInterrupt /* 194 - MU0 general purpose interrupt */
.long undefined_handler
.long mu1ReceiveInterrupt        /* 196 - MU1 receive interrupt */
.long mu1GeneralPurposeInterrupt /* 197 - MU1 general purpose interrupt */
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler

/* External interrupts */
.long undefined_handler /*208*/
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler
.long undefined_handler /*223*/

.size VTABLE, . - VTABLE

