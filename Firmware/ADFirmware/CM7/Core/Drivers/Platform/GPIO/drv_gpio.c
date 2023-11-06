/**
 ******************************************************************************
 * @file   	gpio.c
 * @brief  	...
 * @author	Haris Turkmanovic
 * @email	haris.turkmanovic@gmail.com
 * @date	November 2022
 ******************************************************************************
 */
#include "drv_gpio.h"
#include "main.h"


static drv_gpio_port_handle_t prvDRV_GPIO_PORTS[DRV_GPIO_PORT_MAX_NUMBER];

static uint32_t	prvDRV_GPIO_Pin_GetModeCode(drv_gpio_pin_mode_t mode)
{
	switch(mode)
	{
	case	DRV_GPIO_PIN_MODE_INPUT:
		return GPIO_MODE_INPUT;
	case	DRV_GPIO_PIN_MODE_OUTPUT_PP:
		return GPIO_MODE_OUTPUT_PP;
	case	DRV_GPIO_PIN_MODE_OUTPUT_OD:
		return GPIO_MODE_OUTPUT_OD;
	case	DRV_GPIO_PIN_MODE_AF_PP:
		return GPIO_MODE_AF_PP;
	case	DRV_GPIO_PIN_MODE_AF_OD:
		return GPIO_MODE_AF_OD;
	case	DRV_GPIO_PIN_MODE_ANALOG:
		return GPIO_MODE_ANALOG;
	case	DRV_GPIO_PIN_MODE_IT_RISING:
		return GPIO_MODE_IT_RISING;
	case	DRV_GPIO_PIN_MODE_IT_FALLING:
		return GPIO_MODE_IT_FALLING;
	case	DRV_GPIO_PIN_MODE_IT_RISING_FALLING:
		return GPIO_MODE_IT_RISING_FALLING;
	case	DRV_GPIO_PIN_MODE_EVT_RISING:
		return GPIO_MODE_EVT_RISING;
	case	DRV_GPIO_PIN_MODE_EVT_FALLING:
		return GPIO_MODE_EVT_FALLING;
	case	DRV_GPIO_PIN_MODE_EVT_RISING_FALLING:
		return GPIO_MODE_EVT_RISING_FALLING;
	}
}

drv_gpio_status_t DRV_GPIO_Init()
{
	memset(prvDRV_GPIO_PORTS, 0, DRV_GPIO_PORT_MAX_NUMBER*sizeof(drv_gpio_port_handle_t));
	return DRV_GPIO_STATUS_OK;
}

drv_gpio_status_t DRV_GPIO_Port_Init(drv_gpio_port_t port)
{
	if(prvDRV_GPIO_PORTS[port].lock != NULL) return DRV_GPIO_STATUS_ERROR;

	prvDRV_GPIO_PORTS[port].lock = xSemaphoreCreateMutex();

	if(prvDRV_GPIO_PORTS[port].lock == NULL)  return DRV_GPIO_STATUS_ERROR;

	prvDRV_GPIO_PORTS[port].initState = DRV_GPIO_PORT_INIT_STATUS_INITIALIZED;

	switch(port)
	{
	case DRV_GPIO_PORT_A:
		  __HAL_RCC_GPIOA_CLK_ENABLE();
		  prvDRV_GPIO_PORTS[port].portInstance = GPIOA;
		break;
	case DRV_GPIO_PORT_B:
		  __HAL_RCC_GPIOB_CLK_ENABLE();
		  prvDRV_GPIO_PORTS[port].portInstance = GPIOB;
		break;
	case DRV_GPIO_PORT_C:
		  __HAL_RCC_GPIOC_CLK_ENABLE();
		  prvDRV_GPIO_PORTS[port].portInstance = GPIOC;
		break;
	case DRV_GPIO_PORT_D:
		  __HAL_RCC_GPIOD_CLK_ENABLE();
		  prvDRV_GPIO_PORTS[port].portInstance = GPIOD;
		break;
	case DRV_GPIO_PORT_E:
		  __HAL_RCC_GPIOE_CLK_ENABLE();
		  prvDRV_GPIO_PORTS[port].portInstance = GPIOE;
		break;
	case DRV_GPIO_PORT_F:
		  __HAL_RCC_GPIOF_CLK_ENABLE();
		  prvDRV_GPIO_PORTS[port].portInstance = GPIOF;
		break;
	case DRV_GPIO_PORT_G:
		  __HAL_RCC_GPIOG_CLK_ENABLE();
		  prvDRV_GPIO_PORTS[port].portInstance = GPIOG;
		break;
	case DRV_GPIO_PORT_H:
		  __HAL_RCC_GPIOH_CLK_ENABLE();
		  prvDRV_GPIO_PORTS[port].portInstance = GPIOH;
		break;
	default:
		return DRV_GPIO_STATUS_ERROR;
		break;
	}

	return DRV_GPIO_STATUS_OK;
}


