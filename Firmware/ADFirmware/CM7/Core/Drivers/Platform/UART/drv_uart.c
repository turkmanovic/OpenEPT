/*
 * uart.c
 *
 *  Created on: Nov 5, 2023
 *      Author: Haris
 */
#include "main.h"

#include "FreeRTOS.h"
#include "semphr.h"


#include "drv_uart.h"

typedef struct
{
	drv_uart_instance_t 				instance;
	drv_uart_initialization_status_t	initState;
	drv_uart_config_t					config;
	SemaphoreHandle_t					lock;
	UART_HandleTypeDef 					deviceHandler;
}drv_uart_handle_t;

static drv_uart_handle_t 	prvDRV_UART_INSTANCES[CONF_UART_INSTANCES_MAX_NUMBER];


void HAL_UART_MspInit(UART_HandleTypeDef* husart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	if(husart->Instance == USART3)
	{
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
		PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
		  Error_Handler();
		}

		/* Peripheral clock enable */
		__HAL_RCC_USART3_CLK_ENABLE();

		__HAL_RCC_GPIOD_CLK_ENABLE();
		/**USART3 GPIO Configuration
		PD8     ------> USART3_TX
		PD9     ------> USART3_RX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


	    HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
	    HAL_NVIC_EnableIRQ(USART3_IRQn);

	}

}

void HAL_USART_MspDeInit(UART_HandleTypeDef* husart)
{
	if(husart->Instance==USART3)
	{
		__HAL_RCC_USART3_CLK_DISABLE();

		/**USART3 GPIO Configuration
		PD8     ------> USART3_TX
		PD9     ------> USART3_RX
		*/
		HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);
	}

}

drv_uart_status_t	DRV_UART_Init()
{
	memset(prvDRV_UART_INSTANCES, 0, CONF_UART_INSTANCES_MAX_NUMBER*sizeof(drv_uart_handle_t));
	return	DRV_UART_STATUS_OK;
}
drv_uart_status_t	DRV_UART_Instance_Init(drv_uart_instance_t instance, drv_uart_config_t* config)
{
	if(prvDRV_UART_INSTANCES[instance].lock != NULL) return DRV_UART_STATUS_ERROR;

	prvDRV_UART_INSTANCES[instance].lock = xSemaphoreCreateMutex();

	if(prvDRV_UART_INSTANCES[instance].lock == NULL) return DRV_UART_STATUS_ERROR;

	switch(instance)
	{
	case DRV_UART_INSTANCE_1:
		prvDRV_UART_INSTANCES[instance].deviceHandler.Instance = USART1;
		break;
	case DRV_UART_INSTANCE_3:
		prvDRV_UART_INSTANCES[instance].deviceHandler.Instance = USART3;
		break;

	}

	/*TODO: Only baudrate is configurable*/
	prvDRV_UART_INSTANCES[instance].deviceHandler.Init.BaudRate = config->baudRate;
	prvDRV_UART_INSTANCES[instance].deviceHandler.Init.WordLength = UART_WORDLENGTH_8B;
	prvDRV_UART_INSTANCES[instance].deviceHandler.Init.StopBits = UART_STOPBITS_1;
	prvDRV_UART_INSTANCES[instance].deviceHandler.Init.Parity = UART_PARITY_NONE;
	prvDRV_UART_INSTANCES[instance].deviceHandler.Init.Mode = UART_MODE_TX_RX;
	prvDRV_UART_INSTANCES[instance].deviceHandler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	prvDRV_UART_INSTANCES[instance].deviceHandler.Init.OverSampling = UART_OVERSAMPLING_16;
	prvDRV_UART_INSTANCES[instance].deviceHandler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	prvDRV_UART_INSTANCES[instance].deviceHandler.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	prvDRV_UART_INSTANCES[instance].deviceHandler.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if (HAL_UART_Init(&prvDRV_UART_INSTANCES[instance].deviceHandler) != HAL_OK) return DRV_UART_STATUS_ERROR;
	if (HAL_UARTEx_SetTxFifoThreshold(&prvDRV_UART_INSTANCES[instance].deviceHandler, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) return DRV_UART_STATUS_ERROR;
	if (HAL_UARTEx_SetRxFifoThreshold(&prvDRV_UART_INSTANCES[instance].deviceHandler, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) return DRV_UART_STATUS_ERROR;
	if (HAL_UARTEx_DisableFifoMode(&prvDRV_UART_INSTANCES[instance].deviceHandler) != HAL_OK) return DRV_UART_STATUS_ERROR;

	prvDRV_UART_INSTANCES[instance].initState = DRV_UART_INITIALIZATION_STATUS_INIT;

	return	DRV_UART_STATUS_OK;
}
drv_uart_status_t	DRV_UART_TransferData(drv_uart_instance_t instance, uint8_t* buffer, uint8_t size, uint32_t timeout)
{
	if(prvDRV_UART_INSTANCES[instance].initState != DRV_UART_INITIALIZATION_STATUS_INIT || prvDRV_UART_INSTANCES[instance].lock == NULL) return DRV_UART_STATUS_ERROR;

	if(xSemaphoreTake(prvDRV_UART_INSTANCES[instance].lock, pdMS_TO_TICKS(timeout)) != pdTRUE) return DRV_UART_STATUS_ERROR;

	if(HAL_UART_Transmit(&prvDRV_UART_INSTANCES[instance].deviceHandler, buffer, size, timeout) != HAL_OK) return DRV_UART_STATUS_ERROR;

	if(xSemaphoreGive(prvDRV_UART_INSTANCES[instance].lock) != pdTRUE) return DRV_UART_STATUS_ERROR;

	return	DRV_UART_STATUS_OK;
}
