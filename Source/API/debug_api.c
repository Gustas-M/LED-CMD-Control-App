/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "cmsis_os.h"
#include "debug_api.h"
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define DELIMITER "\r\n"
#define DELIMITER_LENGTH 2

#define CB_MEM_DEFAULT NULL
#define CB_SIZE_DEFAULT 0

#define DEBUG_BUFFER_SIZE 128

#define INFO_HEADER "INF"
#define WARNING_HEADER "WRN"
#define ERROR_HEADER "ERR"
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
const static osMutexAttr_t g_static_debug_mutex_attr = {
    .name = "Debug Api Mutex",
    .attr_bits = osMutexPrioInherit,
    .cb_mem = CB_MEM_DEFAULT,
    .cb_size = CB_SIZE_DEFAULT
};
/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static char g_static_debug_buffer[DEBUG_BUFFER_SIZE];

static osMutexId_t debug_mutex = NULL;
/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
bool DEBUG_API_Init (void) {
    if (debug_mutex != NULL) {
        return false;
    }

    debug_mutex = osMutexNew(&g_static_debug_mutex_attr);
    if (debug_mutex == NULL) {
        return false;
    }

    return true;
}

bool DEBUG_API_Print (const char *module_tag, eDebugApiType_t type, char *file, int line, eUartApiPort_t port, const char *format, ...) {
    if ((port < eUartApiPort_First) || (port >= eUartApiPort_Last)) {
        return false;
    }

    if ((type < eDebugApiType_First) || (type >= eDebugApiType_Last)) {
        return false;
    }

    if (module_tag == NULL) {
        return false;
    }

    if ((type == eDebugApiType_Error) && ((file == NULL) || (line < 0))) {
        return false;
    }

    if (osMutexAcquire(debug_mutex, osWaitForever) != osOK) {
        return false;
    }

    size_t index = 0;
    memset(g_static_debug_buffer, 0, DEBUG_BUFFER_SIZE);

    switch (type) {
        case eDebugApiType_Info: {
            index += snprintf(g_static_debug_buffer, DEBUG_BUFFER_SIZE, "[%s.%s] ", module_tag, INFO_HEADER);
            break;
        }
        case eDebugApiType_Warning: {
            index += snprintf(g_static_debug_buffer, DEBUG_BUFFER_SIZE, "[%s.%s] ", module_tag, WARNING_HEADER);
            break;
        }
        case eDebugApiType_Error: {
            index += snprintf(g_static_debug_buffer, DEBUG_BUFFER_SIZE, "[%s.%s] (%s @line:%d) ", module_tag, ERROR_HEADER, file, line);
            break;
        }
        default: {
            return false;
        }
    }

    va_list args;
    va_start(args, format);
    int written_symbols = vsnprintf(g_static_debug_buffer + index, DEBUG_BUFFER_SIZE - index, format, args);
    va_end(args);

    if (written_symbols < DEBUG_BUFFER_SIZE) {
        index += written_symbols;

        strcat(g_static_debug_buffer, DELIMITER);
        index += DELIMITER_LENGTH;
    } else {
        index += DEBUG_BUFFER_SIZE - index;
    }

    sMessage_t message = {.message = (uint8_t*)g_static_debug_buffer, .message_length = index};
    if (UART_API_Send(port, &message, osWaitForever) == false) {
        osMutexRelease(debug_mutex);
        return false;
    }

    if (osMutexRelease(debug_mutex) != osOK) {
        return false;
    }

    return true;
}
