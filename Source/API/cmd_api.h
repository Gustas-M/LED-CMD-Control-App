#ifndef SOURCE_API_CMD_API_H_
#define SOURCE_API_CMD_API_H_
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "message.h"
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef struct {
    uint8_t *params;
    size_t params_length;
} sCommandParams_t;

typedef bool (*CommandFunction_t)(sCommandParams_t *params);

typedef struct {
    uint8_t *command_name;
    size_t command_name_size;
    CommandFunction_t function;
} sCommandDesc_t;

typedef struct {
    sCommandDesc_t *command_table;
    size_t command_table_size;
    uint8_t *reply;
    size_t reply_size;
} sCmdLauncherParams_t;
/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool CLI_CMD_Process (sMessage_t *received_message, sCmdLauncherParams_t *cmd_params);

#endif /* SOURCE_API_CMD_API_H_ */
