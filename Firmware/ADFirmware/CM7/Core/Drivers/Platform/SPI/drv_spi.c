/*
 * drv_spi.c
 *
 *  Created on: Jun 30, 2024
 *      Author: Pavle Lakic & Dimitrije Lilic
 */

#include "main.h"
#include "drv_spi.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include <string.h>

typedef struct drv_spi_handle_t
{
	drv_spi_instance_t					instance;
	drv_spi_initialization_status_t		initState;
	drv_spi_config_t					config;
	SemaphoreHandle_t					lock;
	SPI_HandleTypeDef 					deviceHandler;
}drv_spi_handle_t;

static drv_spi_handle_t prvDRV_SPI_INSTANCE;

/**
  * @brief  Initialize the SPI MSP.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	PeriphClkInitStruct.PeriphClockSelection =  RCC_PERIPHCLK_SPI3;
	PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_CLKP;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
	  Error_Handler();
	}

	__HAL_RCC_SPI3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/**SPI3 GPIO Configuration
	PB2     ------> SPI3_MOSI
	PC10	------> SPI3_SCK
	PC11	------> SPI3_MISO
	*/

	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_SPI3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_SPI3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief  De-Initialize the SPI MSP.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	__HAL_RCC_SPI3_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);
}

drv_spi_status_t	DRV_SPI_Init()
{
	memset(&prvDRV_SPI_INSTANCE, 0, sizeof(drv_spi_handle_t));
	return	DRV_SPI_STATUS_OK;
}

drv_spi_status_t	DRV_SPI_Instance_Init(drv_spi_instance_t instance, drv_spi_config_t* config)
{
	if(prvDRV_SPI_INSTANCE.lock != NULL) return DRV_SPI_STATUS_ERROR;

	prvDRV_SPI_INSTANCE.lock = xSemaphoreCreateMutex();

	if(prvDRV_SPI_INSTANCE.lock == NULL) return DRV_SPI_STATUS_ERROR;

	/*TODO Check what user can actually configure. */
	prvDRV_SPI_INSTANCE.deviceHandler.Instance = SPI3;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.Mode = config->mode;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.Direction = SPI_DIRECTION_2LINES;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.DataSize = SPI_DATASIZE_8BIT;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.CLKPolarity = config->polarity;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.CLKPhase = config->phase;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.NSS = SPI_NSS_SOFT;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.FirstBit = SPI_FIRSTBIT_MSB;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.TIMode = SPI_TIMODE_DISABLE;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.CRCPolynomial = 0x0;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	prvDRV_SPI_INSTANCE.deviceHandler.Init.IOSwap = SPI_IO_SWAP_DISABLE;

	if (HAL_SPI_Init(&prvDRV_SPI_INSTANCE.deviceHandler) != HAL_OK) return DRV_SPI_STATUS_ERROR;

	prvDRV_SPI_INSTANCE.initState = DRV_SPI_INITIALIZATION_STATUS_INIT;

	return	DRV_SPI_STATUS_OK;
}

/*TODO check with another MCU for Transmit and Receive */
drv_spi_status_t	DRV_SPI_TransmitData(uint8_t* buffer, uint8_t size, uint32_t timeout)
{
	if(prvDRV_SPI_INSTANCE.initState != DRV_SPI_INITIALIZATION_STATUS_INIT || prvDRV_SPI_INSTANCE.lock == NULL) return DRV_SPI_STATUS_ERROR;

	if(xSemaphoreTake(prvDRV_SPI_INSTANCE.lock, pdMS_TO_TICKS(timeout)) != pdTRUE) return DRV_SPI_STATUS_ERROR;

	//transmit function here.

	if(xSemaphoreGive(prvDRV_SPI_INSTANCE.lock) != pdTRUE) return DRV_SPI_STATUS_ERROR;

	return	DRV_SPI_STATUS_OK;
}

drv_spi_status_t	DRV_SPI_ReceiveData(uint8_t* buffer, uint8_t size, uint32_t timeout)
{
	if(prvDRV_SPI_INSTANCE.initState != DRV_SPI_INITIALIZATION_STATUS_INIT || prvDRV_SPI_INSTANCE.lock == NULL) return DRV_SPI_STATUS_ERROR;

	if(xSemaphoreTake(prvDRV_SPI_INSTANCE.lock, pdMS_TO_TICKS(timeout)) != pdTRUE) return DRV_SPI_STATUS_ERROR;

	//receive function here.

	if(xSemaphoreGive(prvDRV_SPI_INSTANCE.lock) != pdTRUE) return DRV_SPI_STATUS_ERROR;

	return	DRV_SPI_STATUS_OK;
}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */

//static void MX_SPI3_Init(void)
//{
//
//  /* USER CODE BEGIN SPI3_Init 0 */
//
//  /* USER CODE END SPI3_Init 0 */
//
//  /* USER CODE BEGIN SPI3_Init 1 */
//
//  /* USER CODE END SPI3_Init 1 */
//  /* SPI3 parameter configuration*/
//  hspi3.Instance = SPI3;
//  hspi3.Init.Mode = SPI_MODE_MASTER;
//  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
//  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
//  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
//  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
//  hspi3.Init.NSS = SPI_NSS_SOFT;
//  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
//  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
//  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
//  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//  hspi3.Init.CRCPolynomial = 7;
//  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
//  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
//  if (HAL_SPI_Init(&hspi3) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN SPI3_Init 2 */
//
//  /* USER CODE END SPI3_Init 2 */
//
//}
