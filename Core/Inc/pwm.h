/*
 * pwm.h
 *
 *  Created on: Dec 17, 2024
 *      Author: Kasutaja
 */

#include "uart_tr.h"
#include "servo_struct.h"

#ifndef INC_PWM_H_
#define INC_PWM_H_

void PWM_var_init();
void Update_PWM_status(UART_Rec* rx_data);

#endif /* INC_PWM_H_ */
