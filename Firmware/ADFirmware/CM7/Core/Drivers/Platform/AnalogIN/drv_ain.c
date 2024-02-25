/**
 ******************************************************************************
 * @file   	drv_ain.c
 * @brief  	...
 * @author	Haris Turkmanovic
 * @email	haris.turkmanovic@gmail.com
 * @date	November 2022
 ******************************************************************************
 */
#include "drv_ain.h"
#include "drv_gpio.h"
#include "string.h"
#include "main.h"


static ADC_HandleTypeDef 				prvDRV_AIN_DEVICE_ADC_HANDLER;
static DMA_HandleTypeDef 				prvDRV_AIN_DEVICE_DMA_HANDLER;
static TIM_HandleTypeDef 				prvDRV_AIN_DEVICE_TIMER_HANDLER;
static uint16_t							prvDRV_AIN_ADC_DATA_SAMPLES[CONF_AIN_MAX_BUFFER_NO][DRV_AIN_ADC_BUFFER_MAX_SIZE]
																							__attribute__((section(".ADCSamplesBuffer")));
static drv_ain_adc_acquisition_status_t	prvDRV_AIN_ACQUISITION_STATUS;
static ADC_ChannelConfTypeDef 			prvDRV_AIN_ADC_CHANNEL_1_CONFIG;
static ADC_ChannelConfTypeDef 			prvDRV_AIN_ADC_CHANNEL_2_CONFIG;
static drv_ain_adc_config_t				prvDRV_AIN_ADC_CONFIG;


/**
  * @brief This function handles ADC3 global interrupt.
  */
void ADC3_IRQHandler(void)
{
	HAL_ADC_IRQHandler(&prvDRV_AIN_DEVICE_ADC_HANDLER);
}


/**
  * @brief This function handles TIM1 update interrupt.
  */
void TIM1_UP_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&prvDRV_AIN_DEVICE_TIMER_HANDLER);
}

/**
  * @brief This function handles TIM1 capture compare interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&prvDRV_AIN_DEVICE_TIMER_HANDLER);
}

/**
  * @brief This function handles BDMA channel0 global interrupt.
  */
void BDMA_Channel0_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&prvDRV_AIN_DEVICE_DMA_HANDLER);
	//DRV_GPIO_Pin_ToogleFromISR(DRV_GPIO_PORT_E, 15);
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15);

	/*Enable for debugging purposes*/
	//ITM_SendChar('a');
}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM1)
  {
    __HAL_RCC_TIM1_CLK_ENABLE();
  }

}

/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM1)
  {
    __HAL_RCC_TIM1_CLK_DISABLE();
  }

}
/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	if(hadc->Instance==ADC3)
	{

		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
		PeriphClkInitStruct.PLL2.PLL2M = 4;
		PeriphClkInitStruct.PLL2.PLL2N = 10;
		PeriphClkInitStruct.PLL2.PLL2P = 2;
		PeriphClkInitStruct.PLL2.PLL2Q = 2;
		PeriphClkInitStruct.PLL2.PLL2R = 2;
		PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
		PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
		PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
		PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
		  Error_Handler();
		}

		/* Peripheral clock enable */
		__HAL_RCC_ADC3_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		/**ADC3 GPIO Configuration
		PC2_C     ------> ADC3_INP0
		PC3_C     ------> ADC3_INP1
		*/
		HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC2, SYSCFG_SWITCH_PC2_OPEN);

		HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC3, SYSCFG_SWITCH_PC3_OPEN);

		/* ADC3 interrupt Init */
