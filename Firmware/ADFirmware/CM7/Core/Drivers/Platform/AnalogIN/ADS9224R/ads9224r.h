/*
 * ads9224r.h
 *
 *  Created on: Oct 7, 2024
 *      Author: Haris
 */

#ifndef CORE_DRIVERS_PLATFORM_ANALOGIN_ADS9224R_ADS9224R_H_
#define CORE_DRIVERS_PLATFORM_ANALOGIN_ADS9224R_ADS9224R_H_

typedef enum
{
	ADS9224R_STATUS_OK = 0,
	ADS9224R_STATUS_ERROR
}ads9224r_status_t;

#define 			ADS9224R_READY_STROBE_PORT
#define 			ADS9224R_READY_STROBE_PIN
#define 			ADS9224R_RESET_PD_PORT
#define 			ADS9224R_RESET_PD_PIN
#define 			ADS9224R_CONVST_PORT
#define 			ADS9224R_CONVST_PIN

ads9224r_status_t	ADS9224R_Init();

#endif /* CORE_DRIVERS_PLATFORM_ANALOGIN_ADS9224R_ADS9224R_H_ */