/*
 * uart_tr.h
 *
 *  Created on: Dec 17, 2024
 *      Author: Kasutaja
 */
#include "stdint.h"
#include "servo_struct.h"

#ifndef INC_UART_TR_H_
#define INC_UART_TR_H_

//to be deprecated:
typedef struct{
	  uint8_t joint;
	  uint16_t pwm_oc;
	  uint16_t time_ms;
	}Uart_Data;

extern UART_Rec rec_data;
extern UART_Send tx_data;
extern uint16_t adc_values[5];

void init_uart_rx_interrupt(uint8_t num_bytes);
void uart_send_status(char* puhver, int len);

#endif /* INC_UART_TR_H_ */
