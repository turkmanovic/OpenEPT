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
	void*								deviceBaseAddr;
}drv_uart_handle_t;

static drv_uart_handle_t 	prvDRV_UART_INSTANCES[CONF_UART_INSTANCES_MAX_NUMBER];
static USART_HandleTypeDef 	prvDRV_UART_PLATFORM_HANDLER;


void HAL_USART_MspInit(USART_HandleTypeDef* husart)
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

    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB12     ------> USART3_CK
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = STLINK_RX_Pin|STLINK_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  }

}

void HAL_USART_MspDeInit(USART_HandleTypeDef* husart)
{
  if(husart->Instance==USART3)
  {
    __HAL_RCC_USART3_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, STLINK_RX_Pin|STLINK_TX_Pin);
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
		prvDRV_UART_INSTANCES[instance].deviceBaseAddr = USART1;
		break;
	case DRV_UART_INSTANCE_3:
		prvDRV_UART_INSTANCES[instance].deviceBaseAddr = USART3;
		break;

	}

	/*TODO: Only baudrate is configurable*/
	prvDRV_UART_PLATFORM_HANDLER.Instance = prvDRV_UART_INSTANCES[instance].deviceBaseAddr;
	prvDRV_UART_PLATFORM_HANDLER.Init.BaudRate = config->baudRate;
	prvDRV_UART_PLATFORM_HANDLER.Init.WordLength = USART_WORDLENGTH_8B;
	prvDRV_UART_PLATFORM_HANDLER.Init.StopBits = USART_STOPBITS_1;
	prvDRV_UART_PLATFORM_HANDLER.Init.Parity = USART_PARITY_NONE;
	prvDRV_UART_PLATFORM_HANDLER.Init.Mode = USART_MODE_TX_RX;
	prvDRV_UART_PLATFORM_HANDLER.Init.CLKPolarity = USART_POLARITY_LOW;
	prvDRV_UART_PLATFORM_HANDLER.Init.CLKPhase = USART_PHASE_1EDGE;
	prvDRV_UART_PLATFORM_HANDLER.Init.CLKLastBit = USART_LASTBIT_DISABLE;
	prvDRV_UART_PLATFORM_HANDLER.Init.ClockPrescaler = USART_PRESCALER_DIV1;
	prvDRV_UART_PLATFORM_HANDLER.SlaveMode = USART_SLAVEMODE_DISABLE;

	if (HAL_USART_Init(&prvDRV_UART_PLATFORM_HANDLER) != HAL_OK) return DRV_UART_STATUS_ERROR;
	if (HAL_USARTEx_SetTxFifoThreshold(&prvDRV_UART_PLATFORM_HANDLER, USART_TXFIFO_THRESHOLD_1_8) != HAL_OK) return DRV_UART_STATUS_ERROR;
	if (HAL_USARTEx_SetRxFifoThreshold(&prvDRV_UART_PLATFORM_HANDLER, USART_RXFIFO_THRESHOLD_1_8) != HAL_OK) return DRV_UART_STATUS_ERROR;
	if (HAL_USARTEx_DisableFifoMode(&prvDRV_UART_PLATFORM_HANDLER) != HAL_OK) return DRV_UART_STATUS_ERROR;

	prvDRV_UART_INSTANCES[instance].initState = DRV_UART_INITIALIZATION_STATUS_INIT;

	return	DRV_UART_STATUS_OK;
}
drv_uart_status_t	DRV_UART_TransferData(drv_uart_instance_t instance, uint8_t* buffer, uint8_t size, uint32_t timeout)
{
	if(prvDRV_UART_INSTANCES[instance].initState != DRV_UART_INITIALIZATION_STATUS_INIT || prvDRV_UART_INSTANCES[instance].lock == NULL) return DRV_UART_STATUS_ERROR;

	if(xSemaphoreTake(prvDRV_UART_INSTANCES[instance].lock, pdMS_TO_TICKS(timeout)) != pdTRUE) return DRV_UART_STATUS_ERROR;

	if(HAL_USART_Transmit((USART_HandleTypeDef*)(&prvDRV_UART_INSTANCES[instance].deviceBaseAddr), buffer, size, timeout) != HAL_OK) return DRV_UART_STATUS_ERROR;

	if(xSemaphoreGive(prvDRV_UART_INSTANCES[instance].lock) != pdTRUE) return DRV_UART_STATUS_ERROR;

	return	DRV_UART_STATUS_OK;
}
