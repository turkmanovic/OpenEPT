/*
 * dpcontrol.h
 *
 *  Created on: Nov 5, 2023
 *      Author: Haris
 */
#include "globalConfig.h"

#define DPCONTROL_TASK_NAME				CONFIG_LOGGING_TASK_NAME
#define DPCONTROL_TASK_PRIO				CONFIG_LOGGING_PRIO
#define DPCONTROL_TASK_STACK			CONFIG_LOGGING_STACK_SIZE


typedef enum{
	DPCONTROL_STATUS_OK = 0,
	DPCONTROL_STATUS_ERROR
}dpcontrol_status_t;

typedef enum
{
	DPCONTROL_ACTIVE_STATUS_DISABLE = 0,
	DPCONTROL_ACTIVE_STATUS_ENABLE
}dpcontrol_active_status_t;

typedef enum
{
	DPCONTROL_STATE_INIT,
	DPCONTROL_STATE_SERVICE,
	DPCONTROL_STATE_ERROR
}dpcontrol_state_t;

dpcontrol_status_t 	DPCONTROL_Init(uint32_t initTimeout);
dpcontrol_status_t 	DPCONTROL_SetValue(uint32_t value, uint32_t timeout);
dpcontrol_status_t 	DPCONTROL_SetActivestatus(dpcontrol_active_status_t activeStatus, uint32_t timeout);
