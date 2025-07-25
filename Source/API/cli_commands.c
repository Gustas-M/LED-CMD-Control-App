/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <ctype.h>
#include "cli_commands.h"
#include "cmd_api.h"
#include "debug_api.h"
#include "gpio_driver.h"
#include "led_api.h"
#include "led_app.h"
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define PARAMS_SEPERATOR ","

#define MIN_TIME 0
#define MAX_TIME 59
#define MIN_FREQUENCY 1
#define MAX_FREQUENCY 100
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
 MODULE_TAG_CREATION(CLI_APP);
/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
 
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
bool CLI_CMD_Set (sCommandParams_t *params) {
    if (params == NULL) {
        DEBUG_ERR("No parameters received");
        return false;
    }

    if ((params->params == NULL) || (params->params_length <= 0)) {
        DEBUG_ERR("Empty parameters");
        return false;
    }

    if (isdigit(params->params[0]) == 0) {
        DEBUG_ERR("Parameters are not numeric");
        return false;
    }

    int led_number = atoi((char*)params->params);
    if ((led_number < eLedApi_First) || (led_number >= eLedApi_Last)) {
        DEBUG_ERR("Such led does not exist");
        return false;
    }

    sLedTaskParams_t *led_params = (sLedTaskParams_t*)HEAP_API_Calloc(1, sizeof(sLedTaskParams_t));
    if (led_params == NULL) {
        DEBUG_ERR("Failed to allocate memory");
        return false;
    }

    led_params->led_number = led_number;
    sLedAppTaskParams_t task_params = {.task = eLedAppTask_Set, .params = led_params};

    if (LED_APP_AddTask(led_number, &task_params) == false) {
        DEBUG_ERR("Failed to add led task");
        HEAP_API_Free(led_params);
        return false;
    }

    DEBUG_INF("Set function has started");
    return true;
}

bool CLI_CMD_Reset (sCommandParams_t *params) {
    if (params == NULL) {
        DEBUG_ERR("No parameters received");
        return false;
    }

    if ((params->params == NULL) || (params->params_length <= 0)) {
        DEBUG_ERR("Empty parameters");
        return false;
    }

    if (isdigit(params->params[0]) == 0) {
        DEBUG_ERR("Parameters are not numeric");
        return false;
    }

    int led_number = atoi((char*)params->params);
    if ((led_number < eLedApi_First) || (led_number >= eLedApi_Last)) {
        DEBUG_ERR("Such led does not exist");
        return false;
    }

    sLedTaskParams_t *led_params = (sLedTaskParams_t*)HEAP_API_Calloc(1, sizeof(sLedTaskParams_t));
    if (led_params == NULL) {
        DEBUG_ERR("Failed to allocate memory");
        return false;
    }

    led_params->led_number = led_number;
    sLedAppTaskParams_t task_params = {.task = eLedAppTask_Reset, .params = led_params};

    if (LED_APP_AddTask(led_number, &task_params) == false) {
        DEBUG_ERR("Failed to add led task");
        HEAP_API_Free(led_params);
        return false;
    }
    DEBUG_INF("Reset function has started");
    return true;
}

bool CLI_CMD_Toggle (sCommandParams_t *params) {
    if (params == NULL) {
        DEBUG_ERR("No parameters received");
        return false;
    }

    if ((params->params == NULL) || (params->params_length <= 0)) {
        DEBUG_ERR("Empty parameters");
        return false;
    }

    if (isdigit(params->params[0]) == 0) {
        DEBUG_ERR("Parameters are not numeric");
        return false;
    }

    int led_number = atoi((char*)params->params);
    if ((led_number < eLedApi_First) || (led_number >= eLedApi_Last)) {
        DEBUG_ERR("Such led does not exist");
        return false;
    }

    sLedTaskParams_t *led_params = (sLedTaskParams_t*)HEAP_API_Calloc(1, sizeof(sLedTaskParams_t));
    if (led_params == NULL) {
        DEBUG_ERR("Failed to allocate memory");
        return false;
    }

    led_params->led_number = led_number;
    sLedAppTaskParams_t task_params = {.task = eLedAppTask_Toggle, .params = led_params};

    if (LED_APP_AddTask(led_number, &task_params) == false) {
        DEBUG_ERR("Failed to add led task");
        HEAP_API_Free(led_params);
        return false;
    }

    DEBUG_INF("Toggle function has started");
    return true;
}

bool CLI_CMD_Blink (sCommandParams_t *params) {
    if (params == NULL) {
        DEBUG_ERR("No parameters received");
        return false;
    }

    if ((params->params == NULL) || (params->params_length <= 0)) {
        DEBUG_ERR("Empty parameters");
        return false;
    }

    char *token = NULL;
    token = strtok((char*)params->params, PARAMS_SEPERATOR);
    if (token == NULL) {
        DEBUG_ERR("Failed to tokenize string");
        return false;
    }

    if (isdigit((int)token[0]) == 0) {
        DEBUG_ERR("Parameters are not numeric");
        return false;
    }

    int led_number = atoi((char*)token);
    if ((led_number < eLedApi_First) || (led_number >= eLedApi_Last)) {
        DEBUG_ERR("Such led does not exist");
        return false;
    }

    token = strtok(NULL, PARAMS_SEPERATOR);
    if (isdigit((int)token[0]) == 0) {
        DEBUG_ERR("Parameters are not numeric");
        return false;
    }

    int time = atoi((char*)token);
    if ((time < MIN_TIME) || (time > MAX_TIME)) {
        DEBUG_WRN("Invalid time");
        return false;
    }

    token = strtok(NULL, PARAMS_SEPERATOR);
    if (isdigit((int)token[0]) == 0) {
        DEBUG_ERR("Parameters are not numeric");
        return false;
    }

    int frequency = atoi((char*)token);
    if ((frequency < MIN_FREQUENCY) || (frequency > MAX_FREQUENCY)) {
        DEBUG_WRN("Invalid frequency");
        return false;
    }

    sLedBlinkParams_t *led_params = (sLedBlinkParams_t*)HEAP_API_Calloc(1, sizeof(sLedBlinkParams_t));
    if (led_params == NULL) {
        DEBUG_ERR("Failed to allocate memory");
        return false;
    }

    led_params->led_number = led_number;
    led_params->time = time;
    led_params->frequency = frequency;

    sLedAppTaskParams_t task_params = {.task = eLedAppTask_Blink, .params = led_params};
    if (LED_APP_AddTask(led_number, &task_params) == false) {
        DEBUG_ERR("Failed to add led task");
        HEAP_API_Free(&led_params);
        return false;
    }

    DEBUG_INF("Blink function has started");
    return true;
}
