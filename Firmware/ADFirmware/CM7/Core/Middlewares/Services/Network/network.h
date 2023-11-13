/*
 * network.h
 *
 *  Created on: Nov 5, 2023
 *      Author: Haris
 */

#ifndef CORE_MIDDLEWARES_SERVICES_NETWORK_NETWORK_H_
#define CORE_MIDDLEWARES_SERVICES_NETWORK_NETWORK_H_

#include <stdint.h>

#include "globalConfig.h"

#define NETWORK_TASK_NAME					CONF_NETWORK_TASK_NAME
#define NETWORK_TASK_PRIO					CONF_NETWORK_TASK_PRIO
#define NETWORK_TASK_STACK_SIZE				CONF_NETWORK_TASK_STACK_SIZE

#define NETWORK_DEVICE_IP_ADDRESS			CONF_NETWORK_DEVICE_IP_ADDRESS
#define NETWORK_DEVICE_IP_MASK				CONF_NETWORK_DEVICE_IP_MASK
#define NETWORK_DEVICE_IP_GW				CONF_NETWORK_DEVICE_IP_GW

typedef enum
{
	NETWORK_STATE_INIT,
	NETWORK_STATE_SERVICE,
	NETWORK_STATE_ERROR
}network_state_t;


typedef enum
{
	NETWORK_STATUS_OK,
	NETWORK_STATUS_ERROR
}network_status_t;

network_status_t NETWORK_Init(uint32_t timeout);


#endif /* CORE_MIDDLEWARES_SERVICES_NETWORK_NETWORK_H_ */