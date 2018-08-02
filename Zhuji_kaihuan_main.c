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
#include "uart1Drv.h"
#include "algorithm.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/
/* i.e. uint16_t <variable_name>; */
/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void)
{

    /* Configure the oscillator for the device */
    ConfigureOscillator();
    /* Initialize IO ports and peripherals */
    InitApp();
    Para_adjust();
    Sample_start();
    /* TODO <INSERT USER APPLICATION CODE HERE> */
    char str[10];
    while(1)
    {
        if(Flag)
        {
            PORTBbits.RB15 = 1;
            Delay_ms(1000);
            Filter();
            UART_printf_str("\n#############|    SenseX   |###############\n");
            for(I=0;I<Cur_I;I++)
            {
                sprintf(str,"%4d ",SenseX[I]-512);
                UART_printf_str(str);
            }
            UART_printf_str("\n#############|SenseX_Filter|###############\n");
            for(I=0;I<Cur_I;I++)
            {
                sprintf(str,"%4d ",SenseX_Filter[I]);
                UART_printf_str(str);
            }
            UART_printf_str("\n#############|    SenseY    |##############\n");
            for(I=0;I<Cur_I;I++)
            {
                sprintf(str,"%4d ",SenseY[I]-512);
                UART_printf_str(str);
            }
            UART_printf_str("\n#############|SenseY_Filter|###############\n");
            for(I=0;I<Cur_I;I++)
            {
                sprintf(str,"%4d ",SenseY_Filter[I]);
                UART_printf_str(str);
            }
            UART_printf_str("\n#############|    SenseZ    |##############\n");
            for(I=0;I<Cur_I;I++)
            {
                sprintf(str,"%4d ",SenseZ[I]-512);
                UART_printf_str(str);
            }
            UART_printf_str("\n#############|SenseZ_Filter|###############\n");
            for(I=0;I<Cur_I;I++)
            {
                sprintf(str,"%4d ",SenseZ_Filter[I]);
                UART_printf_str(str);
            }
            PORTBbits.RB15 = 0;
            Sample_start();
        }
    }
}
