
extern float Drive_PWM_Dead_Time; 
extern float Feedback_PWM_Fre;
extern uint16_t Filter_ParaX[8],Filter_ParaY[8],Filter_ParaZ[8]; 
extern float PID_Para[3];

extern float TOffset,TScale,FcosXY,FcosXZ,FcosYX,FcosYZ,FcosZX,FcosZY;
extern float XScale,XGain,YScale,YGain,ZScale,ZGain;
extern float FXSetoff[4],FYSetoff[4],FZSetoff[4],FXScale[4],FYScale[4],FZScale[4];
#define FB_PWM_VAL (1e9/(Feedback_PWM_Fre*12.5))

#define TRIAXIAL 