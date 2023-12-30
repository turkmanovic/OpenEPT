/**
 ******************************************************************************
 * @file   	control.h
 *
 * @brief  	Control service is in charge to receive and process control messages.
 * 			This service communicates with others services defined within this
 * 			firmware. Communications is mostly related to the configuration of
 * 			corresponding service or obtaining status messages from certain service
 * 			defined within control message content.
 * 			All control service interface functions, as well as public defines and
 * 			data types,are defined within this header file
 *
 * @author	Haris Turkmanovic
 * @email	haris.turkmanovic@gmail.com
 * @date	December 2023
 ******************************************************************************
 */
#include <stdint.h>
#include "globalConfig.h"

#ifndef CORE_MIDDLEWARES_SERVICES_CONTROL_CONTROL_H_
#define CORE_MIDDLEWARES_SERVICES_CONTROL_CONTROL_H_
/**
 * @defgroup CONTROL_PUBLIC_DATA Control service public data and defines
 * @{
 */
/**
 * @defgroup CONTROL_PUBLIC_DEFINES Control service public defines
 * @{
 */
#define CONTROL_BUFFER_SIZE					CONFIG_CONTROL_BUFFER_SIZE				/*!< Request and response buffer sizes */
#define CONTROL_SERVER_PORT					CONFIG_CONTROL_SERVER_PORT				/*!< Control service TCP PORT number value */

#define CONTROL_TASK_NAME					CONFIG_CONTROL_TASK_NAME				/*!< Control service task name */
#define CONTROL_TASK_PRIO					CONFIG_CONTROL_PRIO						/*!< Control service task prio */
#define CONTROL_TASK_STACK					CONFIG_CONTROL_STACK_SIZE				/*!< Control service task stack size */

#define CONTROL_RESPONSE_OK_STATUS_MSG		CONF_CONTROL_RESPONSE_OK_STATUS_MSG		/*!< Default OK response message */
#define CONTROL_RESPONSE_ERROR_STATUS_MSG	CONF_CONTROL_RESPONSE_ERROR_STATUS_MSG	/*!< Default ERROR response message */
/**
 * @}
 */
/**
 * @defgroup CONTROL_PUBLIC_STRUCTURES Control service public structures
 * @{
 */
/**
 * @brief Control service task state
 */
typedef enum
{
	CONTROL_STATE_INIT,			/*!< Control service initialization state */
	CONTROL_STATE_SERVICE,		/*!< Control service is in service state */
	CONTROL_STATE_ERROR			/*!< Control service is in error state */
}control_state_t;
/**
 * @brief Control service return status
 */
typedef enum{
	CONTROL_STATUS_OK,			/*!< Control service status OK */
	CONTROL_STATUS_ERROR		/*!< Control service status Error */
}control_status_t;
/**
 * @}
 */
/**
 * @}
 */
/**
 * @defgroup CONTROL_PUBLIC_FUNCTIONS Control service interface functions
 * @{
 */
/**
 * @brief	Initialize control service
 * @param	initTimeout: timeout initialization
 * @retval	::control_status_t
 */
control_status_t 	CONTROL_Init(uint32_t initTimeout);
/**
 * @}
 */
#endif /* CORE_MIDDLEWARES_SERVICES_CONTROL_CONTROL_H_ */
