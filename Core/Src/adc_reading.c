/*
 * adc_reading.c
 *
 *  Created on: Feb 11, 2025
 *      Author: Kasutaja
 */

#include "adc.h"
#include "servo_struct.h"
#include "adc_reading.h"

uint16_t adc1_values[4] = {0, 0, 0, 0};
uint16_t adc2_values[1] = {0};

void get_adc_values(uint16_t* adc_values){

	for(uint8_t i=0; i<4; i++){
		adc_values[i] = (uint16_t)(adc1_values[i]);
	}
	adc_values[4] = (uint16_t)adc2_values[0];


	HAL_ADC_Start_DMA(&hadc1, adc1_values, 4);
	HAL_ADC_Start_DMA(&hadc2, adc2_values, 1);

}

void calculate_currents(UART_Send* servo_info, uint16_t* adc_values){
	servo_info->currents[0] = (float)(adc_values[0]-3230)/241.8f;
	servo_info->currents[1] = (float)(adc_values[1]-3230)/241.8f;
	servo_info->currents[2] = (float)(adc_values[2]-3230)/241.8f;
	servo_info->currents[3] = (float)(adc_values[3]-3230)/241.8f;
	servo_info->currents[4] = (float)(adc_values[4]-3230)/241.8f;
}

void currents_calculate_status(UART_Rec* servo_input, UART_Send* servo_info, uint16_t* adc_values){

	uint16_t current_mA = (uint16_t)((adc_values[0]-3230)/241.8f) * 1000;
	if(servo_input->c_limits[0] < (current_mA))servo_info->status=1;

	current_mA = (uint16_t)((adc_values[1]-3230)/241.8f)*1000;
	if(servo_input->c_limits[1] < (current_mA))servo_info->status=1;

	current_mA = (uint16_t)((adc_values[2]-3230)/241.8f)*1000;
	if(servo_input->c_limits[2] < (current_mA))servo_info->status=1;

	current_mA = (uint16_t)((adc_values[3]-3230)/241.8f)*1000;
	if(servo_input->c_limits[3] < (current_mA))servo_info->status=1;

	current_mA = (uint16_t)((adc_values[4]-3230)/241.8f)*1000;
	if(servo_input->c_limits[4] < (current_mA))servo_info->status=1;


}
