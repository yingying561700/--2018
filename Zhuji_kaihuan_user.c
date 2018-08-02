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
#include "uart1Drv.h"

/* <Initialize variables in user.h and insert code for user algorithms.> */
uint8_t Flag = 0,Flag1 = 0;
int SenseX[LENGTH],SenseY[LENGTH],SenseZ[LENGTH],Temp[LENGTH]; 
int SenseX_Filter[LENGTH],SenseY_Filter[LENGTH],SenseZ_Filter[LENGTH];
int SenseX_Mid,SenseY_Mid,SenseZ_Mid,SenseX_Val,SenseY_Val,SenseZ_Val;
uint16_t I,Cur_I;
uint16_t charI = 0,Cur_charI = 0;

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
    U1BRG = 21;                                   // Set baud-rate  40M/(16*(21+1))   112500
    U1MODEbits.UARTEN = 1;                      // Enable UART
//    U1STAbits.UTXEN = 1;                        // Enable UART TX  
    U1STAbits.URXISEL = 0b00;                   
//    IEC4bits.U1EIE = 0;
    _U1RXIE = 1;
    _U1RXIF = 0;
    
    U2MODEbits.STSEL = 0;                       // 1-Stop bit
    U2MODEbits.PDSEL = 0;                       // No Parity, 8-Data bits
    U2MODEbits.ABAUD = 0;                       // Auto-Baud disabled
    U2MODEbits.BRGH = 0;                        // Standard-Speed mode
    U2BRG = 9;                                   // Set baud-rate  40M/(16*(9+1))   250000
    U2MODEbits.UARTEN = 1;                      // Enable UART
    U2STAbits.UTXEN = 1;                        // Enable UART TX                    
}

void Init_pwm()
{
     //PWM1 for feedbackX
    PTPER = FB_PWM_VAL;
    PDC1 = PTPER/2;                                                                   
    IOCON1bits.PENH = 1;                        /* PWM1H output controlled by PWM */
    IOCON1bits.PENL = 0;                    
    IOCON1bits.PMOD = 0;                        
    FCLCON1bits.FLTMOD = 3; 
    
    //PWM2 for feedbackY
    PDC2 = PTPER/2;                                                                   
    IOCON2bits.PENH = 1;                        /* PWM2H output controlled by PWM */
    IOCON2bits.PENL = 0;                    
    IOCON2bits.PMOD = 0;                        
    FCLCON2bits.FLTMOD = 3; 
    //PWM3 for feedbackZ
    PDC3 = PTPER/2;                                                                        
    IOCON3bits.PENH = 1;                        /* PWM3H output controlled by PWM */
    IOCON3bits.PENL =0;                         /* PWM3L output controlled by PWM */
    IOCON3bits.PMOD = 0;                       
    FCLCON3bits.FLTMOD = 3; 
 
//    PTCONbits.PTEN = 1;                         /* Turn on PWM module */
    PTCONbits.PTEN = 0;                         /* Turn off PWM module */
}

void Init_int1()
{
//    RPINR0 = 0b0100111;                         /* Set RP39 pin16 as INT1 pin */
//ying changed on 2018 05 02
    RPINR0=0b0010010;        /* Set RPI18(pin9)as INT1 pin */ 
    CNPDAbits.CNPDA2 = 1;
    CNENAbits.CNIEA2 = 1;
    _CNIE = 0;
    _CNIF = 0;
}

void InitApp(void)
{
    /* TODO Initialize User Ports/Peripherals/Project here */
    RPOR1bits.RP36R = 0b000011;                  /* Set pin11 as U2TX */
    RPINR18 = 0b0010011;                         /* Set pin? ad U1RX */
    
    //ying added on 2018 JA
    IPC2bits.U1RXIP = 2;
    IPC7bits.U2RXIP = 2;
    IPC3bits.AD1IP = 3;
    IPC4bits.CNIP = 4;
    
    /* Setup analog functionality and port direction */
    ANSELA = ANSELB = 0;
    TRISBbits.TRISB13 = 0;
    PORTBbits.RB13 = 0;
    TRISBbits.TRISB15 = 0;
    PORTBbits.RB15 = 0;
 
    Init_uart();
    Init_pwm();
    Init_adc1();
    Init_timer3();
    Init_int1();
//    InitDma0();   //unuse DMA, only ADCxBUF were used
}

void UART_printf_byte( char c )
{
    U2TXREG = c;
    while(U2STAbits.UTXBF);
}

void UART_printf_str( char * str )
{
    while(*str)
    {
        U2TXREG = *str++;
        while(U2STAbits.UTXBF);
    }
}

void Sample_start()
{
    Flag = 0;
    I = 0;
    //T3CONbits.TON = 1;
    _CNIE = 1;
}

void Sample_stop()
{
    Flag = 1;
    Cur_I = I;
    T3CONbits.TON = 0;
    _CNIE = 0;
}
