/**
 ******************************************************************************
 * @file   	drv_ain.h
 * @brief  	...
 * @author	Haris Turkmanovic
 * @email	haris.turkmanovic@gmail.com
 * @date	November 2022
 ******************************************************************************
 */

#ifndef CORE_DRIVERS_PLATFORM_ANALOGIN_H_
#define CORE_DRIVERS_PLATFORM_ANALOGIN_H_

#include <stdint.h>
#include "globalConfig.h"


#define DRV_AIN_ADC_BUFFER_MAX_SIZE	CONF_AIN_MAX_BUFFER_SIZE

typedef enum
{
	DRV_AIN_STATUS_OK,
	DRV_AIN_STATUS_ERROR
}drv_ain_status;

typedef enum
{
	DRV_AIN_ADC_1 = 0x01,
	DRV_AIN_ADC_2 = 0x02,
	DRV_AIN_ADC_3 = 0x04
}drv_ain_adc_t;

typedef enum
{
	DRV_AIN_ADC_DMA_CONNECTION_STATUS_CONNECTED,
	DRV_AIN_ADC_DMA_CONNECTION_STATUS_DISCONNECTED,
	DRV_AIN_ADC_DMA_CONNECTION_STATUS_UKNOWN
}drv_ain_adc_dma_connection_status_t;

typedef enum
{
	DRV_AIN_ADC_ACQUISITION_STATUS_ACTIVE,
	DRV_AIN_ADC_ACQUISITION_STATUS_INACTIVE,
	DRV_AIN_ADC_ACQUISITION_STATUS_UKNOWN
}drv_ain_adc_acquisition_status_t;

typedef enum
{
	DRV_AIN_ADC_RESOLUTION_8BIT,
	DRV_AIN_ADC_RESOLUTION_10BIT,
	DRV_AIN_ADC_RESOLUTION_12BIT,
	DRV_AIN_ADC_RESOLUTION_14BIT,
	DRV_AIN_ADC_RESOLUTION_16BIT
}drv_ain_adc_resolution_t;

typedef enum
{
	DRV_AIN_ADC_SAMPLE_TIME_1C5,
	DRV_AIN_ADC_SAMPLE_TIME_2C5,
	DRV_AIN_ADC_SAMPLE_TIME_8C5,
	DRV_AIN_ADC_SAMPLE_TIME_16C5,
	DRV_AIN_ADC_SAMPLE_TIME_32C5,
	DRV_AIN_ADC_SAMPLE_TIME_64C5,
	DRV_AIN_ADC_SAMPLE_TIME_387C5,
	DRV_AIN_ADC_SAMPLE_TIME_810C5,
}drv_ain_adc_sample_time_t;

typedef uint8_t	drv_ain_adc_channel_t;
typedef uint8_t	drv_ain_adc_clock_div_t;

typedef void (*drv_ain_adc_stream_callback)(uint32_t);


typedef struct
{
	drv_ain_adc_channel_t 		channelNo;
	drv_ain_adc_clock_div_t 	clockDiv;
	drv_ain_adc_resolution_t 	resolution;
}drv_ain_adc_config_t;

drv_ain_status 						DRV_AIN_Init(drv_ain_adc_t adc, drv_ain_adc_config_t* configuration);
drv_ain_status 						DRV_AIN_Start(drv_ain_adc_t adc);
drv_ain_status 						DRV_AIN_Stop(drv_ain_adc_t adc);
drv_ain_adc_acquisition_status_t 	DRV_AIN_GetAcquisitonStatus(drv_ain_adc_t adc);
drv_ain_status 						DRV_AIN_SetResolution(drv_ain_adc_t adc, drv_ain_adc_resolution_t res);
drv_ain_status 						DRV_AIN_SetSamplingTime(drv_ain_adc_t adc, drv_ain_adc_sample_time_t stime);
drv_ain_adc_resolution_t 			DRV_AIN_GetResolution(drv_ain_adc_t adc);
drv_ain_adc_sample_time_t 			DRV_AIN_GetSamplingTime(drv_ain_adc_t adc);

drv_ain_status 						DRV_AIN_Stream_Enable(drv_ain_adc_t adc, uint32_t sampleSize); // Enable DMA
drv_ain_status 						DRV_AIN_Stream_RegisterCallback(drv_ain_adc_t adc, drv_ain_adc_stream_callback cbfunction);
drv_ain_status 						DRV_AIN_Stream_SubmitAddr(drv_ain_adc_t adc, uint32_t addr);

#endif /* CORE_DRIVERS_PLATFORM_ANALOGIN_AIN_H_ */
