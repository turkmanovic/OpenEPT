/*
 * globalConfig.h
 *
 *  Created on: Nov 5, 2023
 *      Author: Haris
 */

#ifndef CORE_CONFIGURATION_GLOBALCONFIG_H_
#define CORE_CONFIGURATION_GLOBALCONFIG_H_


/*Driver layer connfiguration*/
/* AnalogIN configuration*/
#define CONF_AIN_MAX_BUFFER_SIZE	1500


/* GPIO configuration*/
#define CONF_GPIO_PORT_MAX_NUMBER	10
#define CONF_GPIO_PIN_MAX_NUMBER	16



/*Middleware layer configuration*/


#define CONF_SSTREAM_TASK_NAME			"Sample stream"
#define CONF_SSTREAM_TASK_PRIO			4
#define CONF_SSTREAM_TASK_STACK_SIZE	1024
#define CONF_SSTREAM_UDP_PORT			5100



#endif /* CORE_CONFIGURATION_GLOBALCONFIG_H_ */