//	    HAL_NVIC_SetPriority(ADC3_IRQn, 5, 0);
//	    HAL_NVIC_EnableIRQ(ADC3_IRQn);
		/* ADC3 DMA Init */
		/* ADC3 Init */
		prvDRV_AIN_DEVICE_DMA_HANDLER.Instance = BDMA_Channel0;
		prvDRV_AIN_DEVICE_DMA_HANDLER.Init.Request = BDMA_REQUEST_ADC3;
		prvDRV_AIN_DEVICE_DMA_HANDLER.Init.Direction = DMA_PERIPH_TO_MEMORY;
		prvDRV_AIN_DEVICE_DMA_HANDLER.Init.PeriphInc = DMA_PINC_DISABLE;
		prvDRV_AIN_DEVICE_DMA_HANDLER.Init.MemInc = DMA_MINC_ENABLE;
		prvDRV_AIN_DEVICE_DMA_HANDLER.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		prvDRV_AIN_DEVICE_DMA_HANDLER.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		prvDRV_AIN_DEVICE_DMA_HANDLER.Init.Mode = DMA_CIRCULAR;
		prvDRV_AIN_DEVICE_DMA_HANDLER.Init.Priority = DMA_PRIORITY_HIGH;
		if (HAL_DMA_Init(&prvDRV_AIN_DEVICE_DMA_HANDLER) != HAL_OK)
		{
		  Error_Handler();
		}

		__HAL_LINKDMA(hadc,DMA_Handle,prvDRV_AIN_DEVICE_DMA_HANDLER);
	}

}
/**
  * Enable DMA controller clock
  */
