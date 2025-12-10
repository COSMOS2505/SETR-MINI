#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

void temp_sensor_init(void);

double temp_sensor_init(void);
int temp_sensor_read_filter(double *temp_out);

#endif // TEMP_SENSOR_H

