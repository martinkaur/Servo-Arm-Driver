/*
 * servo_struct.c
 *
 *  Created on: Nov 7, 2024
 *      Author: Kasutaja
 */

#include "servo_struct.h"
#include "stdint.h"


// called after first byte is received separately
void to_rec_struct(char* uart_rx_buffer, UART_Rec* p_rx_data){

	uint8_t n = 5; //p_rx_data->num_servos;
	uint8_t a = 0;
	uint8_t b = 1;

	for(uint8_t i=0; i<n; i++){
		// bitshift selline sest windows happened
		p_rx_data->ddeg[i] = (uint16_t)(((uint16_t)uart_rx_buffer[a]) + ((uint16_t)uart_rx_buffer[b] << 8)); // bytes [0, 9] if n=5
		p_rx_data->c_limits[i] = (uint16_t)(((uint16_t)uart_rx_buffer[2*n+a]) + ((uint16_t)uart_rx_buffer[2*n+b] << 8)); // [10, 19]
		p_rx_data->time_ms[i] = (uint16_t)(((uint16_t)uart_rx_buffer[4*n+a]) + ((uint16_t)uart_rx_buffer[4*n+b] << 8)); //[20, 29]
		a += 2;
		b += 2;
	}

	p_rx_data->debug_enabled = (uint8_t)uart_rx_buffer[30];
}

void to_send_struct(uint8_t status, uint16_t* currents, UART_Send* p_tx_data){
	p_tx_data->status = status;
	for(uint8_t i = 0; i<5; i++){
		p_tx_data->currents[i] = currents[i];
	}

}


