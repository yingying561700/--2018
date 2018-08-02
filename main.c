/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

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


#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <stdio.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp              */


int16_t main(void)
{

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();
    Delay_ms(1000);
    Sample_start();
    /* TODO <INSERT USER APPLICATION CODE HERE> */
    char str[10];
    while(1)
    {
        if(Flag)
        {
            UART_printf_str("\n##############|  Drive_PWM  |##############\n");
            for(I=0;I<Cur_I;I++)
            {
                sprintf(str,"%4d ",Drive_PWMH[I] - Drive_PWML[I]);
                UART_printf_str(str);
            }
            UART_printf_str("\n##############|Drive_Current|##############\n");
            for(I=0;I<Cur_I;I++)
            {
                sprintf(str,"%4d ",Drive_PWMH[I] - Drive_MID[I]);
                UART_printf_str(str);
            }
            Flag = 0;
        }
    }
}
