/**
 ******************************************************************************
 * @file   	sstream.c
 *
 * @brief
 *
 * @author	Haris Turkmanovic
 * @email	haris.turkmanovic@gmail.com
 * @date	Februar 2024
 ******************************************************************************
 */
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "logging.h"
#include "system.h"

#include "sstream.h"

#include "drv_ain.h"


#define  	SSTREAM_TASK_START_BIT						0x00000001
#define  	SSTREAM_TASK_STOP_BIT						0x00000002
#define  	SSTREAM_TASK_STREAM_BIT						0x00000004
#define  	SSTREAM_TASK_SET_ADC_RESOLUTION_BIT			0x00000008
#define  	SSTREAM_TASK_SET_ADC_STIME_BIT				0x00000010
#define  	SSTREAM_TASK_SET_ADC_CLOCK_DIV_BIT			0x00000020
#define  	SSTREAM_TASK_SET_ADC_CH1_STIME_BIT			0x00000040
#define  	SSTREAM_TASK_SET_ADC_CH2_STIME_BIT			0x00000080
#define  	SSTREAM_TASK_SET_ADC_CH1_OFFSET_BIT			0x00000100
#define  	SSTREAM_TASK_SET_ADC_CH2_OFFSET_BIT			0x00000200
#define  	SSTREAM_TASK_SET_ADC_CH1_AVERAGING_RATIO	0x00000400
#define  	SSTREAM_TASK_SET_ADC_CH2_AVERAGING_RATIO	0x00000800



typedef struct
{
	TaskHandle_t				taskHandle;
	SemaphoreHandle_t			initSig;
	SemaphoreHandle_t			guard;
	sstream_state_t				state;
	sstream_connection_info		connectionInfo;
	sstream_acquisition_state_t	acquisitionState;
	drv_ain_adc_config_t		ainConfig;
}channel_data_t;

typedef struct
{
	channel_data_t				connections[CONF_SSTREAM_CONNECTIONS_MAX_NO];
	uint32_t					activeConnectionsNo;
}sstream_data_t;


static sstream_data_t								prvSSTREAM_DATA;

