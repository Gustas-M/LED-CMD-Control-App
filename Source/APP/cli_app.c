/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "cmsis_os.h"
#include "cli_app.h"
#include "cli_commands.h"
#include "cmd_api.h"
#include "message.h"
#include "uart_api.h"
#include "debug_api.h"
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define CLI_COMMAND_SIZE 64
#define CLI_REPLY_BUFFER_SIZE 32
#define DELIMITER_LENGTH 2

#define DEBUG_USART_PORT eUartApiPort_Usart1

MODULE_TAG_CREATION(CLI_APP);

#define DEFINE_CMD(cmd, cmd_function) { \
    .command_name = (uint8_t*)#cmd, \
    .command_name_size = sizeof(#cmd) - 1, \
    .function = &cmd_function \
} \
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
static uint8_t g_static_reply_buffer[CLI_REPLY_BUFFER_SIZE] = {0};

const static osThreadAttr_t g_static_cli_app_task_attr = {
    .name = "cliAppTask",
    .stack_size = 512,
    .priority = (osPriority_t)osPriorityNormal
};

const static sCommandDesc_t g_static_command_lut[eCliCmd_Last] = {
    DEFINE_CMD(set:, CLI_CMD_Set),
    DEFINE_CMD(reset:, CLI_CMD_Reset),
    DEFINE_CMD(toggle:, CLI_CMD_Toggle),
    DEFINE_CMD(blink:, CLI_CMD_Blink),
};

static sCmdLauncherParams_t g_static_cmd_params = {
    .command_table = (sCommandDesc_t*)g_static_command_lut,
    .command_table_size = eCliCmd_Last,
    .reply = g_static_reply_buffer,
    .reply_size = CLI_REPLY_BUFFER_SIZE
};
/**********************************************************************************************************************
 * Private variables
*********************************************************************************************************************/
static osThreadId_t g_static_cli_app_task_handle;
/**********************************************************************************************************************
* Exported variables and references
*********************************************************************************************************************/

/**********************************************************************************************************************
* Prototypes of private functions
*********************************************************************************************************************/
static void RTOS_CliAppTask (void *argument);
/**********************************************************************************************************************
* Definitions of private functions
*********************************************************************************************************************/

static void RTOS_CliAppTask (void *argument) {
    sMessage_t message = {0};
    while (1) {
        if (UART_API_Receive(DEBUG_USART_PORT, &message, osWaitForever) == true) {
            if (CLI_CMD_Process(&message, &g_static_cmd_params) == false) {
                sMessage_t reply_message = {.message = g_static_cmd_params.reply, .message_length = strnlen((char*)g_static_cmd_params.reply, g_static_cmd_params.reply_size)};
                UART_API_Send(eUartApiPort_Usart1, &reply_message, osWaitForever);
            }
        }

        HEAP_API_Free(message.message);
    }
}

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
bool CLI_APP_Init (void) {
    if (g_static_cli_app_task_handle != NULL) {
        return false;
    }

    g_static_cli_app_task_handle = osThreadNew(RTOS_CliAppTask, NULL, &g_static_cli_app_task_attr);
    if (g_static_cli_app_task_handle == NULL) {
        return false;
    }

    return true;
}
