/*
 * energy_debugger.h
 *
 *  Created on: Jun 23, 2024
 *      Author: Pavle Lakic & Dimitrije Lilic
 */

#ifndef CORE_MIDDLEWARES_SERVICES_ED_ENERGY_DEBUGGER_H_
#define CORE_MIDDLEWARES_SERVICES_ED_ENERGY_DEBUGGER_H_

#include <stdint.h>

#include "globalConfig.h"

#define ENERGY_DEBUGGER_TASK_NAME			CONF_ENERGY_DEBUGGER_TASK_NAME
#define ENERGY_DEBUGGER_TASK_PRIO			CONF_ENERGY_DEBUGGER_TASK_PRIO
#define ENERGY_DEBUGGER_STACK_SIZE			CONF_ENERGY_DEBUGGER_STACK_SIZE

//prvo ovde staviti pa se referencirati iz config fajla

typedef enum
{
	ENERGY_DEBUGGER_STATE_INIT,
	ENERGY_DEBUGGER_STATE_SERVICE,
	ENERGY_DEBUGGER_STATE_ERROR
}energy_debugger_state_t;

typedef enum
{
	ENERGY_DEBUGGER_STATUS_OK,
	ENERGY_DEBUGGER_STATUS_ERROR
}energy_debugger_status_t;

energy_debugger_status_t ENERGY_DEBUGGER_Init(uint32_t timeout);
void ButtonPressedCallback(uint16_t GPIO_Pin);

#endif /* CORE_MIDDLEWARES_SERVICES_ED_ENERGY_DEBUGGER_H_ */
