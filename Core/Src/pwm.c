/*
 * pwm.c
 *
 *  Created on: Dec 17, 2024
 *      Author: Kasutaja
 */

#include <stdio.h>
#include "tim.h"
#include "pwm.h"
#include "servo_struct.h"
#include "adc_reading.h"
#include "uart_tr.h"

const uint16_t PWM_max = 15000;
const uint16_t PWM_neutral = 9000;
const uint16_t PWM_min = 3000;

uint16_t J1_PWM;
uint16_t J2_PWM;
uint16_t J3_PWM;
uint16_t J4_PWM;
uint16_t J5_PWM;

uint16_t J1_PWM_target;
uint16_t J2_PWM_target;
uint16_t J3_PWM_target;
uint16_t J4_PWM_target;
uint16_t J5_PWM_target;

int16_t J1_PWM_increment;
int16_t J2_PWM_increment;
int16_t J3_PWM_increment;
int16_t J4_PWM_increment;
int16_t J5_PWM_increment;

void PWM_var_init(){
	J1_PWM = PWM_neutral; 	// timer 3 ch 1
	J2_PWM = PWM_neutral;	// timer 3 ch 2
	J3_PWM = PWM_neutral;	// timer 3 ch 3
	J4_PWM = PWM_neutral;	// timer 4 ch 1
	J5_PWM = PWM_neutral; 	// timer 4 ch 2

	J1_PWM_target = PWM_neutral;
	J2_PWM_target = PWM_neutral;
	J3_PWM_target = PWM_neutral;
	J4_PWM_target = PWM_neutral;
	J5_PWM_target = PWM_neutral;

	J1_PWM_increment = 0;
	J2_PWM_increment = 0;
	J3_PWM_increment = 0;
	J4_PWM_increment = 0;
	J5_PWM_increment = 0;

	// set timer OC values
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, J1_PWM);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, J2_PWM);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, J3_PWM);
	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, J4_PWM);
	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, J5_PWM);

}


