#ifndef __MOVING_AVERAGE_H
#define __MOVING_AVERAGE_H

typedef struct{
	float *data;
	int index;
	float sum;
} moving_average_buffer;

void init_buffer(moving_average_buffer* buffer, int buffer_depth);
void update_buffer(float new_reading, moving_average_buffer* m_a, int buffer_depth);

#endif