static void prvSSTREAM_TaskFunc(void* pvParam)
{
	uint32_t				notifyValue = 0;
	TickType_t				blockingTime = portMAX_DELAY;
	channel_data_t			*connectionData;
	connectionData 			= (channel_data_t*)pvParam;
	LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Samples stream service created\r\n");
	for(;;)
	{
		switch(connectionData->state)
		{
		case SSTREAM_STATE_INIT:
			/* Try to configure default resolution */
			if(DRV_AIN_SetResolution(DRV_AIN_ADC_3, SSTREAM_AIN_DEFAULT_RESOLUTION) == DRV_AIN_STATUS_OK)
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "%d bit resolution set\r\n", SSTREAM_AIN_DEFAULT_RESOLUTION);
				connectionData->ainConfig.resolution = SSTREAM_AIN_DEFAULT_RESOLUTION;
			}
			else
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem with AIN resolution setting\r\n");
				connectionData->ainConfig.resolution = DRV_AIN_ADC_RESOLUTION_UKNOWN;
			}

			/* Try to configure default clok div */
			if(DRV_AIN_SetClockDiv(DRV_AIN_ADC_3, SSTREAM_AIN_DEFAULT_CLOCK_DIV) == DRV_AIN_STATUS_OK)
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Clock div resolution set\r\n", SSTREAM_AIN_DEFAULT_CLOCK_DIV);
				connectionData->ainConfig.clockDiv = SSTREAM_AIN_DEFAULT_CLOCK_DIV;
			}
			else
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem with AIN clock div setting\r\n");
				connectionData->ainConfig.resolution = DRV_AIN_ADC_CLOCK_DIV_UKNOWN;
			}

			/* Try to configure channels default sampling time */
			if(DRV_AIN_SetChannelsSamplingTime(DRV_AIN_ADC_3, SSTREAM_AIN_DEFAULT_CH_SAMPLE_TIME) == DRV_AIN_STATUS_OK)
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Channels sampling time %d set\r\n", SSTREAM_AIN_DEFAULT_CH_SAMPLE_TIME);
				connectionData->ainConfig.ch1.sampleTime = SSTREAM_AIN_DEFAULT_CH_SAMPLE_TIME;
				connectionData->ainConfig.ch2.sampleTime = SSTREAM_AIN_DEFAULT_CH_SAMPLE_TIME;
			}
			else
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem with AIN channels sampling time setting\r\n");
				connectionData->ainConfig.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_UKNOWN;
				connectionData->ainConfig.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_UKNOWN;
			}

			/* Try to configure default sampling time */
			if(DRV_AIN_SetChannelAvgRatio(DRV_AIN_ADC_3, 1, SSTREAM_AIN_DEFAULT_CH_AVG_RATIO) == DRV_AIN_STATUS_OK)
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Channel 1 averaging ration %d set\r\n", SSTREAM_AIN_DEFAULT_CH_AVG_RATIO);
				connectionData->ainConfig.ch1.avgRatio = SSTREAM_AIN_DEFAULT_CH_AVG_RATIO;
			}
			else
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to set default channel 1 averaging ration\r\n");
				connectionData->ainConfig.ch1.avgRatio = 0;
			}

			/* Try to configure default sampling time */
			if(DRV_AIN_SetChannelAvgRatio(DRV_AIN_ADC_3, 2, SSTREAM_AIN_DEFAULT_CH_AVG_RATIO) == DRV_AIN_STATUS_OK)
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Channel 2 averaging ration %d set\r\n", SSTREAM_AIN_DEFAULT_CH_AVG_RATIO);
				connectionData->ainConfig.ch1.avgRatio = SSTREAM_AIN_DEFAULT_CH_AVG_RATIO;
			}
			else
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to set default channel 1 averaging ration\r\n");
				connectionData->ainConfig.ch1.avgRatio = 0;
			}

			/*Obtain ADC input clk*/
			if(DRV_AIN_GetADCClk(DRV_AIN_ADC_3, &connectionData->ainConfig.inputClk) == DRV_AIN_STATUS_OK)
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "ADC Input clk %d\r\n", connectionData->ainConfig.inputClk);
			}
			else
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Unable to obtain ADC clock\r\n");
			}

			if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
			{
				LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem with init semaphore\r\n");
				connectionData->state = SSTREAM_STATE_ERROR;
				break;
			}
			connectionData->state = SSTREAM_STATE_SERVICE;
			break;
		case SSTREAM_STATE_SERVICE:
			notifyValue = ulTaskNotifyTake(pdTRUE, blockingTime);
			if(notifyValue & SSTREAM_TASK_START_BIT)
			{
				connectionData->acquisitionState = SSTREAM_ACQUISITION_STATE_START;
				if(DRV_AIN_Start(DRV_AIN_ADC_3) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Stream started \r\n");
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Unable to start stream\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_STREAM_BIT)
			{
				connectionData->acquisitionState = SSTREAM_ACQUISITION_STATE_STREAM;
			}
			if(notifyValue & SSTREAM_TASK_STOP_BIT)
			{
				connectionData->acquisitionState = SSTREAM_ACQUISITION_STATE_STOP;
				if(DRV_AIN_Stop(DRV_AIN_ADC_3) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Stream stoped \r\n");
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Unable to stop stream\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_SET_ADC_RESOLUTION_BIT)
			{
				/* Try to configure default resolution */
				if(DRV_AIN_SetResolution(DRV_AIN_ADC_3, connectionData->ainConfig.resolution) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "%d bit resolution set\r\n", connectionData->ainConfig.resolution);
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem with AIN resolution setting\r\n");
				}

				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem with ADC configuration\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_SET_ADC_CLOCK_DIV_BIT)
			{
				/* Try to configure ADC clock div */
				if(DRV_AIN_SetClockDiv(DRV_AIN_ADC_3, connectionData->ainConfig.clockDiv) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Clok div %d set\r\n", connectionData->ainConfig.clockDiv);
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem with AIN clock div\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_SET_ADC_STIME_BIT)
			{
				/* Try to configure ADC clock div */
				if(DRV_AIN_SetSamplingResolutionTime(DRV_AIN_ADC_3, connectionData->ainConfig.samplingTime) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Sampling time %d set\r\n", connectionData->ainConfig.samplingTime);
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to set sampling time\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_SET_ADC_CH1_STIME_BIT)
			{
				/* Try to configure ADC channel 0 sampling time */
				if(DRV_AIN_SetChannelsSamplingTime(DRV_AIN_ADC_3, connectionData->ainConfig.ch1.sampleTime) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Sampling time %d set on channel 0\r\n", connectionData->ainConfig.ch1.sampleTime);
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to set channel sampling time\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_SET_ADC_CH2_STIME_BIT)
			{
				/* Try to configure ADC channel 1 sampling time */
				if(DRV_AIN_SetChannelsSamplingTime(DRV_AIN_ADC_3, connectionData->ainConfig.ch2.sampleTime) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Sampling time %d set on channel 2\r\n", connectionData->ainConfig.ch2.sampleTime);
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to set channel sampling time\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_SET_ADC_CH1_OFFSET_BIT)
			{
				/* Try to configure ADC channel 1 sampling time */
				if(DRV_AIN_SetChannelOffset(DRV_AIN_ADC_3, 1, connectionData->ainConfig.ch1.offset) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Channel 1 offset %d successfully set\r\n", connectionData->ainConfig.ch1.offset);
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "Unable to set channel 1 offset\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_SET_ADC_CH2_OFFSET_BIT)
			{
				/* Try to configure ADC channel 1 sampling time */
				if(DRV_AIN_SetChannelOffset(DRV_AIN_ADC_3, 2, connectionData->ainConfig.ch2.offset) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Channel 2 offset %d successfully set\r\n", connectionData->ainConfig.ch2.offset);
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "Unable to set channel 1 offset\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_SET_ADC_CH1_AVERAGING_RATIO)
			{
				/* Try to configure ADC channel 1 sampling time */
				if(DRV_AIN_SetChannelAvgRatio(DRV_AIN_ADC_3, 1, connectionData->ainConfig.ch1.avgRatio) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Channel 1 averaging ration %d successfully set\r\n", connectionData->ainConfig.ch1.avgRatio);
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "Unable to set channel 1 averaging ratio\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			if(notifyValue & SSTREAM_TASK_SET_ADC_CH2_AVERAGING_RATIO)
			{
				/* Try to configure ADC channel 1 sampling time */
				if(DRV_AIN_SetChannelAvgRatio(DRV_AIN_ADC_3, 2, connectionData->ainConfig.ch2.avgRatio) == DRV_AIN_STATUS_OK)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_INFO,  "Channel 2 averaging ration %d successfully set\r\n", connectionData->ainConfig.ch1.avgRatio);
				}
				else
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "Unable to set channel 2 averaging ratio\r\n");
				}
				if(xSemaphoreGive(connectionData->initSig) != pdTRUE)
				{
					LOGGING_Write("SStream service", LOGGING_MSG_TYPE_ERROR,  "There is a problem to release init semaphore\r\n");
					connectionData->state = SSTREAM_STATE_ERROR;
					break;
				}
			}
			break;
		case SSTREAM_STATE_ERROR:
			SYSTEM_ReportError(SYSTEM_ERROR_LEVEL_LOW);
			vTaskDelay(portMAX_DELAY);
			break;
		}
	}
}

sstream_status_t			SSTREAM_Init(void)
{
	memset(&prvSSTREAM_DATA, 0, sizeof(sstream_data_t));
	return SSTREAM_STATUS_OK;
}
sstream_status_t			SSTREAM_CreateChannel(sstream_connection_info* connectionHandler, uint32_t timeout)
{
	if(prvSSTREAM_DATA.activeConnectionsNo > SSTREAM_CONNECTIONS_MAX_NO) return SSTREAM_STATUS_ERROR;
	uint32_t currentId = prvSSTREAM_DATA.activeConnectionsNo;
	memcpy(&prvSSTREAM_DATA.connections[currentId].connectionInfo, connectionHandler,  sizeof(sstream_connection_info));
	if(xTaskCreate(
			prvSSTREAM_TaskFunc,
			SSTREAM_TASK_NAME,
			SSTREAM_TASK_STACK_SIZE,
			&prvSSTREAM_DATA.connections[currentId],
			SSTREAM_TASK_PRIO,
			&prvSSTREAM_DATA.connections[currentId].taskHandle) != pdPASS) return SSTREAM_STATUS_ERROR;

	prvSSTREAM_DATA.connections[currentId].initSig = xSemaphoreCreateBinary();
	if(prvSSTREAM_DATA.connections[currentId].initSig == NULL) return SSTREAM_STATUS_ERROR;
	prvSSTREAM_DATA.connections[currentId].guard = xSemaphoreCreateMutex();
	if(prvSSTREAM_DATA.connections[currentId].guard == NULL) return SSTREAM_STATUS_ERROR;
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[currentId].initSig,
			pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	prvSSTREAM_DATA.connections[currentId].connectionInfo.id = currentId;
	prvSSTREAM_DATA.activeConnectionsNo += 1;
	return SSTREAM_STATUS_OK;
}
sstream_status_t				SSTREAM_GetConnectionByIP(sstream_connection_info* connectionHandler, uint8_t ip[4], uint16_t port)
{
	uint32_t 	connectionIterator = 0;
	uint8_t 	ipIterator = 0;
	while(connectionIterator < prvSSTREAM_DATA.activeConnectionsNo)
	{
		for(ipIterator = 0; ipIterator < 4; ipIterator++){
			if(prvSSTREAM_DATA.connections[connectionIterator].connectionInfo.serverIp[ipIterator] == ip[ipIterator]) continue;
			break;
		}
		if(ipIterator == 4 && prvSSTREAM_DATA.connections[connectionIterator].connectionInfo.serverport == port)
		{
			connectionHandler = &prvSSTREAM_DATA.connections[connectionIterator].connectionInfo;
			return SSTREAM_STATUS_OK;
		}
		connectionIterator += 1;
	}
	return SSTREAM_STATUS_ERROR;

}
sstream_status_t				SSTREAM_GetConnectionByID(sstream_connection_info** connectionHandler, uint32_t id)
{
	uint32_t 	connectionIterator = 0;
	while(connectionIterator < prvSSTREAM_DATA.activeConnectionsNo)
	{
		if(prvSSTREAM_DATA.connections[connectionIterator].connectionInfo.id == id)
		{
			*connectionHandler = &prvSSTREAM_DATA.connections[connectionIterator].connectionInfo;
			return SSTREAM_STATUS_OK;
		}
		connectionIterator += 1;
	}

	return SSTREAM_STATUS_ERROR;
}
sstream_status_t				SSTREAM_Start(sstream_connection_info* connectionHandler, uint32_t timeout)
{
	if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
			SSTREAM_TASK_START_BIT,
			eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;

	/* Wait until configuration is applied*/
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].initSig,
			pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	return SSTREAM_STATUS_OK;
}
sstream_status_t				SSTREAM_Stop(sstream_connection_info* connectionHandler, uint32_t timeout)
{
	if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
			SSTREAM_TASK_STOP_BIT,
			eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;

	/* Wait until configuration is applied*/
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].initSig,
			pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	return SSTREAM_STATUS_OK;
}
sstream_status_t				SSTREAM_SetResolution(sstream_connection_info* connectionHandler, sstream_adc_resolution_t resolution, uint32_t timeout)
{

	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.resolution = resolution;
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	/* Send request to configure AIN*/
	if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
			SSTREAM_TASK_SET_ADC_RESOLUTION_BIT,
			eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;

	/* Wait until configuration is applied*/
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].initSig,
			pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	return SSTREAM_STATUS_OK;
}
sstream_status_t				SSTREAM_SetSamplingTime(sstream_connection_info* connectionHandler, uint32_t stime, uint32_t timeout)
{
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.samplingTime = stime;
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	/* Send request to configure AIN*/
	if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
			SSTREAM_TASK_SET_ADC_STIME_BIT,
			eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;

	/* Wait until configuration is applied*/
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].initSig,
			pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	return SSTREAM_STATUS_OK;
}
sstream_status_t				SSTREAM_SetClkDiv(sstream_connection_info* connectionHandler, sstream_adc_clk_div_t adcClkDiv, uint32_t timeout)
{
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.clockDiv = adcClkDiv;
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	/* Send request to configure AIN*/
	if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
			SSTREAM_TASK_SET_ADC_CLOCK_DIV_BIT,
			eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;

	/* Wait until configuration is applied*/
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].initSig,
			pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	return SSTREAM_STATUS_OK;
}
sstream_status_t				SSTREAM_SetChannelSamplingTime(sstream_connection_info* connectionHandler, uint32_t channel, sstream_adc_sampling_time_t stime, uint32_t timeout)
{
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	if(channel == 1)
	{
		prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch1.sampleTime = stime;
	}
	if(channel == 2)
	{
		prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch2.sampleTime = stime;
	}
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	/* Send request to configure AIN*/
	if(channel == 1)
	{
		if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
				SSTREAM_TASK_SET_ADC_CH1_STIME_BIT,
				eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;
	}
	if(channel == 2)
	{
		if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
				SSTREAM_TASK_SET_ADC_CH2_STIME_BIT,
				eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;
	}

	/* Wait until configuration is applied*/
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].initSig,
			pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return SSTREAM_STATUS_OK;
}
sstream_status_t				SSTREAM_SetChannelOffset(sstream_connection_info* connectionHandler, uint32_t channel, uint32_t offset, uint32_t timeout)
{
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	if(channel == 1)
	{
		prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch1.offset = offset;
	}
	if(channel == 2)
	{
		prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch2.offset = offset;
	}
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	/* Send request to configure AIN*/
	if(channel == 1)
	{
		if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
				SSTREAM_TASK_SET_ADC_CH1_OFFSET_BIT,
				eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;
	}
	if(channel == 2)
	{
		if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
				SSTREAM_TASK_SET_ADC_CH2_OFFSET_BIT,
				eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;
	}

	/* Wait until configuration is applied*/
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].initSig,
			pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return SSTREAM_STATUS_OK;
}

