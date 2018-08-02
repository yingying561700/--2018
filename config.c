
#include <stdint.h>                         
#include <stdbool.h>
#include "config.h"

/* Change the Drive PWM */
//20170824 Ying Change 11:13
float Drive_PWM_Fre = 1500;               /* 1500Hz */
float Drive_PWM_Dead_Time =0;              /* us */
float Feedback_PWM_Fre =30000;