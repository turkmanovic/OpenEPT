/*
 * timer.c
 *
 *  Created on: Oct 2, 2024
 *      Author: Haris
 */
#include "drv_timer.h"

static TIM_HandleTypeDef		prvDRV_TIMER_PLATFORM_HANDLER[DRV_TIMER_MAX_NUMBER_OF_TIMERS];
static drv_timer_handle_t 		prvDRV_TIMER_HANDLER[DRV_TIMER_MAX_NUMBER_OF_CHANNELS];
static drv_timer_init_status_t  prvDRV_TIMER_INIT_STATUS ;

drv_timer_status_t DRV_Timer_Init()
{
	memset(prvDRV_TIMER_PLATFORM_HANDLER, 0, DRV_TIMER_MAX_NUMBER_OF_TIMERS*sizeof(TIM_HandleTypeDef));
	memset(prvDRV_TIMER_HANDLER, 0, DRV_TIMER_MAX_NUMBER_OF_CHANNELS*sizeof(drv_timer_handle_t));
	prvDRV_TIMER_INIT_STATUS = DRV_TIMER_INIT_STATUS_INIT;
	return DRV_TIMER_STATUS_OK;
}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(htim_base->Instance == TIM1)
	{
		__HAL_RCC_TIM1_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	}
	if(htim_base->Instance == TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();
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
	if(htim_base->Instance==TIM2)
	{
		__HAL_RCC_TIM1_CLK_DISABLE();
	}
}
drv_timer_status_t DRV_Timer_Init_Instance(drv_timer_instance_t instance, drv_timer_config_t* config)
{

	TIM_ClockConfigTypeDef sClockSourceConfig 	= {0};
	TIM_MasterConfigTypeDef sMasterConfig 		= {0};

	if(prvDRV_TIMER_INIT_STATUS != DRV_TIMER_INIT_STATUS_INIT) return DRV_TIMER_STATUS_ERROR;
	if(prvDRV_TIMER_HANDLER[instance].initState == DRV_TIMER_INIT_STATUS_INIT) return DRV_TIMER_STATUS_ERROR;

	switch(instance)
	{
	case DRV_TIMER_1:
		prvDRV_TIMER_PLATFORM_HANDLER[instance].Instance = TIM1;
		break;
	case DRV_TIMER_2:
		prvDRV_TIMER_PLATFORM_HANDLER[instance].Instance = TIM2;
		break;
	case DRV_TIMER_3:
		prvDRV_TIMER_PLATFORM_HANDLER[instance].Instance = TIM3;
		break;
	case DRV_TIMER_4:
		prvDRV_TIMER_PLATFORM_HANDLER[instance].Instance = TIM4;
		break;
	}

	prvDRV_TIMER_PLATFORM_HANDLER[instance].Init.Prescaler = config->prescaler - 1;
	switch(config->mode)
	{
	case DRV_TIMER_COUNTER_MODE_UP:
		prvDRV_TIMER_PLATFORM_HANDLER[instance].Init.CounterMode = TIM_COUNTERMODE_UP;
		break;
	case DRV_TIMER_COUNTER_MODE_DOWN:
		prvDRV_TIMER_PLATFORM_HANDLER[instance].Init.CounterMode = TIM_COUNTERMODE_DOWN;
		break;
	}
	prvDRV_TIMER_PLATFORM_HANDLER[instance].Init.Period = config->period -1 ;
	switch(config->div)
	{
	case DRV_TIMER_DIV_1:
		prvDRV_TIMER_PLATFORM_HANDLER[instance].Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		break;
	case DRV_TIMER_DIV_2:
		prvDRV_TIMER_PLATFORM_HANDLER[instance].Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
		break;
	case DRV_TIMER_DIV_4:
		prvDRV_TIMER_PLATFORM_HANDLER[instance].Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
		break;
	}
	prvDRV_TIMER_PLATFORM_HANDLER[instance].Init.RepetitionCounter = 0;
	prvDRV_TIMER_PLATFORM_HANDLER[instance].Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&prvDRV_TIMER_PLATFORM_HANDLER[instance]) != HAL_OK) return DRV_TIMER_STATUS_OK;
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&prvDRV_TIMER_PLATFORM_HANDLER[instance], &sClockSourceConfig) != HAL_OK) return DRV_TIMER_STATUS_OK;
	if (HAL_TIM_PWM_Init(&prvDRV_TIMER_PLATFORM_HANDLER[instance]) != HAL_OK) return DRV_TIMER_STATUS_OK;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&prvDRV_TIMER_PLATFORM_HANDLER[instance], &sMasterConfig) != HAL_OK) return DRV_TIMER_STATUS_OK;

	prvDRV_TIMER_HANDLER[instance].lock = xSemaphoreCreateMutex();

	if(prvDRV_TIMER_HANDLER[instance].lock == NULL) return DRV_TIMER_STATUS_OK;

	prvDRV_TIMER_HANDLER[instance].baseAddr = &prvDRV_TIMER_PLATFORM_HANDLER[instance];

	prvDRV_TIMER_HANDLER[instance].initState = DRV_TIMER_INIT_STATUS_INIT;

	return DRV_TIMER_STATUS_OK;
}
drv_timer_status_t DRV_Timer_Channel_Init(drv_timer_instance_t instance, drv_timer_channel_t channel, drv_timer_channel_config_t* config)
{
	TIM_OC_InitTypeDef sConfigOC = {0};

	if(prvDRV_TIMER_INIT_STATUS != DRV_TIMER_INIT_STATUS_INIT) return DRV_TIMER_STATUS_ERROR;

	if(prvDRV_TIMER_HANDLER[instance].initState != DRV_TIMER_INIT_STATUS_INIT) return DRV_TIMER_STATUS_ERROR;

	switch(config->mode)
	{
	case DRV_TIMER_CHANNEL_MODE_PWM1:
		  prvDRV_TIMER_HANDLER[instance].channels[channel].mode = config->mode;
		  sConfigOC.OCMode = TIM_OCMODE_PWM1;
		  sConfigOC.Pulse = 0;
		  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		  if (HAL_TIM_PWM_ConfigChannel((TIM_HandleTypeDef*)prvDRV_TIMER_HANDLER[instance].baseAddr, &sConfigOC, channel << 2) != HAL_OK) return DRV_TIMER_STATUS_ERROR;
		  prvDRV_TIMER_HANDLER[instance].channels[channel].init = DRV_TIMER_INIT_STATUS_INIT;
		  break;
	default:
		break;
	}

	prvDRV_TIMER_HANDLER[instance].numberOfInitChannels += 1;
	return DRV_TIMER_STATUS_OK;
}
drv_timer_status_t DRV_Timer_Channel_PWM_Start(drv_timer_instance_t instance, drv_timer_channel_t channel, uint32_t period, uint32_t timeout)
{
	if(prvDRV_TIMER_INIT_STATUS != DRV_TIMER_INIT_STATUS_INIT) return DRV_TIMER_STATUS_ERROR;
	if(xSemaphoreTake(prvDRV_TIMER_HANDLER[instance].lock, timeout) != pdTRUE)  return DRV_TIMER_STATUS_ERROR;
	if(prvDRV_TIMER_HANDLER[instance].initState != DRV_TIMER_INIT_STATUS_INIT)
	{
		xSemaphoreGive(prvDRV_TIMER_HANDLER[instance].lock);
		return DRV_TIMER_STATUS_ERROR;
	}
	if(prvDRV_TIMER_HANDLER[instance].channels[channel].init != DRV_TIMER_INIT_STATUS_INIT)
	{
		xSemaphoreGive(prvDRV_TIMER_HANDLER[instance].lock);
		return DRV_TIMER_STATUS_ERROR;
	}
	prvDRV_TIMER_HANDLER[instance].channels[channel].period = period;
	switch(channel)
	{
	case DRV_TIMER_CHANNEL_1:
		((TIM_HandleTypeDef*)(prvDRV_TIMER_HANDLER[instance].baseAddr))->Instance->CCR1 = period;
		break;
	case DRV_TIMER_CHANNEL_2:
		((TIM_HandleTypeDef*)(prvDRV_TIMER_HANDLER[instance].baseAddr))->Instance->CCR2 = period;
		break;
	case DRV_TIMER_CHANNEL_3:
		((TIM_HandleTypeDef*)(prvDRV_TIMER_HANDLER[instance].baseAddr))->Instance->CCR3 = period;
		break;
	case DRV_TIMER_CHANNEL_4:
		((TIM_HandleTypeDef*)(prvDRV_TIMER_HANDLER[instance].baseAddr))->Instance->CCR4 = period;
		break;
	}

	if(HAL_TIM_PWM_Start((TIM_HandleTypeDef*)prvDRV_TIMER_HANDLER[instance].baseAddr, channel << 2) != HAL_OK)
	{
		xSemaphoreGive(prvDRV_TIMER_HANDLER[instance].lock);
		return DRV_TIMER_STATUS_OK;
	}

	xSemaphoreGive(prvDRV_TIMER_HANDLER[instance].lock);
	return DRV_TIMER_STATUS_OK;
}
