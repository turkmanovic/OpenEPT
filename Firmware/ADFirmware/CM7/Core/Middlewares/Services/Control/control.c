/**
 ******************************************************************************
 * @file   	control.c
 *
 * @brief  	Control service is in charge to receive and process control messages.
 * 			This service communicates with others services defined within this
 * 			firmware. Communications is mostly related to the configuration of
 * 			corresponding service or obtaining status messages from certain service
 * 			defined within control message content.
 * 			All control service logic is implemented within this file
 *
 * @author	Haris Turkmanovic
 * @email	haris.turkmanovic@gmail.com
 * @date	December 2023
 ******************************************************************************
 */
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lwip.h"
#include "lwip/api.h"

#include "control.h"
#include "logging.h"
#include "system.h"
#include "CMParse/cmparse.h"

/**
 * @defgroup CONTROL_PRIVATE_STRUCTURES Control service private structures defines
 * @{
 */
typedef struct
{
	TaskHandle_t		taskHandle;
	SemaphoreHandle_t	initSig;
	char				requestBuffer[CONTROL_BUFFER_SIZE];
	char				responseBuffer[CONTROL_BUFFER_SIZE];
	uint16_t			responseBufferSize;
	control_state_t		state;
}control_data_t;
/**
 * @}
 */
/**
 * @defgroup CONTROL_PRIVATE_DATA Control service private data instances
 */
static control_data_t	prvCONTROL_DATA;
/**
 * @}
 */
/**
 * @defgroup CONTROL_PRIVATE_FUNCTIONS Control service private functions
 * @{
 */
/**
 * @brief	Prepare response in case of error
 * @param	response: buffer where response message will be stored
 * @param	responseSize: response message size
 * @retval	void
 */
static void inline prvCONTROL_PrepareErrorResponse(char* response, uint16_t* responseSize)
{
	uint32_t	tmpIncreaseSize  = 0;
	char* tmpResponsePtr = response;
	tmpIncreaseSize = strlen(CONTROL_RESPONSE_ERROR_STATUS_MSG);
	memcpy(tmpResponsePtr, CONTROL_RESPONSE_ERROR_STATUS_MSG, tmpIncreaseSize);
	tmpResponsePtr	+= tmpIncreaseSize;
	*responseSize	+= tmpIncreaseSize;

	tmpIncreaseSize = strlen(" 1");
	memcpy(tmpResponsePtr, " 1", tmpIncreaseSize);
	tmpResponsePtr	+= tmpIncreaseSize;
	*responseSize	+= tmpIncreaseSize;

	memcpy(tmpResponsePtr, "\r\n", 2);
	tmpResponsePtr	+= 2;
	*responseSize	+= 2;
}
/**
 * @brief	Prepare response in case when request is successfully process
 * @param	response: buffer where response message will be stored
 * @param	responseSize: response message size
 * @param	msg: message that will be integrated between ::CONTROL_RESPONSE_OK_STATUS_MSG and end of the message defined within "\r\n"
 * @param	msgSize: size of the ::msg
 * @param	responseSize: response message size
 * @retval	void
 */
static void inline prvCONTROL_PrepareOkResponse(char* response, uint16_t* responseSize, char* msg, uint32_t msgSize)
{
	uint32_t	tmpIncreaseSize  = 0;
	char* tmpResponsePtr = response;
	tmpIncreaseSize = strlen(CONTROL_RESPONSE_OK_STATUS_MSG);
	memcpy(tmpResponsePtr, CONTROL_RESPONSE_OK_STATUS_MSG, tmpIncreaseSize);
	tmpResponsePtr	+= tmpIncreaseSize;
	*responseSize	+= tmpIncreaseSize;

	memcpy(tmpResponsePtr, " ", 1);
	tmpResponsePtr	+= 1;
	*responseSize	+= 1;

	memcpy(tmpResponsePtr, msg, msgSize);
	tmpResponsePtr	+= msgSize;
	*responseSize	+= msgSize;

	memcpy(tmpResponsePtr, "\r\n", 2);
	tmpResponsePtr	+= 2;
	*responseSize	+= 2;
}
/**
 * @brief	Get device name from system service
 * @param	arguments: arguments defined within control message
 * @param	argumentsLength: arguments message length
 * @param	response: response message content
 * @param	argumentsLength: length of response message
 * @retval	void
 */