void Update_PWM_status(UART_Rec* rx_data){
	//refresh period in ms
	const int16_t time_const = 10;

	/*
	if(rx_data.pwm_oc > PWM_max){
		rx_data.pwm_oc = PWM_max;
	}
	if(rx_data.pwm_oc < PWM_min){
		rx_data.pwm_oc = PWM_min;
	}

	if(rx_data.joint > 5) return;
	if(rx_data.joint < 1) return;
	*/


	//if(rx_data.time_ms < 2) rx_data.time_ms = 2;

	J1_PWM_target = (uint16_t)12000/1800*rx_data->ddeg[0]+3000;
	J2_PWM_target = (uint16_t)12000/1800*rx_data->ddeg[1]+3000;
	J3_PWM_target = (uint16_t)12000/1800*rx_data->ddeg[2]+3000;
	J4_PWM_target = (uint16_t)12000/1800*rx_data->ddeg[3]+3000;
	J5_PWM_target = (uint16_t)12000/1800*rx_data->ddeg[4]+3000;
	// find difference and divide by ms / 10, set as increment
	// rest is implemented in timer interrupt (every 10 ms)
	if(J1_PWM != J1_PWM_target){
		int16_t diff = J1_PWM_target - J1_PWM;
		if(rx_data->time_ms[0] < 10) rx_data->time_ms[0]=10;
		J1_PWM_increment = (diff*time_const)/(int16_t)rx_data->time_ms[0];
	}

	if(J2_PWM != J2_PWM_target){
		int16_t diff = J2_PWM_target - J2_PWM;
		if(rx_data->time_ms[1] < 10) rx_data->time_ms[1]=10;
		J2_PWM_increment = (diff*time_const)/(int16_t)rx_data->time_ms[1];
	}

	if(J3_PWM != J3_PWM_target){
		int16_t diff = J3_PWM_target - J3_PWM;
		if(rx_data->time_ms[2] < 10) rx_data->time_ms[2]=10;
		J3_PWM_increment = (diff*time_const)/(int16_t)rx_data->time_ms[2];
	}

	if(J4_PWM != J4_PWM_target){
		int16_t diff = J4_PWM_target - J4_PWM;
		if(rx_data->time_ms[3] < 10) rx_data->time_ms[3]=10;
		J4_PWM_increment = (diff*time_const)/(int16_t)rx_data->time_ms[3];
	}

	if(J5_PWM != J5_PWM_target){
		int16_t diff = J5_PWM_target - J5_PWM;
		if(rx_data->time_ms[4] < 10) rx_data->time_ms[4]=10;
		J5_PWM_increment = (diff*time_const)/(int16_t)rx_data->time_ms[4];
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	__disable_irq();

	get_adc_values(adc_values);
	currents_calculate_status(&rec_data, &tx_data, adc_values);

	if(htim == &htim2 && tx_data.status == 0){
		HAL_GPIO_TogglePin(GPIOC, 0x0200);

		if(J1_PWM != J1_PWM_target){
			J1_PWM += J1_PWM_increment;

			if(J1_PWM_increment < 0 && J1_PWM <= J1_PWM_target){
				J1_PWM = J1_PWM_target;
				J1_PWM_increment = 0;
			}

			if(J1_PWM_increment > 0 && J1_PWM >= J1_PWM_target){
				J1_PWM = J1_PWM_target;
				J1_PWM_increment = 0;
			}

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, J1_PWM);

		}

		if(J2_PWM != J2_PWM_target){
			J2_PWM += J2_PWM_increment;

			if(J2_PWM_increment < 0 && J2_PWM <= J2_PWM_target){
				J2_PWM = J2_PWM_target;
				J2_PWM_increment = 0;
			}

			if(J2_PWM_increment > 0 && J2_PWM >= J2_PWM_target){
				J2_PWM = J2_PWM_target;
				J2_PWM_increment = 0;
			}

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, J2_PWM);

		}

		if(J3_PWM != J3_PWM_target){
			J3_PWM += J3_PWM_increment;

			if(J3_PWM_increment < 0 && J3_PWM <= J3_PWM_target){
				J3_PWM = J3_PWM_target;
				J3_PWM_increment = 0;
			}

			if(J3_PWM_increment > 0 && J3_PWM >= J3_PWM_target){
				J3_PWM = J3_PWM_target;
				J3_PWM_increment = 0;
			}

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, J3_PWM);

		}

		if(J4_PWM != J4_PWM_target){
			J4_PWM += J4_PWM_increment;

			if(J4_PWM_increment < 0 && J4_PWM <= J4_PWM_target){
				J4_PWM = J4_PWM_target;
				J4_PWM_increment = 0;
			}

			if(J4_PWM_increment > 0 && J4_PWM >= J4_PWM_target){
				J4_PWM = J4_PWM_target;
				J4_PWM_increment = 0;
			}

			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, J4_PWM);

		}

		if(J5_PWM != J5_PWM_target){
			J5_PWM += J5_PWM_increment;

			if(J5_PWM_increment < 0 && J5_PWM <= J5_PWM_target){
				J5_PWM = J5_PWM_target;
				J5_PWM_increment = 0;
			}

			if(J5_PWM_increment > 0 && J5_PWM >= J5_PWM_target){
				J5_PWM = J5_PWM_target;
				J5_PWM_increment = 0;
			}

			__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, J5_PWM);

		}
	}
	else{
		J1_PWM_target = J1_PWM;
		J1_PWM_increment = 0;

		J2_PWM_target = J2_PWM;
		J2_PWM_increment = 0;

		J3_PWM_target = J3_PWM;
		J3_PWM_increment = 0;

		J4_PWM_target = J4_PWM;
		J4_PWM_increment = 0;

		J5_PWM_target = J5_PWM;
		J5_PWM_increment = 0;
	}

	__enable_irq();
}
