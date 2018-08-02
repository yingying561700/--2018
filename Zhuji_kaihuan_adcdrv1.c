/*******************************************************************************
  ADC driver functions source file
  Company:Microchip Technology Inc.
  File Name:adcdrv1.c
  Summary:ADC function definitions
  Description:
    This source file configures the ADC for channel scanning and 10-bit operating mode is used. 
    The ADC works with the DMA and the result is stored in the DMA buffers. The timer 3 is used 
    to provide trigger for the conversion operations once every 125us or 8 kHz rate. 
*******************************************************************************/
#include <xc.h>
#include <stdint.h>
#include "adcdrv1.h"

// Define Message Buffer Length for ECAN1/ECAN2
#define MAX_CHNUM       4           // Highest Analog input number in Channel Scan
#define SAMP_BUFF_SIZE  8           // Size of the input buffer per analog input
#define NUM_CHS2SCAN    4           // Number of channels enabled for channel scan

// Number of locations for ADC buffer = 14 (AN0 to AN13) x 8 = 112 words
// Align the buffer to 128 words or 256 bytes. This is needed for peripheral indirect mode
#ifdef _HAS_DMA_
__eds__ int16_t bufferA[MAX_CHNUM + 1][SAMP_BUFF_SIZE] __attribute__( (eds, space(dma), aligned(256)) );
__eds__ int16_t bufferB[MAX_CHNUM + 1][SAMP_BUFF_SIZE] __attribute__( (eds, space(dma), aligned(256)) );
#else
int16_t         bufferA[MAX_CHNUM + 1][SAMP_BUFF_SIZE] __attribute__( (space(xmemory), aligned(256)) );
int16_t         bufferB[MAX_CHNUM + 1][SAMP_BUFF_SIZE] __attribute__( (space(xmemory), aligned(256)) );
#endif
#ifdef TEST_MODE
unsigned char  test_dma0_int_flag =0;
unsigned char test_process_adc_samp_flag=0;
#endif
//Function Prototype
void            ProcessADCSamples( __eds__ int16_t *adcBuffer );

/******************************************************************************
 * Function:        void InitAdc1(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        ADC initialization/configuration function.
 *                  This function is used to configure the ADC for channel scanning of 4 channels. 
 *****************************************************************************/
void Init_adc1( void )
{
    /*-----------------------------ENABLE OPA-----------------------------------*/
    CM1CONbits.OPAEN = 1;
    CM2CONbits.OPAEN = 1;
    CM5CONbits.OPAEN = 1;
    
    //TSIM = TSMP + (M ? TCONV) = 2*Tad + 4*12*Tad = 50*Tad = 1250ns
    ANSELAbits.ANSA0 = 1;
    ANSELAbits.ANSA1 = 1;
    ANSELBbits.ANSB0 = 1;
    ANSELBbits.ANSB1 = 1;
    ANSELBbits.ANSB2 = 1;
    ANSELBbits.ANSB3 = 1;
    ANSELAbits.ANSA4 = 1;
    ANSELBbits.ANSB7 = 1;
    ANSELBbits.ANSB9 = 1;
    
    AD1CON1bits.FORM = 0;                       // Data Output Format:  DOUT = 0000 00dd dddd dddd
    //need to change
    AD1CON1bits.SSRC = 2;                       // Sample Clock Source: GP Timer starts conversion
    AD1CON1bits.ASAM = 1;                       // ADC Sample Control: Sampling begins immediately after conversion
    AD1CON1bits.AD12B = 0;                      // 10-bit ADC operation
    AD1CON2bits.CSCNA = 0;                     // Don't Scan Input Selections for CH0+ during Sample A bit
    //need to change
    AD1CON1bits.SIMSAM = 1;
    AD1CON2bits.CHPS = 0b11;                    // Converts CH0,CH1,CH2,CH3
    AD1CON3bits.ADRC = 0;                       // ADC Clock is derived from Systems Clock
    AD1CON3bits.SAMC = 2;                       // Sample for 2 * Tad before converting
    AD1CON3bits.ADCS = 0;                       // ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M) = 25ns 

    //AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
    AD1CHS0bits.CH0SA = 0b111110;
    AD1CHS123bits.CH123SA0 = 1;
    AD1CHS123bits.CH123SA1 = 1;
    AD1CHS123bits.CH123SA2 = 0;
    AD1CHS123bits.CH123NA = 0;
    
    IEC0bits.AD1IE = 1;                         // Do Not Enable A/D interrupt
    _AD1IF = 0; 
    AD1CON1bits.ADON = 1;                       // Turn on the A/D converter
}

/******************************************************************************
 * Function:        void InitTmr3(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        Timer initialization/configuration function.
 *                  Timer 3 is setup to time-out every 125 microseconds (8Khz Rate).
 *                  As a result, the module  will stop sampling and trigger a conversion on every Timer3
 *                  time-out, i.e., Ts=125us.
 *****************************************************************************/
void Init_timer3( void )
{
    TMR3 = 0x0000;
    PR3 = 165.666666;           // Trigger ADC1 every 125usec
    IFS0bits.T3IF = 0;  // Clear Timer 3 interrupt
    IEC0bits.T3IE = 0;  // Disable Timer 3 interrupt
    T3CONbits.TON = 0;  //Start Timer 3
}

/******************************************************************************
 * Function:        void InitDma0(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:         This function is used to configure the DMA0 module to read from peripheral
 *                  address 0x300 (ADC1BUF0)  and write to the RAM continuously using peripheral
 *                  indirect addressing in ping pong mode.
 *****************************************************************************/
void Init_dma0( void )
{
    DMA0CONbits.AMODE = 2;  // Configure DMA for Peripheral indirect mode
    DMA0CONbits.MODE = 2;   // Configure DMA for Continuous Ping-Pong mode
    DMA0PAD = ( int ) &ADC1BUF0;
    DMA0CNT = ( SAMP_BUFF_SIZE * NUM_CHS2SCAN ) - 1;
    DMA0REQ = 13;           // Select ADC1 as DMA Request source
    #ifdef _HAS_DMA_
    DMA0STAL = __builtin_dmaoffset( &bufferA );
    DMA0STAH = __builtin_dmapage( &bufferA );
    DMA0STBL = __builtin_dmaoffset( &bufferB );
    DMA0STBH = __builtin_dmapage( &bufferB );
    #else
    DMA0STAL = ( uint16_t ) ( &bufferA );
    DMA0STAH = ( uint16_t ) ( &bufferA );
    DMA0STBL = ( uint16_t ) ( &bufferB );
    DMA0STBH = ( uint16_t ) ( &bufferB );
    #endif
    IFS0bits.DMA0IF = 0;    //Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1;    //Set the DMA interrupt enable bit
    DMA0CONbits.CHEN = 1;   // Enable DMA
}

/*=============================================================================
_DMA0Interrupt(): ISR name is chosen from the device linker script.
=============================================================================*/
// dmaBuffer variable is used to toggle the buffer between A and B for copying.
uint16_t    dmaBuffer = 0;
/******************************************************************************
 * Function:        void ProcessADCSamples(__eds__ int16_t * AdcBuffer)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        Dummy function, does nothing.
 *****************************************************************************/
void ProcessADCSamples( __eds__ int16_t *adcBuffer )
{
#ifdef TEST_MODE
    test_process_adc_samp_flag=1;
#endif
    /* Do something with ADC Samples */
}