static void prvCONTROL_GetDeviceName(const char* arguments, uint16_t argumentsLength, char* response, uint16_t* responseSize)
{
	uint32_t 	deviceNameSize;
	char tmpDeviceName[CONF_SYSTEM_DEFAULT_DEVICE_NAME_MAX];
	memset(tmpDeviceName, 0, CONF_SYSTEM_DEFAULT_DEVICE_NAME_MAX);

	*responseSize = 0;

	if(SYSTEM_GetDeviceName(tmpDeviceName, &deviceNameSize) != SYSTEM_STATUS_OK  )
	{
		prvCONTROL_PrepareErrorResponse(response, responseSize);
		return;
	}

	prvCONTROL_PrepareOkResponse(response, responseSize, tmpDeviceName, deviceNameSize);
	LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO, "Device name successfully obtained\r\n");
}
/**
 * @brief	Set device name by utilazing system service
 * @param	arguments: arguments defined within control message
 * @param	argumentsLength: arguments message length
 * @param	response: response message content
 * @param	argumentsLength: length of response message
 * @retval	void
 */
static void prvCONTROL_SetDeviceName(const char* arguments, uint16_t argumentsLength, char* response, uint16_t* responseSize)
{
	cmparse_value_t	value;

	memset(&value, 0, sizeof(cmparse_value_t));
	if(CMPARSE_GetArgValue(arguments, argumentsLength, "value", &value) != CMPARSE_STATUS_OK)
	{
		prvCONTROL_PrepareErrorResponse(response, responseSize);
		return;
	}

	if(SYSTEM_SetDeviceName(value.value) != SYSTEM_STATUS_OK)
	{
		prvCONTROL_PrepareErrorResponse(response, responseSize);
		return;
	}

	prvCONTROL_PrepareOkResponse(response, responseSize, "", 0);
	LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO, "Device name successfully set\r\n");
}

/**
 * @brief	Main control service task
 * @param	pvParameter: value forwarded during task creation
 * @retval	void
 */
