#ifndef SOURCE_APP_LED_APP_H_
#define SOURCE_APP_LED_APP_H_
/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include <stdbool.h>
#include "led_api.h"
/**********************************************************************************************************************
 * Exported definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported types
 *********************************************************************************************************************/
typedef struct {
    uint32_t led_number;
} sLedTaskParams_t;

typedef struct {
    uint32_t led_number;
    uint32_t time;
    uint32_t frequency;
} sLedBlinkParams_t;

typedef enum {
    eLedAppTask_First = 0,
    eLedAppTask_Set = eLedAppTask_First,
    eLedAppTask_Reset,
    eLedAppTask_Toggle,
    eLedAppTask_Blink,
    eLedAppTask_Last
} eLedAppTask_t;

typedef struct {
    eLedAppTask_t task;
    void *params;
} sLedAppTaskParams_t;

typedef struct {
    osTimerId_t timer_id;
    osMessageQueueId_t message_queue_id;
    bool busy;
    uint32_t toggle_count;
} sLedTaskData_t;
/**********************************************************************************************************************
 * Exported variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of exported functions
 *********************************************************************************************************************/
bool LED_APP_Init (void);
bool LED_APP_AddTask (eLedApi_t led_number, sLedAppTaskParams_t *led_params);

#endif /* SOURCE_APP_LED_APP_H_ */
