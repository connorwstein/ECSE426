#ifndef TEMPERATURE_H
#define TEMPERATURE_H

void initialize_ADC_Temp(void);
void store_temperature_in_buffer(void);
float get_average_temperature(void);

#endif