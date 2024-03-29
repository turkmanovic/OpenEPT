/*
 * system.c
 *
 *  Created on: Nov 5, 2023
 *      Author: Haris
 */

#ifndef CORE_MIDDLEWARES_SERVICES_SYSTEM_SYSTEM_C_
#define CORE_MIDDLEWARES_SERVICES_SYSTEM_SYSTEM_C_


#include <stdint.h>
#include <string.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#include "lwip.h"

#include "drv_system.h"
#include "drv_gpio.h"


#include "system.h"
#include "logging.h"
#include "network.h"
#include "control.h"
#include "sstream.h"



typedef  struct
{
	system_state_t 		state;
	SemaphoreHandle_t 	initSig;
	char				deviceName[CONF_SYSTEM_DEFAULT_DEVICE_NAME_MAX];
}system_data_t;

static system_data_t prvSYSTEM_DATA;


static TaskHandle_t  prvSYSTEM_TASK_HANDLE;



static void	prvBUTTON_Callback(drv_gpio_pin pin)
{

}

static void prvSYSTEM_Task()
{
	drv_gpio_pin_init_conf_t userLedConf;

	for(;;)
	{
		switch(prvSYSTEM_DATA.state)
		{
		case SYSTEM_STATE_INIT:
			userLedConf.mode = DRV_GPIO_PIN_MODE_OUTPUT_PP;
			userLedConf.pullState = DRV_GPIO_PIN_PULL_NOPULL;
			uint32_t	state = 0x01;

			if(DRV_SYSTEM_InitDrivers() != DRV_SYSTEM_STATUS_OK)
			{
				prvSYSTEM_DATA.state = SYSTEM_STATE_ERROR;
				break;
			}
			DRV_GPIO_Port_Init(SYSTEM_LINK_STATUS_DIODE_PORT);
			DRV_GPIO_Port_Init(SYSTEM_ERROR_STATUS_DIODE_PORT);
			DRV_GPIO_Pin_Init(SYSTEM_LINK_STATUS_DIODE_PORT, SYSTEM_LINK_STATUS_DIODE_PIN, &userLedConf);
			DRV_GPIO_Pin_Init(SYSTEM_ERROR_STATUS_DIODE_PORT, SYSTEM_ERROR_STATUS_DIODE_PIN, &userLedConf);
			DRV_GPIO_Pin_EnableInt(DRV_GPIO_PORT_C, 13, 5, prvBUTTON_Callback);

			memset(prvSYSTEM_DATA.deviceName, 0, CONF_SYSTEM_DEFAULT_DEVICE_NAME_MAX);
			memcpy(prvSYSTEM_DATA.deviceName, CONF_SYSTEM_DEFAULT_DEVICE_NAME, strlen(CONF_SYSTEM_DEFAULT_DEVICE_NAME));

			if(LOGGING_Init(2000) != LOGGING_STATUS_OK)
			{
				prvSYSTEM_DATA.state = SYSTEM_STATE_ERROR;
				break;
			}
			LOGGING_Write("System", LOGGING_MSG_TYPE_INFO, "Logging service successfully initialized\r\n");
			if(NETWORK_Init(2000) != NETWORK_STATUS_OK)
			{
				prvSYSTEM_DATA.state = SYSTEM_STATE_ERROR;
				break;
			}
			LOGGING_Write("System", LOGGING_MSG_TYPE_INFO, "Network service successfully initialized\r\n");
			if(CONTROL_Init(2000) != CONTROL_STATUS_OK)
			{
				prvSYSTEM_DATA.state = SYSTEM_STATE_ERROR;
				break;
			}
			LOGGING_Write("System", LOGGING_MSG_TYPE_INFO, "Control service successfully initialized\r\n");

			if(SSTREAM_Init() != SSTREAM_STATUS_OK)
			{
				prvSYSTEM_DATA.state = SYSTEM_STATE_ERROR;
				break;
			}
			xSemaphoreGive(prvSYSTEM_DATA.initSig);
			prvSYSTEM_DATA.state = SYSTEM_STATE_SERVICE;
			break;
		case SYSTEM_STATE_SERVICE:
			/*Main application logic goes here*/
			vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
			break;
		case SYSTEM_STATE_ERROR:
			SYSTEM_ReportError(SYSTEM_ERROR_LEVEL_LOW);
			vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
			break;
		}

	}
}


