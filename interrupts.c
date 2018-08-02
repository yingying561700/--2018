/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__dsPIC33E__)
    	#include <p33Exxxx.h>
    #elif defined(__dsPIC33F__)
    	#include <p33Fxxxx.h>
    #endif
#endif

#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>       /* Includes true/false definition */
#include <stdio.h>
#include "config.h"
#include "user.h"

 // PWM1 Trigger ISR
 uint8_t sample_count = 0;
 void __attribute__((interrupt,no_auto_psv)) _PWM1Interrupt(void)               
{        
    PORTAbits.RA2 = ~PORTAbits.RA2;
    sample_count++;
    if(sample_count == 1)
    {
        T3CONbits.TON = 1;
    }
    if(sample_count == 2)
    {
        sample_count = 0;
        Sample_stop();
    }
     _PWM1IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void)
{
    Drive_PWML[I] = ADC1BUF1;
    Drive_MID[I] = ADC1BUF2;
    Drive_PWMH[I] = ADC1BUF3;
    I++;
    _AD1IF = 0; 
}

 