static void prvDRV_AIN_InitDMA(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_BDMA_CLK_ENABLE();

  /* DMA interrupt init */
  /* BDMA_Channel0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BDMA_Channel0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(BDMA_Channel0_IRQn);

}
/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance==ADC3)
	{
		__HAL_RCC_ADC3_CLK_DISABLE();

		HAL_DMA_DeInit(hadc->DMA_Handle);
	}

}
static void							prvDRV_AIN_DMAHalfComplitedCallback(ADC_HandleTypeDef *adc)
{

}

static drv_ain_status				prvDRV_AIN_InitDeviceTimer()
{

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	prvDRV_AIN_DEVICE_TIMER_HANDLER.Instance = TIM1;
	prvDRV_AIN_DEVICE_TIMER_HANDLER.Init.Prescaler = 1;
	prvDRV_AIN_DEVICE_TIMER_HANDLER.Init.CounterMode = TIM_COUNTERMODE_UP;
	prvDRV_AIN_DEVICE_TIMER_HANDLER.Init.Period = 1;
	prvDRV_AIN_DEVICE_TIMER_HANDLER.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	prvDRV_AIN_DEVICE_TIMER_HANDLER.Init.RepetitionCounter = 0;
	prvDRV_AIN_DEVICE_TIMER_HANDLER.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&prvDRV_AIN_DEVICE_TIMER_HANDLER) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&prvDRV_AIN_DEVICE_TIMER_HANDLER, &sClockSourceConfig) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	if (HAL_TIM_OC_Init(&prvDRV_AIN_DEVICE_TIMER_HANDLER) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&prvDRV_AIN_DEVICE_TIMER_HANDLER, &sMasterConfig) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	return DRV_AIN_STATUS_OK;
}
static drv_ain_status				prvDRV_AIN_InitDeviceADC()
{
	memset(&prvDRV_AIN_ADC_CHANNEL_1_CONFIG, 0, sizeof(ADC_ChannelConfTypeDef));
	memset(&prvDRV_AIN_ADC_CHANNEL_2_CONFIG, 0, sizeof(ADC_ChannelConfTypeDef));

	prvDRV_AIN_DEVICE_ADC_HANDLER.Instance 						= ADC3;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.Resolution				= ADC_RESOLUTION_16B;
	prvDRV_AIN_ADC_CONFIG.resolution							= DRV_AIN_ADC_RESOLUTION_16BIT;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler 			= ADC_CLOCK_ASYNC_DIV1;
	prvDRV_AIN_ADC_CONFIG.clockDiv								= DRV_AIN_ADC_CLOCK_DIV_1;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ScanConvMode 			= ADC_SCAN_ENABLE;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.EOCSelection 			= ADC_EOC_SINGLE_CONV;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.LowPowerAutoWait 		= DISABLE;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ContinuousConvMode 		= DISABLE;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.NbrOfConversion 			= 2;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.DiscontinuousConvMode 	= DISABLE;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ExternalTrigConv 		= ADC_EXTERNALTRIG_T1_TRGO;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ExternalTrigConvEdge 	= ADC_EXTERNALTRIGCONVEDGE_RISING;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.Overrun 					= ADC_OVR_DATA_OVERWRITTEN;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.LeftBitShift 			= ADC_LEFTBITSHIFT_NONE;
	prvDRV_AIN_DEVICE_ADC_HANDLER.Init.OversamplingMode 		= DISABLE;
	if (HAL_ADC_Init(&prvDRV_AIN_DEVICE_ADC_HANDLER) != HAL_OK) return DRV_AIN_STATUS_ERROR;

	/** Configure Regular Channel 1 */
	prvDRV_AIN_ADC_CHANNEL_1_CONFIG.Channel 					= ADC_CHANNEL_0;
	prvDRV_AIN_ADC_CHANNEL_1_CONFIG.Rank 						= ADC_REGULAR_RANK_1;
	prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SamplingTime 				= ADC_SAMPLETIME_1CYCLE_5;
	prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SingleDiff 					= ADC_SINGLE_ENDED;
	prvDRV_AIN_ADC_CHANNEL_1_CONFIG.OffsetNumber 				= ADC_OFFSET_NONE;
	prvDRV_AIN_ADC_CHANNEL_1_CONFIG.Offset 						= 0;
	prvDRV_AIN_ADC_CHANNEL_1_CONFIG.OffsetSignedSaturation 		= DISABLE;
	prvDRV_AIN_ADC_CONFIG.ch1.sampleTime						= DRV_AIN_ADC_SAMPLE_TIME_1C5;
	if (HAL_ADC_ConfigChannel(&prvDRV_AIN_DEVICE_ADC_HANDLER, &prvDRV_AIN_ADC_CHANNEL_1_CONFIG) != HAL_OK) return DRV_AIN_STATUS_ERROR;

	/** Configure Regular Channel 2 */
	prvDRV_AIN_ADC_CHANNEL_2_CONFIG.Channel 					= ADC_CHANNEL_1;
	prvDRV_AIN_ADC_CHANNEL_2_CONFIG.Rank 						= ADC_REGULAR_RANK_2;
	prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SamplingTime 				= ADC_SAMPLETIME_1CYCLE_5;
	prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SingleDiff 					= ADC_SINGLE_ENDED;
	prvDRV_AIN_ADC_CHANNEL_2_CONFIG.OffsetNumber 				= ADC_OFFSET_NONE;
	prvDRV_AIN_ADC_CHANNEL_2_CONFIG.Offset 						= 0;
	prvDRV_AIN_ADC_CHANNEL_2_CONFIG.OffsetSignedSaturation 		= DISABLE;
	prvDRV_AIN_ADC_CONFIG.ch2.sampleTime						= DRV_AIN_ADC_SAMPLE_TIME_1C5;
	if (HAL_ADC_ConfigChannel(&prvDRV_AIN_DEVICE_ADC_HANDLER, &prvDRV_AIN_ADC_CHANNEL_2_CONFIG) != HAL_OK) return DRV_AIN_STATUS_ERROR;

	if (HAL_ADC_RegisterCallback(&prvDRV_AIN_DEVICE_ADC_HANDLER, HAL_ADC_CONVERSION_HALF_CB_ID, prvDRV_AIN_DMAHalfComplitedCallback)!= HAL_OK)return DRV_AIN_STATUS_ERROR;
	if (HAL_ADC_RegisterCallback(&prvDRV_AIN_DEVICE_ADC_HANDLER, HAL_ADC_CONVERSION_COMPLETE_CB_ID, prvDRV_AIN_DMAHalfComplitedCallback)!= HAL_OK)return DRV_AIN_STATUS_ERROR;

	return DRV_AIN_STATUS_OK;
}

