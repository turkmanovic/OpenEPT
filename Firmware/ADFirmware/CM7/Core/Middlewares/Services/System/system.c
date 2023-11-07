/*
 * system.c
 *
 *  Created on: Nov 5, 2023
 *      Author: Haris
 */

#ifndef CORE_MIDDLEWARES_SERVICES_SYSTEM_SYSTEM_C_
#define CORE_MIDDLEWARES_SERVICES_SYSTEM_SYSTEM_C_


#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#include "lwip.h"

#include "system.h"
#include "drv_gpio.h"


#include <stdint.h>

typedef  struct
{
	system_state_t state;
	SemaphoreHandle_t initSig;
}system_data_t;

static system_data_t prvSYSTEM_DATA;


static TaskHandle_t prvSYSTEM_TASK_HANDLE;



static void	prvBUTTON_Callback(drv_gpio_pin pin)
{

}

static void prvSYSTEM_Task()
{
	drv_gpio_pin_init_conf_t userLedConf;

	userLedConf.mode = DRV_GPIO_PIN_MODE_OUTPUT_PP;
	userLedConf.pullState = DRV_GPIO_PIN_PULL_NOPULL;
	uint32_t	state = 0x01;

	/* init code for LWIP */
	MX_LWIP_Init();

	DRV_GPIO_Init();
	DRV_GPIO_Port_Init(DRV_GPIO_PORT_E);
	DRV_GPIO_Port_Init(DRV_GPIO_PORT_C);
	DRV_GPIO_Pin_Init(DRV_GPIO_PORT_E, 1, &userLedConf);
	DRV_GPIO_Pin_EnableInt(DRV_GPIO_PORT_C, 13, 5, prvBUTTON_Callback);
	for(;;)
	{
		switch(prvSYSTEM_DATA.state)
		{
		case SYSTEM_STATE_INIT:

			xSemaphoreGive(prvSYSTEM_DATA.initSig);
			prvSYSTEM_DATA.state = SYSTEM_STATE_SERVICE;
			break;
		case SYSTEM_STATE_SERVICE:
			state ^= 0x01;
			DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_E, 1, state);
			osDelay(1000);
			break;
		case SYSTEM_STATE_ERROR:
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


#endif /* CORE_MIDDLEWARES_SERVICES_SYSTEM_SYSTEM_C_ */