drv_gpio_status_t DRV_GPIO_Pin_Init(drv_gpio_port_t port, drv_gpio_pin pin, drv_gpio_pin_init_conf_t* conf)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(prvDRV_GPIO_PORTS[port].initState != DRV_GPIO_PORT_INIT_STATUS_INITIALIZED || prvDRV_GPIO_PORTS[port].lock == NULL) return DRV_GPIO_STATUS_ERROR;
	if(pin > DRV_GPIO_PIN_MAX_NUMBER) return DRV_GPIO_STATUS_ERROR;

	if(xSemaphoreTake(prvDRV_GPIO_PORTS[port].lock, portMAX_DELAY) == pdFALSE ) return DRV_GPIO_STATUS_ERROR;

	GPIO_InitStruct.Pin  = 0x0001 << pin;
	GPIO_InitStruct.Mode = prvDRV_GPIO_Pin_GetModeCode(conf->mode);
	GPIO_InitStruct.Pull = conf->pullState;
	HAL_GPIO_Init((GPIO_TypeDef*)prvDRV_GPIO_PORTS[port].portInstance, &GPIO_InitStruct);

	if(xSemaphoreGive(prvDRV_GPIO_PORTS[port].lock) == pdFALSE ) return DRV_GPIO_STATUS_ERROR;

	return DRV_GPIO_STATUS_OK;
}

drv_gpio_status_t DRV_GPIO_Pin_SetState(drv_gpio_port_t port, drv_gpio_pin pin, drv_gpio_pin_state_t state)
{
	if(prvDRV_GPIO_PORTS[port].initState != DRV_GPIO_PORT_INIT_STATUS_INITIALIZED || prvDRV_GPIO_PORTS[port].lock == NULL) return DRV_GPIO_STATUS_ERROR;
	if(pin > DRV_GPIO_PIN_MAX_NUMBER) return DRV_GPIO_STATUS_ERROR;

	if(xSemaphoreTake(prvDRV_GPIO_PORTS[port].lock, portMAX_DELAY) == pdFALSE ) return DRV_GPIO_STATUS_ERROR;

	HAL_GPIO_WritePin((GPIO_TypeDef*)prvDRV_GPIO_PORTS[port].portInstance, 1 << pin, state);

	if(xSemaphoreGive(prvDRV_GPIO_PORTS[port].lock) == pdFALSE ) return DRV_GPIO_STATUS_ERROR;

	return DRV_GPIO_STATUS_OK;
}


drv_gpio_status_t DRV_GPIO_Pin_EnableInt(drv_gpio_port_t port, drv_gpio_pin pin, drv_gpio_pin_isr_callback callback)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(prvDRV_GPIO_PORTS[port].initState != DRV_GPIO_PORT_INIT_STATUS_INITIALIZED || prvDRV_GPIO_PORTS[port].lock == NULL) return DRV_GPIO_STATUS_ERROR;
	if(pin > DRV_GPIO_PIN_MAX_NUMBER) return DRV_GPIO_STATUS_ERROR;

	if(xSemaphoreTake(prvDRV_GPIO_PORTS[port].lock, portMAX_DELAY) == pdFALSE ) return DRV_GPIO_STATUS_ERROR;

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = 1 << pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init((GPIO_TypeDef*)prvDRV_GPIO_PORTS[port].portInstance, &GPIO_InitStruct);

	if(xSemaphoreGive(prvDRV_GPIO_PORTS[port].lock) == pdFALSE ) return DRV_GPIO_STATUS_ERROR;

	return DRV_GPIO_STATUS_OK;
}

