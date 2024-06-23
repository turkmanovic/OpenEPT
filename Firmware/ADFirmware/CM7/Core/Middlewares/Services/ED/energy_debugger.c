/*
 * energy_debugger.c
 *
 *  Created on: Jun 23, 2024
 *      Author: Pavle Lakic & Dimitrije Lilic
 */

#include "energy_debugger.h"

#include "FreeRTOS.h"
#include "task.h"

#include "drv_ain.h"
#include "drv_gpio.h"

#include "system.h"
#include "logging.h"

static 	TaskHandle_t 			prvENERGY_DEBUGGER_TASK_HANDLE;

typedef struct energy_debugger_data {
	uint8_t button_click_counter;
	energy_debugger_state_t state;
} energy_debugger_data;

static energy_debugger_data prvENERGY_DEBUGGER_DATA;

static void prvENERGY_DEBUGGER_Task()
{
		LOGGING_Write("Energy Debugger", LOGGING_MSG_TYPE_INFO, "Energy Debugger service started\r\n");
		for(;;)
		{
			switch(prvENERGY_DEBUGGER_DATA.state)
			{
			case ENERGY_DEBUGGER_STATE_INIT:

				prvENERGY_DEBUGGER_DATA.state = ENERGY_DEBUGGER_STATE_SERVICE;
				break;
			case ENERGY_DEBUGGER_STATE_SERVICE:

				break;
			case ENERGY_DEBUGGER_STATE_ERROR:
				SYSTEM_ReportError(SYSTEM_ERROR_LEVEL_LOW);
				vTaskDelay(portMAX_DELAY);
				break;
			}

		}
}

energy_debugger_status_t ENERGY_DEBUGGER_Init(uint32_t timeout)
{
	if(xTaskCreate(prvENERGY_DEBUGGER_Task,
			ENERGY_DEBUGGER_TASK_NAME,
			ENERGY_DEBUGGER_STACK_SIZE,
			NULL,
			ENERGY_DEBUGGER_TASK_PRIO,
			&prvENERGY_DEBUGGER_TASK_HANDLE) != pdTRUE) return ENERGY_DEBUGGER_STATUS_ERROR;

	return ENERGY_DEBUGGER_STATUS_OK;
}
