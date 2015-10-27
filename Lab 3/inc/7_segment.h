#ifndef __7_SEGMENT_H
#define __7_SEGMENT_H

#include "gpio.h"

void init_7_segment(void);
void test_7_segment(void);
void draw_number(double num); //Would like to be a float, but for some reason passing floats get zeroed
void refresh_7_segment(void);

#endif