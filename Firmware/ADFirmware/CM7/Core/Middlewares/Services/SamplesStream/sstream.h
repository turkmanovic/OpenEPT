/**
 ******************************************************************************
 * @file   	sstream.h
 * @brief  	...
 * @author	Haris Turkmanovic
 * @email	haris.turkmanovic@gmail.com
 * @date	November 2022
 ******************************************************************************
 */

#ifndef CORE_MIDDLEWARES_SERVICES_SAMPLESSTREAM_SSTREAM_H_
#define CORE_MIDDLEWARES_SERVICES_SAMPLESSTREAM_SSTREAM_H_

#include <stdint.h>
#include "globalConfig.h"

#define 	SSTREAM_TASK_NAME					CONF_SSTREAM_TASK_NAME
#define 	SSTREAM_TASK_PRIO					CONF_SSTREAM_TASK_PRIO
#define 	SSTREAM_TASK_STACK_SIZE				CONF_SSTREAM_TASK_STACK_SIZE
#define 	SSTREAM_CONNECTIONS_MAX_NO			CONF_SSTREAM_CONNECTIONS_MAX_NO

#define		SSTREAM_AIN_DEFAULT_RESOLUTION		CONF_SSTREAM_AIN_DEFAULT_RESOLUTION
#define		SSTREAM_AIN_DEFAULT_CLOCK_DIV		CONF_SSTREAM_AIN_DEFAULT_CLOCK_DIV
#define		SSTREAM_AIN_DEFAULT_CH_SAMPLE_TIME	CONF_SSTREAM_AIN_DEFAULT_CH_SAMPLE_TIME
#define		SSTREAM_AIN_DEFAULT_SAMPLE_TIME		CONF_SSTREAM_AIN_DEFAULT_SAMPLE_TIME

/**
 * @brief Samples stream service task state
 */
typedef enum
{
	SSTREAM_STATE_INIT,			/*!< Samples stream service initialization state */
	SSTREAM_STATE_SERVICE,		/*!< Samples stream service is in service state */
	SSTREAM_STATE_ERROR			/*!< Samples stream service is in error state */
}sstream_state_t;

typedef enum{
	SSTREAM_STATUS_OK,
	SSTREAM_STATUS_ERROR
}sstream_status_t;

typedef enum{
	SSTREAM_ADC_RESOLUTION_16BIT = 16,
	SSTREAM_ADC_RESOLUTION_14BIT = 14,
	SSTREAM_ADC_RESOLUTION_12BIT = 12,
	SSTREAM_ADC_RESOLUTION_10BIT = 10
}sstream_adc_resolution_t;

typedef enum{
	SSTREAM_ADC_SAMPLING_TIME_1C5 = 1,
	SSTREAM_ADC_SAMPLING_TIME_2C5 = 2,
	SSTREAM_ADC_SAMPLING_TIME_8C5 = 8,
	SSTREAM_ADC_SAMPLING_TIME_16C5 = 16,
	SSTREAM_ADC_SAMPLING_TIME_32C5 = 32,
	SSTREAM_ADC_SAMPLING_TIME_64C5 = 64,
	SSTREAM_ADC_SAMPLING_TIME_387C5 = 387,
	SSTREAM_ADC_SAMPLING_TIME_810C5 = 810
}sstream_adc_sampling_time_t;

typedef enum{
	SSTREAM_ACQUISITION_STATE_UNDEFINED = 0,
	SSTREAM_ACQUISITION_STATE_STOP,
	SSTREAM_ACQUISITION_STATE_START,
	SSTREAM_ACQUISITION_STATE_STREAM
}sstream_acquisition_state_t;

typedef struct
{
	uint8_t		serverIp[4];
	uint16_t	serverport;
	uint32_t	id;
}sstream_connection_info;



sstream_status_t				SSTREAM_Init(void);
sstream_status_t				SSTREAM_CreateChannel(sstream_connection_info* connectionHandler, uint32_t timeout);
sstream_status_t				SSTREAM_GetConnectionByID(sstream_connection_info** connectionHandler, uint32_t id);
sstream_status_t				SSTREAM_GetConnectionByIP(sstream_connection_info* connectionHandler, uint8_t ip[4], uint16_t port);
sstream_status_t				SSTREAM_Start(sstream_connection_info* connectionHandler);
sstream_status_t				SSTREAM_Stop(sstream_connection_info* connectionHandler);
sstream_status_t				SSTREAM_SetResolution(sstream_connection_info* connectionHandler, sstream_adc_resolution_t resolution, uint32_t timeout);
sstream_status_t				SSTREAM_SetChannelSamplingTime(sstream_connection_info* connectionHandler, uint32_t channel, sstream_adc_sampling_time_t stime, uint32_t timeout);
sstream_adc_resolution_t		SSTREAM_GetResolution(sstream_connection_info* connectionHandler, uint32_t timeout);
sstream_adc_sampling_time_t		SSTREAM_GetChannelSamplingSpeed(sstream_connection_info* connectionHandler, uint32_t channel, uint32_t timeout);
sstream_status_t				SSTREAM_Calibrate(sstream_connection_info* connectionHandler);

#endif /* CORE_MIDDLEWARES_SERVICES_SAMPLESSTREAM_SSTREAM_H_ */
