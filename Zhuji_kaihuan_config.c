/*File:   config.c
 * Author: wild
 * Created on 2015?11?29?, ??3:05*/
#include <stdint.h>                         
#include <stdbool.h>
#include "config.h"

/* Change the Drive PWM */
float Drive_PWM_Dead_Time = 0;             /* us */

uint16_t Filter_ParaX[8] = {18,58,59,99,100,140,141,181};
uint16_t Filter_ParaY[8] = {18,58,59,99,100,140,141,181};
uint16_t Filter_ParaZ[8] = {18,58,59,99,100,140,141,181};
float PID_Para[3] = {0.5,0,0};

/* My config variable */
float Feedback_PWM_Fre = 30000;
