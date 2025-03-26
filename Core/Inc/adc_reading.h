/*
 * adc_reading.h
 *
 *  Created on: Feb 11, 2025
 *      Author: Kasutaja
 */

#ifndef INC_ADC_READING_H_
#define INC_ADC_READING_H_

#include <stdint.h>
#include "servo_struct.h"

extern uint16_t adc1_values[4];
extern uint16_t adc2_values[1];

void get_adc_values(uint16_t* adc_values);
void calculate_currents(UART_Send* servo_info, uint16_t* adc_values);
void currents_calculate_status(UART_Rec* servo_input, UART_Send* servo_info, uint16_t* adc_values);

#endif /* INC_ADC_READING_H_ */
