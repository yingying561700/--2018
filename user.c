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
#include "user.h"            /* variables/params used by user.c               */
#include "config.h"
#include "adcdrv1.h"

/* <Initialize variables in user.h and insert code for user algorithms.> */
uint8_t Flag = 0;
//int SenseX,SenseY,SenseZ,Temp;
int Drive_PWML[LENGTH],Drive_MID[LENGTH],Drive_PWMH[LENGTH];
uint16_t I,Cur_I;

void Delay_ms(uint32_t ms)
{
    ms = 10*ms;
    while(ms--)
        DELAY_100uS;
}

void Init_uart()
{
    U1MODEbits.STSEL = 0;                       // 1-Stop bit
    U1MODEbits.PDSEL = 0;                       // No Parity, 8-Data bits
    U1MODEbits.ABAUD = 0;                       // Auto-Baud disabled
    U1MODEbits.BRGH = 0;                        // Standard-Speed mode
    U1BRG =21;                                   // Set baud-rate  40M/(16*(21+1))   112500
    U1MODEbits.UARTEN = 1;                      // Enable UART
    U1STAbits.UTXEN = 1;                        // Enable UART TX  
}

void Init_pwm()
{
    //Set PWM1 as push-pull mode to drive the Magnetism Core
    PTCON2bits.PCLKDIV = 1;  //2fen pin
    PWMCON1bits.ITB = 1;
    PHASE1 = 1e9/(Drive_PWM_Fre*25);
    PDC1 = PHASE1/2;   
    
    DTR1 = 1000*Drive_PWM_Dead_Time/25;                                                            
    ALTDTR1 = DTR1;                                                  
    IOCON1bits.PENH = 1;                        /* PWM1H output controlled by PWM */
    IOCON1bits.PENL = 1;                        /* PWM1L output controlled by PWM */
    IOCON1bits.PMOD = 0;                        /* Select  the Complementary Output mode */
    FCLCON1bits.FLTMOD = 3; 
    //Set PWM1 trigger
    TRIG1 = 0;
    TRIG1 = 0;
    TRGCON1bits.TRGDIV = 0;             //control Fcn = Fpwm;
    TRGCON1bits.TRGSTRT = 0;
    PWMCON1bits.TRGIEN = 0;
    _PWM1IF = 0;
    _PWM1IE = 1;
    
    //PWM2 for feedback1
    PTPER = FB_PWM_VAL;
    PDC2 = PTPER/2;                                                                   
    IOCON2bits.PENH = 1;                        /* PWM2H output controlled by PWM */
    IOCON2bits.PENL = 1;                    
    IOCON2bits.PMOD = 0;                        /* Select True Independen PWM mode */
    FCLCON2bits.FLTMOD = 3; 
 
    PDC3 = PTPER/2;                                                                   
    IOCON3bits.PENH = 0;                        /* PWM2H output controlled by PWM */
    IOCON3bits.PENL = 1;                    
    IOCON3bits.PMOD = 0;                        /* Select True Independen PWM mode */
    FCLCON3bits.FLTMOD = 3; 
    
    PTCONbits.PTEN = 1;                         /* Turn on PWM module */
}

void Init_int1()
{
    //RPINR0 = 0b0100111;                         /* Set pin16 as INT1 pin */
    CNENAbits.CNIEA2 = 1;
    _CNIE = 1;
    _CNIF = 0;
}

void InitApp(void)
{
    /* TODO Initialize User Ports/Peripherals/Project here */
    RPOR1bits.RP36R = 0b000001;                  /* Set pin11 as U1TX2 */
    
    /* Setup analog functionality and port direction */
    ANSELA = ANSELB = 0;
    TRISAbits.TRISA2 = 0;
    //PORTAbits.RA2 = 0;
 
    Init_uart();
    Init_pwm();
    Init_adc1();
    Init_timer3();
//    Init_int1();
//    InitDma0();
    
}

void UART_print_byte( char c )
{
    U1TXREG = c;
    while(U1STAbits.UTXBF);
}

void UART_printf_str( char * str )
{
    while(*str)
    {
        U1TXREG = *str++;
        while(U1STAbits.UTXBF);
    }
}

void Sample_start()
{
    Flag = 0;
    I = 0;
    //T3CONbits.TON = 1;
    PWMCON1bits.TRGIEN = 1;
}

void Sample_stop()
{
    Flag = 1;
    Cur_I = I;
    T3CONbits.TON = 0;
    PWMCON1bits.TRGIEN = 0;
}