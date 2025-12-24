/*
 * periph_conf.h
 *
 * Created on: Dec 21, 2024
 * Author: User
 */

#ifndef INC_PERIPH_CONF_H_
#define INC_PERIPH_CONF_H_

#include "main_app.h"

// Peripheral Initialization Prototypes
void TIM2_Init(void);
void TIM2_LowLevel_Config(void);

void UART2_Init(void);
void UART2_LowLevel_Config(void);

#endif /* INC_PERIPH_CONF_H_ */
