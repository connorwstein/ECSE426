#ifndef __ACCELEROMETER_H
#define __ACCELEROMETER_H

void init_7_segment(void);
void test_7_segment(void);
void draw_number(double num); //Would like to be a float, but for some reason passing floats get zeroed
void refresh_7_segment(void);

#endif