drv_ain_status 						DRV_AIN_Init(drv_ain_adc_t adc, drv_ain_adc_config_t* configuration)
{
	/* Initialize pin that will be used for signaling end of conversion */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin  = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
    __HAL_RCC_GPIOE_CLK_ENABLE();
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

    /* Initialize configuration structure */
    prvDRV_AIN_ADC_CONFIG.clockDiv 		= DRV_AIN_ADC_CLOCK_DIV_UKNOWN;
    prvDRV_AIN_ADC_CONFIG.resolution 	= DRV_AIN_ADC_RESOLUTION_UKNOWN;
    prvDRV_AIN_ADC_CONFIG.ch1.channel	= 1;
    prvDRV_AIN_ADC_CONFIG.ch1.sampleTime= DRV_AIN_ADC_SAMPLE_TIME_UKNOWN;
    prvDRV_AIN_ADC_CONFIG.ch1.channel	= 2;
    prvDRV_AIN_ADC_CONFIG.ch1.sampleTime= DRV_AIN_ADC_SAMPLE_TIME_UKNOWN;


    /* Initialize DMA */
	prvDRV_AIN_InitDMA();

    /* Initialize ADC */
	prvDRV_AIN_InitDeviceADC();

    /* Initialize Time */
	prvDRV_AIN_InitDeviceTimer();

	/* Set initial acquisition status*/
	prvDRV_AIN_ACQUISITION_STATUS = DRV_AIN_ADC_ACQUISITION_STATUS_UKNOWN;

//	/* Start ADC */
//	DRV_AIN_Start(DRV_AIN_ADC_3);
	return DRV_AIN_STATUS_OK;
}
drv_ain_status 						DRV_AIN_Start(drv_ain_adc_t adc)
{
	memset((void*)prvDRV_AIN_ADC_DATA_SAMPLES, 0, 2*CONF_AIN_MAX_BUFFER_NO*DRV_AIN_ADC_BUFFER_MAX_SIZE);
	if(prvDRV_AIN_ACQUISITION_STATUS == DRV_AIN_ADC_ACQUISITION_STATUS_ACTIVE) return DRV_AIN_STATUS_ERROR;
	if(HAL_TIM_Base_Start(&prvDRV_AIN_DEVICE_TIMER_HANDLER) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	if(HAL_ADC_Start_DMA(&prvDRV_AIN_DEVICE_ADC_HANDLER, (uint32_t*)prvDRV_AIN_ADC_DATA_SAMPLES, CONF_AIN_MAX_BUFFER_NO*DRV_AIN_ADC_BUFFER_MAX_SIZE)) return DRV_AIN_STATUS_ERROR;
	prvDRV_AIN_ACQUISITION_STATUS = DRV_AIN_ADC_ACQUISITION_STATUS_ACTIVE;
	return DRV_AIN_STATUS_OK;
}
drv_ain_status 						DRV_AIN_Stop(drv_ain_adc_t adc)
{
	if(prvDRV_AIN_ACQUISITION_STATUS != DRV_AIN_ADC_ACQUISITION_STATUS_ACTIVE) return DRV_AIN_STATUS_ERROR;
	if(HAL_TIM_Base_Stop(&prvDRV_AIN_DEVICE_TIMER_HANDLER) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	if(HAL_ADC_Stop_DMA(&prvDRV_AIN_DEVICE_ADC_HANDLER)) return DRV_AIN_STATUS_ERROR;
	prvDRV_AIN_ACQUISITION_STATUS = DRV_AIN_ADC_ACQUISITION_STATUS_INACTIVE;
	return DRV_AIN_STATUS_OK;
}
drv_ain_adc_acquisition_status_t 	DRV_AIN_GetAcquisitonStatus(drv_ain_adc_t adc)
{
	return prvDRV_AIN_ACQUISITION_STATUS;
}
drv_ain_status 						DRV_AIN_SetResolution(drv_ain_adc_t adc, drv_ain_adc_resolution_t res)
{
	if(prvDRV_AIN_ACQUISITION_STATUS == DRV_AIN_ADC_ACQUISITION_STATUS_ACTIVE) return DRV_AIN_STATUS_ERROR;
	switch(res)
	{
	case DRV_AIN_ADC_RESOLUTION_10BIT:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.Resolution = ADC_RESOLUTION_10B;
		prvDRV_AIN_ADC_CONFIG.resolution = DRV_AIN_ADC_RESOLUTION_10BIT;
		break;
	case DRV_AIN_ADC_RESOLUTION_12BIT:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.Resolution = ADC_RESOLUTION_12B;
		prvDRV_AIN_ADC_CONFIG.resolution = DRV_AIN_ADC_RESOLUTION_12BIT;
		break;
	case DRV_AIN_ADC_RESOLUTION_14BIT:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.Resolution = ADC_RESOLUTION_14B;
		prvDRV_AIN_ADC_CONFIG.resolution = DRV_AIN_ADC_RESOLUTION_14BIT;
		break;
	case DRV_AIN_ADC_RESOLUTION_16BIT:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.Resolution = ADC_RESOLUTION_16B;
		prvDRV_AIN_ADC_CONFIG.resolution = DRV_AIN_ADC_RESOLUTION_16BIT;
		break;
	default:
		prvDRV_AIN_ADC_CONFIG.resolution = DRV_AIN_ADC_RESOLUTION_UKNOWN;
		return DRV_AIN_STATUS_ERROR;
	}
	if (HAL_ADC_Init(&prvDRV_AIN_DEVICE_ADC_HANDLER) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	return DRV_AIN_STATUS_OK;
}
drv_ain_status 						DRV_AIN_SetClockDiv(drv_ain_adc_t adc, drv_ain_adc_clock_div_t div)
{
	if(prvDRV_AIN_ACQUISITION_STATUS == DRV_AIN_ADC_ACQUISITION_STATUS_ACTIVE) return DRV_AIN_STATUS_ERROR;
	switch(div)
	{
	case DRV_AIN_ADC_CLOCK_DIV_1:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_1;
		break;
	case DRV_AIN_ADC_CLOCK_DIV_2:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_2;
		break;
	case DRV_AIN_ADC_CLOCK_DIV_4:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_4;
		break;
	case DRV_AIN_ADC_CLOCK_DIV_8:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8;
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_8;
		break;
	case DRV_AIN_ADC_CLOCK_DIV_16:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV16;
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_16;
		break;
	case DRV_AIN_ADC_CLOCK_DIV_32:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV32;
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_32;
		break;
	case DRV_AIN_ADC_CLOCK_DIV_64:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV64;
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_64;
		break;
	case DRV_AIN_ADC_CLOCK_DIV_128:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV128;
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_128;
		break;
	case DRV_AIN_ADC_CLOCK_DIV_256:
		prvDRV_AIN_DEVICE_ADC_HANDLER.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV256;
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_256;
		break;
	default:
		prvDRV_AIN_ADC_CONFIG.clockDiv = DRV_AIN_ADC_CLOCK_DIV_UKNOWN;
		break;
	}
	if (HAL_ADC_Init(&prvDRV_AIN_DEVICE_ADC_HANDLER) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	return DRV_AIN_STATUS_OK;
}
drv_ain_status 						DRV_AIN_SetChannelsSamplingTime(drv_ain_adc_t adc, drv_ain_adc_sample_time_t stime)
{
	if(prvDRV_AIN_ACQUISITION_STATUS == DRV_AIN_ADC_ACQUISITION_STATUS_ACTIVE) return DRV_AIN_STATUS_ERROR;
	switch(stime)
	{
	case DRV_AIN_ADC_SAMPLE_TIME_1C5:
		prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
		prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
		prvDRV_AIN_ADC_CONFIG.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_1C5;
		prvDRV_AIN_ADC_CONFIG.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_1C5;
		break;
	case DRV_AIN_ADC_SAMPLE_TIME_2C5:
		prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
		prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
		prvDRV_AIN_ADC_CONFIG.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_2C5;
		prvDRV_AIN_ADC_CONFIG.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_2C5;
		break;
	case DRV_AIN_ADC_SAMPLE_TIME_8C5:
		prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
		prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
		prvDRV_AIN_ADC_CONFIG.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_8C5;
		prvDRV_AIN_ADC_CONFIG.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_8C5;
		break;
	case DRV_AIN_ADC_SAMPLE_TIME_16C5:
		prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SamplingTime = ADC_SAMPLETIME_16CYCLES_5;
		prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SamplingTime = ADC_SAMPLETIME_16CYCLES_5;
		prvDRV_AIN_ADC_CONFIG.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_16C5;
		prvDRV_AIN_ADC_CONFIG.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_16C5;
		break;
	case DRV_AIN_ADC_SAMPLE_TIME_32C5:
		prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SamplingTime = ADC_SAMPLETIME_32CYCLES_5;
		prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SamplingTime = ADC_SAMPLETIME_32CYCLES_5;
		prvDRV_AIN_ADC_CONFIG.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_32C5;
		prvDRV_AIN_ADC_CONFIG.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_32C5;
		break;
	case DRV_AIN_ADC_SAMPLE_TIME_64C5:
		prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
		prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
		prvDRV_AIN_ADC_CONFIG.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_64C5;
		prvDRV_AIN_ADC_CONFIG.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_64C5;
		break;
	case DRV_AIN_ADC_SAMPLE_TIME_387C5:
		prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;
		prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SamplingTime = ADC_SAMPLETIME_387CYCLES_5;
		prvDRV_AIN_ADC_CONFIG.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_387C5;
		prvDRV_AIN_ADC_CONFIG.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_387C5;
		break;
	case DRV_AIN_ADC_SAMPLE_TIME_810C5:
		prvDRV_AIN_ADC_CHANNEL_1_CONFIG.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
		prvDRV_AIN_ADC_CHANNEL_2_CONFIG.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
		prvDRV_AIN_ADC_CONFIG.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_810C5;
		prvDRV_AIN_ADC_CONFIG.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_810C5;
		break;
	default:
		prvDRV_AIN_ADC_CONFIG.ch1.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_UKNOWN;
		prvDRV_AIN_ADC_CONFIG.ch2.sampleTime = DRV_AIN_ADC_SAMPLE_TIME_UKNOWN;
		return DRV_AIN_STATUS_ERROR;
	}
	if (HAL_ADC_ConfigChannel(&prvDRV_AIN_DEVICE_ADC_HANDLER, &prvDRV_AIN_ADC_CHANNEL_1_CONFIG) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	if (HAL_ADC_ConfigChannel(&prvDRV_AIN_DEVICE_ADC_HANDLER, &prvDRV_AIN_ADC_CHANNEL_2_CONFIG) != HAL_OK) return DRV_AIN_STATUS_ERROR;
	return DRV_AIN_STATUS_OK;
}

drv_ain_status 						DRV_AIN_SetChannelOffset(drv_ain_adc_t adc, uint32_t channel, uint32_t offset)
{

	return DRV_AIN_STATUS_OK;
}
drv_ain_status 						DRV_AIN_SetSamplingResolutionTime(drv_ain_adc_t adc, uint32_t time)
{

	return DRV_AIN_STATUS_OK;
}
drv_ain_adc_resolution_t 			DRV_AIN_GetResolution(drv_ain_adc_t adc)
{
	return prvDRV_AIN_ADC_CONFIG.resolution;
}
drv_ain_adc_sample_time_t 			DRV_AIN_GetSamplingTime(drv_ain_adc_t adc, drv_ain_adc_channel_t channel)
{
	drv_ain_adc_sample_time_t stime;
	if(channel == 1)
	{
		stime =  prvDRV_AIN_ADC_CONFIG.ch1.sampleTime;
	}
	else
	{
		stime =  prvDRV_AIN_ADC_CONFIG.ch2.sampleTime;
	}
	return stime;
}
drv_ain_status 						DRV_AIN_GetADCClk(drv_ain_adc_t adc, uint32_t *clk)
{
	*clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_ADC);
	return DRV_AIN_STATUS_OK;
}
drv_ain_status 						DRV_AIN_Stream_RegisterCallback(drv_ain_adc_t adc, drv_ain_adc_stream_callback cbfunction)
{

	return DRV_AIN_STATUS_OK;
}
drv_ain_status 						DRV_AIN_Stream_SubmitAddr(drv_ain_adc_t adc, uint32_t addr)
{

	return DRV_AIN_STATUS_OK;
}

