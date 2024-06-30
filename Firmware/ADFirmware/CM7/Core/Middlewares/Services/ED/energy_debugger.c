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

typedef struct energy_debugger_data_t {
	uint8_t button_click_counter;
	energy_debugger_state_t state;
} energy_debugger_data_t;

static energy_debugger_data_t prvENERGY_DEBUGGER_DATA;


static void prvEDEBUGGING_ButtonPressedCallback(uint16_t GPIO_Pin)
{
	DRV_GPIO_ClearInterruptFlag(GPIO_Pin);

    // Increment the button click counter
    prvENERGY_DEBUGGER_DATA.button_click_counter++;

    // Log the button press
    //LOGGING_Write("Energy Debugger", LOGGING_MSG_TYPE_INFO, "Button pressed! Count: %d\r\n", prvENERGY_DEBUGGER_DATA.button_click_counter);

    // Change state if needed
    if (prvENERGY_DEBUGGER_DATA.state == ENERGY_DEBUGGER_STATE_INIT)
    {
        prvENERGY_DEBUGGER_DATA.state = ENERGY_DEBUGGER_STATE_SERVICE;
    }
}

static void prvENERGY_DEBUGGER_Task()
{
		LOGGING_Write("Energy Debugger", LOGGING_MSG_TYPE_INFO, "Energy Debugger service started\r\n");
		for(;;)
		{
			switch(prvENERGY_DEBUGGER_DATA.state)
			{
			case ENERGY_DEBUGGER_STATE_INIT:
			    // Initialize the GPIO driver
			    if (DRV_GPIO_Init() != DRV_GPIO_STATUS_OK)
			    	prvENERGY_DEBUGGER_DATA.state = ENERGY_DEBUGGER_STATE_ERROR;

			    // Initialize the GPIO port
			    if (DRV_GPIO_Port_Init(DRV_GPIO_PORT_C) != DRV_GPIO_STATUS_OK)
			    	prvENERGY_DEBUGGER_DATA.state = ENERGY_DEBUGGER_STATE_ERROR;

			    // Configure the pin for the button
			    drv_gpio_pin_init_conf_t button_pin_conf;
			    button_pin_conf.mode = DRV_GPIO_PIN_MODE_IT_RISING;
			    button_pin_conf.pullState = DRV_GPIO_PIN_PULL_NOPULL;
			    //Definisati pin preko makroa
			    if (DRV_GPIO_Pin_Init(DRV_GPIO_PORT_C, ENERGY_DEBUGGER_BUTTON_PIN, &button_pin_conf) != DRV_GPIO_STATUS_OK)
			    {
			    	prvENERGY_DEBUGGER_DATA.state = ENERGY_DEBUGGER_STATE_ERROR;
			    	break;
			    }

			    // Register the button press callback
			    if (DRV_GPIO_RegisterCallback(DRV_GPIO_PORT_C, ENERGY_DEBUGGER_BUTTON_PIN, prvEDEBUGGING_ButtonPressedCallback, ENERGY_DEBUGGER_BUTTON_ISR_PRIO) != DRV_GPIO_STATUS_OK)
			    {
			    	prvENERGY_DEBUGGER_DATA.state = ENERGY_DEBUGGER_STATE_ERROR;
			    	break;
			    }
			    prvENERGY_DEBUGGER_DATA.state = ENERGY_DEBUGGER_STATE_SERVICE;
				break;
			case ENERGY_DEBUGGER_STATE_SERVICE:
				if (prvENERGY_DEBUGGER_DATA.button_click_counter > 0)
				{
					prvENERGY_DEBUGGER_DATA.button_click_counter = 0;
					DRV_AIN_Stream_SetCapture();
				}
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
    // Create the task
    if(xTaskCreate(prvENERGY_DEBUGGER_Task,
            ENERGY_DEBUGGER_TASK_NAME,
            ENERGY_DEBUGGER_STACK_SIZE,
            NULL,
            ENERGY_DEBUGGER_TASK_PRIO,
            &prvENERGY_DEBUGGER_TASK_HANDLE) != pdTRUE) return ENERGY_DEBUGGER_STATUS_ERROR;

    return ENERGY_DEBUGGER_STATUS_OK;
}

