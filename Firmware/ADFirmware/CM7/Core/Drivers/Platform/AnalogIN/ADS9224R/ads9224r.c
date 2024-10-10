/*
 * ads9224r.c
 *
 *  Created on: Oct 7, 2024
 *      Author: Haris
 */

#include "../../SPI/drv_spi.h"
#include "../../GPIO/drv_gpio.h"
#include "ads9224r.h"
#include "stm32h7xx_hal_conf.h"


TIM_HandleTypeDef 	htim8;
TIM_HandleTypeDef 	htim4;
TIM_HandleTypeDef 	htim5;


extern SPI_HandleTypeDef hspi4;
extern SPI_HandleTypeDef hspi5;



/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 2-1;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = 10-1;
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = 15;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
	{
	Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
	{
	Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
	{
	Error_Handler();
	}
	if (HAL_TIM_OnePulse_Init(&htim8, TIM_OPMODE_SINGLE) != HAL_OK)
	{
	Error_Handler();
	}
	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_COMBINED_RESETTRIGGER;
	sSlaveConfig.InputTrigger = TIM_TS_ETRF;
	sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_INVERTED;
	sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
	sSlaveConfig.TriggerFilter = 0;
	if (HAL_TIM_SlaveConfigSynchro(&htim8, &sSlaveConfig) != HAL_OK)
	{
	Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
	{
	Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 5;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
	if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
	{
	Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
	{
	Error_Handler();
	}

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CSTimer_Init(void)
{

	/* USER CODE BEGIN TIM4_Init 0 */

	/* USER CODE END TIM4_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 2-1;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 280-1;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
	{
	Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
	{
	Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
	{
	Error_Handler();
	}
	if (HAL_TIM_OnePulse_Init(&htim4, TIM_OPMODE_SINGLE) != HAL_OK)
	{
	Error_Handler();
	}
	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_COMBINED_RESETTRIGGER;
	sSlaveConfig.InputTrigger = TIM_TS_ETRF;
	sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
	sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
	sSlaveConfig.TriggerFilter = 0;
	if (HAL_TIM_SlaveConfigSynchro(&htim4, &sSlaveConfig) != HAL_OK)
	{
	Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
	{
	Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 5;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCNIdleState = TIM_OCIDLESTATE_SET;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
	Error_Handler();
	}

}

volatile uint8_t data1[200] = {0};
volatile uint8_t data2[200] = {0};
/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_Convst_Init(void)
{

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim5.Instance = TIM5;
	htim5.Init.Prescaler = 2-1;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim5.Init.Period = 310-1;
	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
	{
	Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
	{
	Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
	{
	Error_Handler();
	}
//	if (HAL_TIM_OnePulse_Init(&htim5, TIM_OPMODE_SINGLE) != HAL_OK)
//	{
//	Error_Handler();
//	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
	{
	Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 30;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCNIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
	if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
	{
	Error_Handler();
	}

}



ads9224r_status_t	ADS9224R_Init()
{
	drv_spi_config_t config;

	config.mode = DRV_SPI_MODE_MASTER;
	config.phase= DRV_SPI_PHASE_1EDGE;
	config.polarity = DRV_SPI_POLARITY_LOW;

	uint8_t dataTx[2];
	uint8_t dataRx;

	if(DRV_SPI_Instance_Init(DRV_SPI_INSTANCE3, &config) != DRV_SPI_STATUS_OK) return ADS9224R_STATUS_ERROR;

	/*CS Config*/
	drv_gpio_pin_init_conf_t 	csCOnfig;
	csCOnfig.mode = DRV_GPIO_PIN_MODE_OUTPUT_PP;
	csCOnfig.pullState = DRV_GPIO_PIN_PULL_NOPULL;

	drv_gpio_pin_init_conf_t 	readyStrobeConfig;
	readyStrobeConfig.mode = DRV_GPIO_PIN_MODE_INPUT;
	readyStrobeConfig.pullState = DRV_GPIO_PIN_PULL_NOPULL;

	/*CS*/
	DRV_GPIO_Port_Init(DRV_GPIO_PORT_D);
	DRV_GPIO_Pin_Init(DRV_GPIO_PORT_D, 12, &csCOnfig);
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_SET);

	/*CONVST Config*/
	DRV_GPIO_Port_Init(DRV_GPIO_PORT_A);
	DRV_GPIO_Pin_Init(DRV_GPIO_PORT_A, 3, 	&csCOnfig);

	/*Power down/ RST Config*/
	DRV_GPIO_Port_Init(DRV_GPIO_PORT_G);
	DRV_GPIO_Pin_Init(DRV_GPIO_PORT_G, 8, 	&csCOnfig);

	DRV_GPIO_Port_Init(DRV_GPIO_PORT_G);
	DRV_GPIO_Pin_Init(DRV_GPIO_PORT_G, 10, &readyStrobeConfig);


	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_G, 8, DRV_GPIO_PIN_STATE_SET);
	while(DRV_GPIO_Pin_ReadState(DRV_GPIO_PORT_G, 10) != DRV_GPIO_PIN_STATE_SET);
	while(DRV_GPIO_Pin_ReadState(DRV_GPIO_PORT_G, 10) != DRV_GPIO_PIN_STATE_RESET);

	//Read and write to registers
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_A, 3, DRV_GPIO_PIN_STATE_SET);

	//Send Read command to read register 0x05
	dataTx[0] = 0x25;
	dataTx[1] = 0x00;
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_RESET);
	if(DRV_SPI_TransmitData(dataTx, 2, 1000) != DRV_SPI_STATUS_OK)
	{
		while(1);
	}
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_SET);

	//Wait for ready
	while(DRV_GPIO_Pin_ReadState(DRV_GPIO_PORT_G, 10) != DRV_GPIO_PIN_STATE_SET);

	//Read register 0x05
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_RESET);
	if(DRV_SPI_ReceiveData(&dataRx, 1, 1000) != DRV_SPI_STATUS_OK)
	{
		while(1);
	}
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_SET);

	//Send Write command to write register 0x05 with data 0x02
	dataTx[0] = 0x15;
	dataTx[1] = 0x02;
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_RESET);
	if(DRV_SPI_TransmitData(dataTx, 2, 1000) != DRV_SPI_STATUS_OK)
	{
		while(1);
	}
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_SET);





	//Send Read command to read register 0x05
	dataTx[0] = 0x25;
	dataTx[1] = 0x00;
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_RESET);
	if(DRV_SPI_TransmitData(dataTx, 2, 1000) != DRV_SPI_STATUS_OK)
	{
		while(1);
	}
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_SET);

	//Wait for ready
	while(DRV_GPIO_Pin_ReadState(DRV_GPIO_PORT_G, 10) != DRV_GPIO_PIN_STATE_SET);


	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_RESET);
	if(DRV_SPI_ReceiveData(&dataRx, 1, 1000) != DRV_SPI_STATUS_OK)
	{
		while(1);
	}
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_SET);

	DRV_SPI_Instance_DeInit(DRV_SPI_INSTANCE3);

