#ifndef RTDB_H
#define RTDB_H

#include <zephyr/kernel.h>

void rtdb_init(void);
void rtdb_set_system_on(bool on);
void rtdb_toggle_system_on(void);
void rtdb_increment_target_temp(void);
void rtdb_decrement_target_temp(void);
void rtdb_set_current_temp(double temp);


// Leituras seguras sem intereferência das variáveis globais
bool rtdb_get_system_on(void);
double rtdb_get_target_temp(void);
double rtdb_get_current_temp(void);

#endif