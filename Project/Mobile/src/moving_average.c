#include "moving_average.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
	@brief Initialize the buffer to a certain depth
*/
void init_buffer(moving_average_buffer* buffer, int buffer_depth){
	buffer->data = (float*) malloc(sizeof(float)*buffer_depth);
	memset(buffer->data, 0, sizeof(buffer->data));
	buffer->index = 0;
	buffer->sum = 0;
	
}

/**
	@brief Add a new reading to the buffer specified in the parameters in a circular fashion
*/
void update_buffer(float new_reading, moving_average_buffer* m_a, int buffer_depth){
	int current_index=m_a->index % buffer_depth;
	float old_value=m_a->data[current_index];
	m_a->sum-=old_value;
	m_a->data[current_index]=new_reading;
	m_a->sum+=new_reading;
	m_a->index++;
}

