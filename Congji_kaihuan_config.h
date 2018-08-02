extern float Drive_PWM_Fre;
extern float Drive_PWM_Dead_Time; 
extern float Feedback_PWM_Fre;

#define FB_PWM_VAL ((4*1e9)/(Feedback_PWM_Fre*25))
