/*
 * servo_struct.h
 *
 *  Created on: Nov 7, 2024
 *      Author: Kasutaja
 */
#include "stdint.h"

#ifndef INC_SERVO_STRUCT_H_
#define INC_SERVO_STRUCT_H_

typedef struct {
	  //uint8_t num_servos; 	// how many servos will recieve the command, starting at index 0, 0 to 10
	  uint16_t ddeg[5];  		// detsi-degrees to go to, must be 0 to 1800
	  uint16_t c_limits[5];		// current limits in mA, must be 0 to 10000
	  uint16_t time_ms[5];		// time to execute the movement, 0 to 65535 in 10*ms so 0 to 6 seconds.
	  uint8_t debug_enabled;	// set if you want echo messages
	  	  	  	  	  	  	  	// For value values less than 2, a value of 2 is automatically substituted.
	}UART_Rec;					// rx_data


typedef struct {
		uint16_t status;		// status code: 0 - ok, 1 - current limit breached, 2 - received struct is erroneous
		float currents[5];		// currents of all servos in A (float)
	}UART_Send;					// tx_data


void to_rec_struct(char* uart_rx_buffer, UART_Rec* rx_a);

void to_send_struct(uint8_t status, uint16_t* currents, UART_Send* tx_a);




#endif /* INC_SERVO_STRUCT_H_ */
