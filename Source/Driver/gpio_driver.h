#ifndef SOURCE_DRIVER_GPIO_DRIVER_H_
#define SOURCE_DRIVER_GPIO_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdbool.h>
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
typedef enum {
    eGpioPin_First = 0,
    eGpioPin_LedStat = eGpioPin_First,
    eGpioPin_LedGpsFix,
    eGpioPin_DebugTx,
    eGpioPin_DebugRx,
    eGpioPin_ModemTx,
    eGpioPin_ModemRx,
    eGpioPin_ModemUartDtr,
    eGpioPin_ModemUartRts,
    eGpioPin_ModemUartCts,
    eGpioPin_ModemOn,
    eGpioPin_GnssOn,
    eGpioPin_ModemPowerOff,
    eGpioPin_Last
} eGpioPin_t;
/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool GPIO_Driver_Init (void);
bool GPIO_Driver_TogglePin (eGpioPin_t pin);
bool GPIO_Driver_ReadPin (eGpioPin_t pin, bool *state);
bool GPIO_Driver_WritePin (eGpioPin_t pin, bool state);

#ifdef __cplusplus
}
#endif

#endif /* SOURCE_DRIVER_GPIO_DRIVER_H_ */
