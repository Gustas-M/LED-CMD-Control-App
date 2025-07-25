#ifndef SOURCE_API_DEBUG_API_H_
#define SOURCE_API_DEBUG_API_H_
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "uart_api.h"
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/
#define DEBUG_INF(format, ...) DEBUG_API_Print(module_tag, eDebugApiType_Info, NULL, 0, eUartApiPort_Usart1, format, ##__VA_ARGS__)
#define DEBUG_WRN(format, ...) DEBUG_API_Print(module_tag, eDebugApiType_Warning, NULL, 0, eUartApiPort_Usart1, format, ##__VA_ARGS__)
#define DEBUG_ERR(format, ...) DEBUG_API_Print(module_tag, eDebugApiType_Error, __FILE__, __LINE__, eUartApiPort_Usart1, format, ##__VA_ARGS__)
#define MODULE_TAG_CREATION(tag) const static char *module_tag = #tag
/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef enum {
    eDebugApiType_First = 0,
    eDebugApiType_Info = eDebugApiType_First,
    eDebugApiType_Warning,
    eDebugApiType_Error,
    eDebugApiType_Last
} eDebugApiType_t;


/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool DEBUG_API_Init (void);
bool DEBUG_API_Print (const char *module_tag, eDebugApiType_t type, char *file, int line, eUartApiPort_t port, const char *format, ...);

#endif /* SOURCE_API_DEBUG_API_H_ */