sstream_status_t				SSTREAM_SetChannelAvgRatio(sstream_connection_info* connectionHandler, uint32_t channel, sstream_adc_ch_avg_ratio_t avgRatio, uint32_t timeout)
{
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	if(channel == 1)
	{
		prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch1.avgRatio = avgRatio;
	}
	if(channel == 2)
	{
		prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch2.avgRatio = avgRatio;
	}
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	/* Send request to configure AIN*/
	if(channel == 1)
	{
		if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
				SSTREAM_TASK_SET_ADC_CH1_AVERAGING_RATIO,
				eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;
	}
	if(channel == 2)
	{
		if(xTaskNotify(prvSSTREAM_DATA.connections[connectionHandler->id].taskHandle,
				SSTREAM_TASK_SET_ADC_CH2_AVERAGING_RATIO,
				eSetBits) != pdPASS) return SSTREAM_STATUS_ERROR;
	}

	/* Wait until configuration is applied*/
	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].initSig,
			pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return SSTREAM_STATUS_OK;
}
sstream_adc_resolution_t		SSTREAM_GetResolution(sstream_connection_info* connectionHandler, uint32_t timeout)
{
	sstream_adc_resolution_t resolution;

	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	resolution = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.resolution;
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return resolution;
}
sstream_adc_clk_div_t			SSTREAM_GetClkDiv(sstream_connection_info* connectionHandler, uint32_t timeout)
{
	sstream_adc_clk_div_t adcClockDiv;

	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	adcClockDiv = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.clockDiv;
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return adcClockDiv;
}
uint32_t						SSTREAM_GetSamplingTime(sstream_connection_info* connectionHandler, uint32_t timeout)
{
	uint32_t stime;

	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	stime = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.samplingTime;
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return stime;
}
sstream_adc_sampling_time_t		SSTREAM_GetChannelSamplingTime(sstream_connection_info* connectionHandler, uint32_t channel, uint32_t timeout)
{
	sstream_adc_sampling_time_t chstime;

	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	if(channel == 1)
	{
		chstime = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch1.sampleTime;
	}
	if(channel == 2)
	{
		chstime = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch2.sampleTime;
	}
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return chstime;
}
uint32_t						SSTREAM_GetChannelOffset(sstream_connection_info* connectionHandler, uint32_t channel, uint32_t timeout)
{
	uint32_t choffset;

	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	if(channel == 1)
	{
		choffset = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch1.offset;
	}
	if(channel == 2)
	{
		choffset = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch2.offset;
	}
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return choffset;
}

sstream_adc_ch_avg_ratio_t		SSTREAM_GetChannelAvgRatio(sstream_connection_info* connectionHandler, uint32_t channel, uint32_t timeout)
{
	sstream_adc_ch_avg_ratio_t avgRatio;

	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	if(channel == 1)
	{
		avgRatio = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch1.avgRatio;
	}
	if(channel == 2)
	{
		avgRatio = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.ch2.avgRatio;
	}
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return avgRatio;
}

uint32_t						SSTREAM_GetAdcInputClk(sstream_connection_info* connectionHandler, uint32_t timeout)
{
	uint32_t inputClk;

	if(xSemaphoreTake(prvSSTREAM_DATA.connections[connectionHandler->id].guard, pdMS_TO_TICKS(timeout)) != pdTRUE) return SSTREAM_STATUS_ERROR;
	inputClk = prvSSTREAM_DATA.connections[connectionHandler->id].ainConfig.inputClk;
	if(xSemaphoreGive(prvSSTREAM_DATA.connections[connectionHandler->id].guard) != pdTRUE) return SSTREAM_STATUS_ERROR;

	return inputClk;
}


