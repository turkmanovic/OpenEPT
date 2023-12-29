/**
 ******************************************************************************
 * @file   	control.h
 * @brief  	...
 * @author	Haris Turkmanovic
 * @email	haris.turkmanovic@gmail.com
 * @date	December 2023
 ******************************************************************************
 */
#include <stdint.h>
#include "globalConfig.h"

#ifndef CORE_MIDDLEWARES_SERVICES_CONTROL_CONTROL_H_
#define CORE_MIDDLEWARES_SERVICES_CONTROL_CONTROL_H_

#define CONTROL_BUFFER_SIZE					CONFIG_CONTROL_BUFFER_SIZE
#define CONTROL_SERVER_PORT					CONFIG_CONTROL_SERVER_PORT

#define CONTROL_TASK_NAME					CONFIG_CONTROL_TASK_NAME
#define CONTROL_TASK_PRIO					CONFIG_CONTROL_PRIO
#define CONTROL_TASK_STACK					CONFIG_CONTROL_STACK_SIZE

#define CONTROL_RESPONSE_OK_STATUS_MSG		CONF_CONTROL_RESPONSE_OK_STATUS_MSG
#define CONTROL_RESPONSE_ERROR_STATUS_MSG	CONF_CONTROL_RESPONSE_ERROR_STATUS_MSG

typedef enum
{
	CONTROL_STATE_INIT,
	CONTROL_STATE_SERVICE,
	CONTROL_STATE_ERROR
}control_state_t;

typedef enum{
	CONTROL_STATUS_OK,
	CONTROL_STATUS_ERROR
}control_status_t;

control_status_t 	CONTROL_Init(uint32_t initTimeout);

#endif /* CORE_MIDDLEWARES_SERVICES_CONTROL_CONTROL_H_ */
