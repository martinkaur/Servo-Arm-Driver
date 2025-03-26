/*
 * uart_tr.c
 *
 *  Created on: Dec 17, 2024
 *      Author: Kasutaja
 */

#include "usart.h"
#include "main.h"
#include "uart_tr.h"
#include "pwm.h"
#include "stdio.h"
#include "servo_struct.h"
#include "adc_reading.h"


// set if build debug messages are enabled.

uint8_t pikkus = 32;
char puhver[100];
char puhver1[32];
char puhverST[100];
uint16_t adc_values[5] = {0, 0, 0, 0, 0};

//Uart_Data rx_data;

UART_Rec rec_data;
UART_Send tx_data;

// num of bytes = 1 initially, after first byte is num_servos*4+2
// servo number default is 0;
void init_uart_rx_interrupt(uint8_t num_bytes){
	HAL_UART_Receive_IT(&huart1, (uint8_t*)puhver1, num_bytes);
}

// HALi UARTi recieve complete pöördumine, override
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){
	__disable_irq();

	tx_data.status=0;

	to_rec_struct(puhver1, &rec_data);

	int len;

	if(rec_data.debug_enabled != 0){
		len = snprintf(puhver, 99, "ECHO ddeg: 1-%04u, 2-%04u, 3-%04u, 4-%04u, 5-%04u\n",
				rec_data.ddeg[0], rec_data.ddeg[1], rec_data.ddeg[2], rec_data.ddeg[3], rec_data.ddeg[4]);
		if(len > 100) len = 100;
		if(len < 0) len = 0;
		HAL_GPIO_WritePin(GPIOC, 0x0800, 1);
		while(HAL_UART_Transmit(&huart1, (const uint8_t*)puhver, len, 1000));

		len = snprintf(puhver, 99, "ECHO I_lim: 1-%05u, 2-%05u, 3-%05u, 4-%05u, 5-%05u\n",
				rec_data.c_limits[0], rec_data.c_limits[1], rec_data.c_limits[2], rec_data.c_limits[3], rec_data.c_limits[4]);
		if(len > 100) len = 100;
		if(len < 0) len = 0;
		while(HAL_UART_Transmit(&huart1, (const uint8_t*)puhver, len, 1000));

		len = snprintf(puhver, 99, "ECHO time: 1-%05u, 2-%05u, 3-%05u, 4-%05u, 5-%05u\n",
				rec_data.time_ms[0], rec_data.time_ms[1], rec_data.time_ms[2], rec_data.time_ms[3], rec_data.time_ms[4]);
		if(len > 100) len = 100;
		if(len < 0) len = 0;

		while(HAL_UART_Transmit(&huart1, (const uint8_t*)puhver, len, 1000));
		HAL_GPIO_WritePin(GPIOC, 0x0400, 1);
	}


	for(uint8_t i=0; i<100; i++){
		puhver1[i]='\0';
	}

	Update_PWM_status(&rec_data);
	HAL_GPIO_WritePin(GPIOC, 0x0800, 0);

	// needed for re-enabing
	HAL_UART_Receive_IT(&huart1, (uint8_t*)puhver1, pikkus);

	get_adc_values(adc_values);
	calculate_currents(&tx_data, adc_values);
	currents_calculate_status(&rec_data, &tx_data, adc_values);

	len = snprintf(puhverST, 99, "ST: %01u, Currents(A): 1: %.04f, 2: %.04f, 3: %.04f, 4: %.04f, 5: %.04f \n",
			tx_data.status, tx_data.currents[0], tx_data.currents[1], tx_data.currents[2], tx_data.currents[3], tx_data.currents[4]);

	if(len > 100) len = 100;
	if(len > 0) uart_send_status(puhverST, len);

	__enable_irq();
	HAL_GPIO_WritePin(GPIOC, 0x0400, 0);
}

void uart_send_status(char* puhver, int len){
	while(HAL_UART_Transmit(&huart1, (const uint8_t *)puhver, len, 1000));
}
