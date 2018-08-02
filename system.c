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


#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */

#include "system.h"          /* variables/params used by system.c             */

/******************************************************************************/
/* System Level Functions                                                     */
/*                                                                            */
/* Custom oscillator configuration funtions, reset source evaluation          */
/* functions, and other non-peripheral microcontroller initialization         */
/* functions get placed in system.c.                                          */
/*                                                                            */
/******************************************************************************/

/* Refer to the device Family Reference Manual Oscillator section for
information about available oscillator configurations.  Typically
this would involve configuring the oscillator tuning register or clock
switching useing the compiler's __builtin_write_OSCCON functions.
Refer to the C Compiler for PIC24 MCUs and dsPIC DSCs User Guide in the
compiler installation directory /doc folder for documentation on the
__builtin functions.*/

/* TODO Add clock switching code if appropriate.  An example stub is below.   */
void ConfigureOscillator(void)
{
    /* Disable Watch Dog Timer */
    RCONbits.SWDTEN = 0;

    /* Configure Oscillator to operate the device at 60Mhz
    Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
    Fosc= 7.37*65/(2*3)=80Mhz for Fosc, Fcy = 40Mhz */

    /* Configure PLL prescaler, PLL postscaler, PLL divisor */
   
    PLLFBD = 63;                            /* M = PLLFBD + 2 */
    CLKDIVbits.PLLPOST = 0;                 /* N1 = 2*(0+1) = 2 */
    CLKDIVbits.PLLPRE = 1;                  /* N2 = 1+2 = 3 */
    CLKDIVbits.DOZE = 0;                    /* Fcy = Fp */  
    __builtin_write_OSCCONH(0x01);			/* New Oscillator FRC w/ PLL */
    __builtin_write_OSCCONL(0x01);  		/* Enable Switch */
    
    while(OSCCONbits.COSC != 0b001);		/* Wait for new Oscillator to become FRC w/ PLL */  
    while(OSCCONbits.LOCK != 1);			/* Wait for Pll to Lock */  
}

