/*
 * control.c
 *
 *  Created on: Dec 28, 2023
 *      Author: Haris
 */
#include <string.h>
#include "globalConfig.h"
#include "control.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lwip.h"
#include "lwip/api.h"
#include "../Logging/logging.h"
#include "CMParse/cmparse.h"

typedef struct
{
	TaskHandle_t		taskHandle;
	SemaphoreHandle_t	initSig;
	char				requestBuffer[CONTROL_BUFFER_SIZE];
	char				responseBuffer[CONTROL_BUFFER_SIZE];
	uint16_t			responseBufferSize;
	control_state_t		state;
}control_data_t;

static control_data_t	prvCONTROL_DATA;

control_status_t prvCONTROL_GetDeviceName(const char* arguments, uint16_t argumentsLength, char* response, uint16_t* responseSize)
{
	strcpy(response, "OK Acq Device\r\n");
	*responseSize = strlen("OK Acq Device\r\n");
	return CONTROL_STATUS_OK;
}


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

			memset(prvCONTROL_DATA.requestBuffer, 0, CONTROL_BUFFER_SIZE);
			memset(prvCONTROL_DATA.responseBuffer, 0, CONTROL_BUFFER_SIZE);
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
			while((err = netconn_recv(newconn, &buf)) == ERR_OK){
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
				if(err != ERR_OK){
					LOGGING_Write("Control Service", LOGGING_MSG_TYPE_WARNNING, "There is a problem to send message\r\n");
				}
				netbuf_delete(buf);
			}
			LOGGING_Write("Control Service", LOGGING_MSG_TYPE_INFO,  "Connection closed\r\n");
			netconn_close(newconn);
			netconn_delete(newconn);
			break;
		case CONTROL_STATE_ERROR:
			break;
		}

	}
}

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

	CMPARSE_AddCommand("device hello", prvCONTROL_GetDeviceName);

	return CONTROL_STATUS_OK;
}