//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//    /* Prepare pins for timers*/
//	/* Convst is control by timer */
////    memset(GPIO_InitStruct, 0, sizeof(GPIO_InitTypeDef));
////	GPIO_InitStruct.Pin = GPIO_PIN_3;
////	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
////	GPIO_InitStruct.Pull = GPIO_NOPULL;
////	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
////	GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
////	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
//
//
//	/* CS is controled by time*/
////    memset(GPIO_InitStruct, 0, sizeof(GPIO_InitTypeDef));
////    GPIO_InitStruct.Pin = GPIO_PIN_12;
////    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
////    GPIO_InitStruct.Pull = GPIO_NOPULL;
////    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
////    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
////    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
//    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_12);
//
//    /*Disable spi pins*/
////    memset(GPIO_InitStruct, 0, sizeof(GPIO_InitTypeDef));
////    GPIO_InitStruct.Pin = GPIO_PIN_2;
////    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
////    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
////    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
////    GPIO_InitStruct.Alternate = GPIO_AF7_SPI3;
////    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);
//
////    memset(GPIO_InitStruct, 0, sizeof(GPIO_InitTypeDef));
////    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
////    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
////    GPIO_InitStruct.Pull = GPIO_NOPULL;
////    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
////    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
////    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10);
//    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_11);


	MX_Convst_Init();
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_RESET);
	DRV_GPIO_Pin_SetState(DRV_GPIO_PORT_D, 12, DRV_GPIO_PIN_STATE_SET);
	MX_CSTimer_Init();
	MX_TIM3_Init();


	MX_SPI4_Init();
	MX_SPI5_Init();
	HAL_SPI_Receive_IT(&hspi4, data1, 200);
	HAL_SPI_Receive_IT(&hspi5, data2, 200);



	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);

	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);








	while(1);

	return ADS9224R_STATUS_OK;


}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi)
{


	HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_4);
}


