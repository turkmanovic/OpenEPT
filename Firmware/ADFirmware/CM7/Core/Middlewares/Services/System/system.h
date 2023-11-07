/*
 * system.h
 *
 *  Created on: Nov 5, 2023
 *      Author: Haris
 */

#ifndef CORE_MIDDLEWARES_SERVICES_SYSTEM_SYSTEM_H_
#define CORE_MIDDLEWARES_SERVICES_SYSTEM_SYSTEM_H_

#include "globalConfig.h"

#define SYSTEM_TASK_NAME		CONF_SYSTEM_TASK_NAME
#define SYSTEM_TASK_PRIO		CONF_SYSTEM_TASK_PRIO
#define SYSTEM_TASK_STACK_SIZE	CONF_SYSTEM_TASK_SIZE

typedef enum
{
	SYSTEM_STATE_INIT,
	SYSTEM_STATE_SERVICE,
	SYSTEM_STATE_ERROR
}system_state_t;

typedef enum
{
	SYSTEM_STATUS_OK,
	SYSTEM_STATUS_ERROR
}system_status_t;


system_status_t SYSTEM_Init();
system_status_t SYSTEM_Start();

#endif /* CORE_MIDDLEWARES_SERVICES_SYSTEM_SYSTEM_H_ */
