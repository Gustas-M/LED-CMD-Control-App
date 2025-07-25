/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "cmsis_os.h"
#include "led_app.h"
#include "cmd_api.h"
#include "debug_api.h"
/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/
#define MESSAGE_QUEUE_SIZE 8
#define MESSAGE_PRIORITY 0
#define ATTR_BITS_DEFAULT 0
#define CB_MEM_DEFAULT NULL
#define CB_SIZE_DEFAULT 0
#define MQ_MEM_DEFAULT NULL
#define MQ_SIZE_DEFAULT 0

#define LED_APP_TASK_STACK_SIZE 512
#define LED_BLINK_TASK_STACK_SIZE 256

#define NO_QUEUE_ARGUMENTS NULL
#define NO_THREAD_ARGUMENTS NULL

#define MESSAGE_QUEUE_WAIT_TIME 50
/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/
MODULE_TAG_CREATION(LED_APP);

const static osMessageQueueAttr_t g_static_led_jobs_queue_attr[eLedApi_Last] = {
    [eLedApi_GpsFix] = {
        .name = "Gps Fix Led Message Queue",
        .attr_bits = ATTR_BITS_DEFAULT,
        .cb_mem = CB_MEM_DEFAULT,
        .cb_size = CB_SIZE_DEFAULT,
        .mq_mem = MQ_MEM_DEFAULT,
        .mq_size = MQ_SIZE_DEFAULT
    },
    [eLedApi_Stat] = {
        .name = "Stat Led Message Queue",
        .attr_bits = ATTR_BITS_DEFAULT,
        .cb_mem = CB_MEM_DEFAULT,
        .cb_size = CB_SIZE_DEFAULT,
        .mq_mem = MQ_MEM_DEFAULT,
        .mq_size = MQ_SIZE_DEFAULT
    }
};

const static osTimerAttr_t g_static_led_app_timer_attr[eLedApi_Last] = {
    [eLedApi_GpsFix] = {
        .name = "GpsFix Led Timer",
        .attr_bits = ATTR_BITS_DEFAULT,
        .cb_mem = CB_MEM_DEFAULT,
        .cb_size = CB_SIZE_DEFAULT
    },
    [eLedApi_Stat] = {
        .name = "Stat Led Timer",
        .attr_bits = ATTR_BITS_DEFAULT,
        .cb_mem = CB_MEM_DEFAULT,
        .cb_size = CB_SIZE_DEFAULT
    }
};

const static osThreadAttr_t g_static_led_app_task_attr = {
    .name = "Led App Task",
    .stack_size = LED_APP_TASK_STACK_SIZE,
    .priority = (osPriority_t)osPriorityNormal
};

const static eLedApi_t g_static_led_enum[eLedApi_Last] = {
    eLedApi_GpsFix,
    eLedApi_Stat
};
/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
static sLedTaskData_t g_static_led_task_data[eLedApi_Last] = {
    [eLedApi_GpsFix] = {
        .timer_id = NULL,
        .message_queue_id = NULL,
        .busy = false,
        .toggle_count = 0
    },
    [eLedApi_Stat] = {
        .timer_id = NULL,
        .message_queue_id = NULL,
        .busy = false,
        .toggle_count = 0
    }
};

static osThreadId_t g_static_led_app_task = NULL;
/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/
static void LED_BlinkCallback (void *argument);
static bool LED_Blink (uint32_t led, uint32_t time, uint32_t frequency);
static void LED_APP_Task (void *arguments);
/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/
static void LED_BlinkCallback (void *argument) {
    eLedApi_t led = *((eLedApi_t*)argument);

    if (g_static_led_task_data[led].toggle_count > 0) {
        LED_API_Toggle(led);
        g_static_led_task_data[led].toggle_count--;
    } else {
        osTimerStop(g_static_led_task_data[led].timer_id);
        LED_API_Reset(led);
        g_static_led_task_data[led].busy = false;
    }
}

