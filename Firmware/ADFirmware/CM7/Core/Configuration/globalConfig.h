/*
 * globalConfig.h
 *
 *  Created on: Nov 5, 2023
 *      Author: Haris
 */

#include "drv_gpio.h"

#ifndef CORE_CONFIGURATION_GLOBALCONFIG_H_
#define CORE_CONFIGURATION_GLOBALCONFIG_H_


/*Driver layer connfiguration*/
/* AnalogIN configuration*/
#define CONF_AIN_MAX_BUFFER_SIZE	1500


/* GPIO configuration*/
#define CONF_GPIO_PORT_MAX_NUMBER			10
#define CONF_GPIO_PIN_MAX_NUMBER			16
#define	CONF_GPIO_INTERRUPTS_MAX_NUMBER		15



/*Middleware layer configuration*/
/**/
#define CONF_SSTREAM_TASK_NAME					"Sample stream"
#define CONF_SSTREAM_TASK_PRIO					4
#define CONF_SSTREAM_TASK_STACK_SIZE			1024
#define CONF_SSTREAM_UDP_PORT					5100

/**/
#define CONF_SYSTEM_TASK_NAME					"System task"
#define CONF_SYSTEM_TASK_PRIO					5
#define CONF_SYSTEM_TASK_STACK_SIZE				1024

#define	CONF_SYSTEM_ERROR_STATUS_DIODE_PORT		1	//Port B
#define	CONF_SYSTEM_ERROR_STATUS_DIODE_PIN		14

#define	CONF_SYSTEM_LINK_STATUS_DIODE_PORT		4	//Port E
#define	CONF_SYSTEM_LINK_STATUS_DIODE_PIN		1

/**/
#define CONF_NETWORK_TASK_NAME					"Network Task"
#define CONF_NETWORK_TASK_PRIO					3
#define CONF_NETWORK_TASK_STACK_SIZE			1024

#define CONF_NETWORK_DEVICE_IP_ADDRESS			"192.168.2.102"
#define CONF_NETWORK_DEVICE_IP_MASK				"255.255.255.0"
#define CONF_NETWORK_DEVICE_IP_GW				"192.168.2.1"



#endif /* CORE_CONFIGURATION_GLOBALCONFIG_H_ */