system_status_t SYSTEM_Init()
{

	if(xTaskCreate(prvSYSTEM_Task,
			SYSTEM_TASK_NAME,
			SYSTEM_TASK_STACK_SIZE,
			NULL,
			SYSTEM_TASK_PRIO,
			&prvSYSTEM_TASK_HANDLE) != pdTRUE) return SYSTEM_STATUS_ERROR;

	prvSYSTEM_DATA.initSig = xSemaphoreCreateBinary();

	if(prvSYSTEM_DATA.initSig == NULL) return SYSTEM_STATUS_ERROR;

	prvSYSTEM_DATA.state = SYSTEM_STATE_INIT;

	return SYSTEM_STATUS_OK;
}


system_status_t SYSTEM_Start()
{
	if(osKernelInitialize() != osOK) return SYSTEM_STATUS_ERROR;
	if(osKernelStart() != osOK) return SYSTEM_STATUS_ERROR;
	/*Never ends here*/
	return SYSTEM_STATUS_ERROR;
}

system_status_t SYSTEM_ReportError(system_error_level_t errorLevel)
{
	switch(errorLevel)
	{
	case SYSTEM_ERROR_LEVEL_LOW:
		if(DRV_GPIO_Pin_SetState(SYSTEM_ERROR_STATUS_DIODE_PORT, SYSTEM_ERROR_STATUS_DIODE_PIN, DRV_GPIO_PIN_STATE_SET) != DRV_GPIO_STATUS_OK) return SYSTEM_STATUS_ERROR;
		break;
	case SYSTEM_ERROR_LEVEL_MEDIUM:
		if(DRV_GPIO_Pin_SetState(SYSTEM_ERROR_STATUS_DIODE_PORT, SYSTEM_ERROR_STATUS_DIODE_PIN, DRV_GPIO_PIN_STATE_SET) != DRV_GPIO_STATUS_OK) return SYSTEM_STATUS_ERROR;
		break;
	case SYSTEM_ERROR_LEVEL_HIGH:
		if(DRV_GPIO_Pin_SetState(SYSTEM_ERROR_STATUS_DIODE_PORT, SYSTEM_ERROR_STATUS_DIODE_PIN, DRV_GPIO_PIN_STATE_SET) != DRV_GPIO_STATUS_OK) return SYSTEM_STATUS_ERROR;
		break;
	}
	return SYSTEM_STATUS_OK;
}

system_status_t SYSTEM_SetLinkStatus(system_link_status_t linkStatus)
{
	if(linkStatus == SYSTEM_LINK_STATUS_UP)
	{
		if(DRV_GPIO_Pin_SetState(SYSTEM_LINK_STATUS_DIODE_PORT, SYSTEM_LINK_STATUS_DIODE_PIN, DRV_GPIO_PIN_STATE_SET) != DRV_GPIO_STATUS_OK) return SYSTEM_STATUS_ERROR;
	}
	else
	{
		if(DRV_GPIO_Pin_SetState(SYSTEM_LINK_STATUS_DIODE_PORT, SYSTEM_LINK_STATUS_DIODE_PIN, DRV_GPIO_PIN_STATE_RESET) != DRV_GPIO_STATUS_OK) return SYSTEM_STATUS_ERROR;
	}
	return SYSTEM_STATUS_OK;
}

system_status_t SYSTEM_SetDeviceName(const char* deviceName)
{
	if(strlen(deviceName) > CONF_SYSTEM_DEFAULT_DEVICE_NAME_MAX) return SYSTEM_STATE_ERROR;
	memset(prvSYSTEM_DATA.deviceName, 0, CONF_SYSTEM_DEFAULT_DEVICE_NAME_MAX);
	memcpy(prvSYSTEM_DATA.deviceName, deviceName, strlen(deviceName));
	return SYSTEM_STATUS_OK;
}

system_status_t SYSTEM_GetDeviceName(char* deviceName, uint32_t* deviceNameSize)
{
	memcpy(deviceName, prvSYSTEM_DATA.deviceName, strlen(prvSYSTEM_DATA.deviceName));
	*deviceNameSize = strlen(prvSYSTEM_DATA.deviceName);
	return SYSTEM_STATUS_OK;
}

#endif /* CORE_MIDDLEWARES_SERVICES_SYSTEM_SYSTEM_C_ */