static bool LED_Blink (uint32_t led, uint32_t time, uint32_t frequency) {
    if ((led < eLedApi_First) || (led >= eLedApi_Last)) {
        DEBUG_WRN("Such led does not exist");
        return false;
    }

    if ((time <= 0) || (frequency <= 0)) {
        DEBUG_WRN("Invalid timer settings received");
        return false;
    }

    if (g_static_led_task_data[led].timer_id == NULL) {
        DEBUG_WRN("Timer was not initialized");
        return false;
    }

    uint32_t period = 1000 / frequency;
    g_static_led_task_data[(eLedApi_t)led].toggle_count = time * frequency;

    if (osTimerStart(g_static_led_task_data[led].timer_id, period) != osOK) {
        return false;
    }

    return true;
}

static void LED_APP_Task (void *arguments) {
    sLedAppTaskParams_t task_params = {0};
    while (1) {
        for (eLedApi_t led = 0; led < eLedApi_Last; led++) {
            if (g_static_led_task_data[led].busy == true) {
                continue;
            }

            if (osMessageQueueGet(g_static_led_task_data[led].message_queue_id, &task_params, MESSAGE_PRIORITY, MESSAGE_QUEUE_WAIT_TIME) != osOK) {
                continue;
            }

            switch (task_params.task) {
                case eLedAppTask_Set: {
                    sLedTaskParams_t *params = (sLedTaskParams_t*)task_params.params;
                    uint32_t led_number = params->led_number;
                    LED_API_Set(led_number);
                    break;
                }
                case eLedAppTask_Reset: {
                    sLedTaskParams_t *params = (sLedTaskParams_t*)task_params.params;
                    uint32_t led_number = params->led_number;
                    LED_API_Reset(led_number);
                    break;
                }
                case eLedAppTask_Toggle: {
                    sLedTaskParams_t *params = (sLedTaskParams_t*)task_params.params;
                    uint32_t led_number = params->led_number;
                    LED_API_Toggle(led_number);
                }
                case eLedAppTask_Blink: {
                    g_static_led_task_data[led].busy = true;
                    sLedBlinkParams_t *params = (sLedBlinkParams_t*)task_params.params;
                    LED_Blink (params->led_number, params->time, params->frequency);
                    break;
                }
                default: {
                    break;
                }
            }

            HEAP_API_Free(task_params.params);
        }
    }
}
/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
bool LED_APP_Init (void) {
    for (eLedApi_t led = 0; led < eLedApi_Last; led++) {
        if (g_static_led_task_data[led].timer_id == NULL) {
            g_static_led_task_data[led].timer_id = osTimerNew(LED_BlinkCallback, osTimerPeriodic, (void*)&g_static_led_enum[led], &g_static_led_app_timer_attr[led]);
            if (g_static_led_task_data[led].timer_id == NULL) {
                return false;
            }
        }

        if (g_static_led_task_data[led].message_queue_id == NULL) {
            g_static_led_task_data[led].message_queue_id = osMessageQueueNew(MESSAGE_QUEUE_SIZE, sizeof(sLedBlinkParams_t), &g_static_led_jobs_queue_attr[led]);
            if (g_static_led_task_data[led].message_queue_id == NULL) {
                return false;
            }
        }
    }

    if (g_static_led_app_task == NULL) {
        g_static_led_app_task = osThreadNew(LED_APP_Task, NO_THREAD_ARGUMENTS, &g_static_led_app_task_attr);
        if (g_static_led_app_task == NULL) {
            return false;
        }
    }

    return true;
}

bool LED_APP_AddTask (eLedApi_t led_number, sLedAppTaskParams_t *led_params) {
    if (led_params == NULL) {
        return false;
    }

    if ((led_number < eLedApi_First) || (led_number >= eLedApi_Last)) {
        return false;
    }

    if ((led_params->task < eLedAppTask_First) || (led_params->task >= eLedAppTask_Last)) {
        return false;
    }

    if (led_params->params == NULL) {
        return false;
    }

    if (g_static_led_task_data[led_number].message_queue_id == NULL) {
        return false;
    }

    if (osMessageQueuePut(g_static_led_task_data[led_number].message_queue_id, led_params, MESSAGE_PRIORITY, osWaitForever) != osOK) {
        return false;
    }

    return true;
}