static void prvCONTROL_TaskFunc(void* pvParameter){
	struct netconn *conn, *newconn;
	ip_addr_t 		remoteIpAddr;
	uint16_t		remoteIpPort;
	err_t			err;
	struct netbuf 	*buf;
	void 			*data;
	uint16_t 		dataLen;

	for(;;){
		switch(prvCONTROL_DATA.state)
		{
		case CONTROL_STATE_INIT:

			memset(prvCONTROL_DATA.requestBuffer, 	0, CONTROL_BUFFER_SIZE);
			memset(prvCONTROL_DATA.responseBuffer, 	0, CONTROL_BUFFER_SIZE);
			prvCONTROL_DATA.responseBufferSize = 0;

			LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO, "Control Service started\r\n");
			conn = netconn_new(NETCONN_TCP);
			if(conn == NULL){
				LOGGING_Write("Control Service", LOGGING_MSG_TYPE_ERROR, "There is a problem to create TCP socket\r\n");
				prvCONTROL_DATA.state = CONTROL_STATE_ERROR;
				break;
			}
			LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO, "TCP connection successfully created\r\n");

			err = netconn_bind(conn, IP_ADDR_ANY, CONTROL_SERVER_PORT);
			if(err != ERR_OK){
				LOGGING_Write("Control Service",LOGGING_MSG_TYPE_ERROR,  "There is a problem to bind TCP socket\r\n");
				prvCONTROL_DATA.state = CONTROL_STATE_ERROR;
				break;
			}
			LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO, "TCP Connection successfully bound to port %d\r\n", CONTROL_SERVER_PORT);

			netconn_listen(conn);

			if(xSemaphoreGive(prvCONTROL_DATA.initSig) != pdTRUE)
			{
				LOGGING_Write("Control Service", LOGGING_MSG_TYPE_ERROR,  "There is a problem with init semaphore\r\n");
				prvCONTROL_DATA.state = CONTROL_STATE_ERROR;
				break;
			}

			prvCONTROL_DATA.state = CONTROL_STATE_SERVICE;
			break;
		case CONTROL_STATE_SERVICE:
			LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO, "Wait for control message over TCP on port %d\r\n", CONTROL_SERVER_PORT);
			err = netconn_accept(conn, &newconn);
			if(err != ERR_OK) continue;
			netconn_getaddr(newconn, &remoteIpAddr, &remoteIpPort, 0);
			LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO,  "New connection accepted\r\n");
			while((err = netconn_recv(newconn, &buf)) == ERR_OK)
			{
				do{
					prvCONTROL_DATA.responseBufferSize = 0;
					netbuf_data(buf, &data, &dataLen);
					LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO, "New control message received\r\n");
					memcpy(prvCONTROL_DATA.requestBuffer, data, dataLen);
					if(CMPARSE_Execute(prvCONTROL_DATA.requestBuffer, prvCONTROL_DATA.responseBuffer, &prvCONTROL_DATA.responseBufferSize) != CMPARSE_STATUS_OK)
					{
						LOGGING_Write("Control Service", LOGGING_MSG_TYPE_WARNNING, "There is error during control message parsing procedure\r\n");
						memcpy(prvCONTROL_DATA.responseBuffer, "ERROR 0",strlen("ERROR 0"));
						prvCONTROL_DATA.responseBufferSize = strlen("ERROR 0");
					}
					else
					{
						LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO, "Control message successfully processed\r\n");
					}

				}while(netbuf_next(buf) >= 0);

				err = netconn_write(newconn, prvCONTROL_DATA.responseBuffer, prvCONTROL_DATA.responseBufferSize, NETCONN_COPY);
				if(err != ERR_OK)
				{
					LOGGING_Write("Control Service", LOGGING_MSG_TYPE_WARNNING, "There is a problem to send message\r\n");
				}

				netbuf_delete(buf);

				/* Reinit buffers */
				memset(prvCONTROL_DATA.requestBuffer, 	0, CONTROL_BUFFER_SIZE);
				memset(prvCONTROL_DATA.responseBuffer, 	0, CONTROL_BUFFER_SIZE);
				prvCONTROL_DATA.responseBufferSize = 0;
			}
			netconn_close(newconn);
			netconn_delete(newconn);
			break;
		case CONTROL_STATE_ERROR:
			SYSTEM_ReportError(SYSTEM_ERROR_LEVEL_LOW);
			vTaskDelay(portMAX_DELAY);
			break;
		}

	}
}
/**
 * @}
 */
control_status_t 	CONTROL_Init(uint32_t initTimeout){
	if(xTaskCreate(
			prvCONTROL_TaskFunc,
			CONTROL_TASK_NAME,
			CONTROL_TASK_STACK,
			NULL,
			CONTROL_TASK_PRIO, &prvCONTROL_DATA.taskHandle) != pdPASS) return CONTROL_STATUS_ERROR;

	prvCONTROL_DATA.initSig = xSemaphoreCreateBinary();

	if(prvCONTROL_DATA.initSig == NULL) return CONTROL_STATUS_ERROR;

	prvCONTROL_DATA.state = CONTROL_STATE_INIT;

	if(xSemaphoreTake(prvCONTROL_DATA.initSig, pdMS_TO_TICKS(initTimeout)) != pdTRUE) return CONTROL_STATUS_ERROR;

	/* Add commands */
	CMPARSE_AddCommand("device hello", 		prvCONTROL_GetDeviceName);
	CMPARSE_AddCommand("device setname", 	prvCONTROL_SetDeviceName);

	return CONTROL_STATUS_OK;
}
