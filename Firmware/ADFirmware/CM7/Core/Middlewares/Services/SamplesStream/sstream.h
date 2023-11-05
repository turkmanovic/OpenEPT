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

#define 	SSTREAM_TASK_NAME		CONF_SSTREAM_TASK_NAME
#define 	SSTREAM_TASK_PRIO		CONF_SSTREAM_TASK_PRIO
#define 	SSTREAM_TASK_STACK_SIZE	CONF_SSTREAM_TASK_STACK_SIZE
#define 	SSTREAM_UDP_STREAM_PORT	CONF_SSTREAM_UDP_PORT


#define  	SSTREAM_TASK_NEW_FRAME_RECEIVED		0x00000020

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
	SSTREAM_ADC_SAMPLING_SPEED_1C5,
	SSTREAM_ADC_SAMPLING_SPEED_2C5,
	SSTREAM_ADC_SAMPLING_SPEED_8C5,
	SSTREAM_ADC_SAMPLING_SPEED_16C5,
	SSTREAM_ADC_SAMPLING_SPEED_32C5,
	SSTREAM_ADC_SAMPLING_SPEED_64C5,
	SSTREAM_ADC_SAMPLING_SPEED_387C5,
	SSTREAM_ADC_SAMPLING_SPEED_810C5
}sstream_adc_sampling_speed_t;

typedef enum{
	SSTREAM_STATE_STOP,
	SSTREAM_STATE_START,
	SSTREAM_STATE_STREAM
}sstream_state_t;



sstream_status_t			SSTREAM_Init(void);
sstream_status_t			SSTREAM();
sstream_status_t			SSTREAM_Stop();
sstream_status_t			SSTREAM_SetResolution(sstream_adc_resolution_t resolution);
sstream_adc_resolution_t	SSTREAM_GetReolsution();
sstream_status_t			SSTREAM_SetSamplingSpeed(sstream_adc_sampling_speed_t samplingSpeed);
sstream_state_t				SSTREAM_GetState();
sstream_status_t			SSTREAM_WaitStart(uint32_t timeoutMs);
sstream_status_t			SSTREAM_WaitStop(uint32_t timeoutMs);

#endif /* CORE_MIDDLEWARES_SERVICES_SAMPLESSTREAM_SSTREAM_H_ */
