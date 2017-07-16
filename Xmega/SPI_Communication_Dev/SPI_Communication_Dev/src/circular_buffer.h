/*
 * circular_buffer.h
 *
 * Created: 7/16/2017 10:43:02 AM
 *  Author: rgw3d
 */ 


#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_
#include <asf.h>
//
#define MAX_BUFFER_SIZE 50

typedef struct circular_buffer
{
	uint8_t buffer[MAX_BUFFER_SIZE];
	uint8_t front;
	uint8_t back;
	uint8_t len;
} circular_buffer_t;

void circular_buffer_push(circular_buffer_t * cb, uint8_t data);

uint8_t circular_buffer_pop(circular_buffer_t * cb);

extern uint8_t circular_buffer_size(circular_buffer_t * cb);


#endif /* CIRCULAR_BUFFER_H_ */