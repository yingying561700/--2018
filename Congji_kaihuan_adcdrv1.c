/*******************************************************************************
  ADC driver functions source file

  Company:
    Microchip Technology Inc.

  File Name:
    adcdrv1.c

  Summary:
    ADC function definitions

  Description:
    This source file configures the ADC for channel scanning and 10-bit operating mode is used. 
    The ADC works with the DMA and the result is stored in the DMA buffers. The timer 3 is used 
    to provide trigger for the conversion operations once every 125us or 8 kHz rate. 
*******************************************************************************/
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <xc.h>
#include <stdint.h>
#include "adcdrv1.h"

//uncomment next line if __eds__ is supported
//#define _HAS_DMA_
// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Constants
// *****************************************************************************
// *****************************************************************************
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
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        ADC initialization/configuration function.
 *                  This function is used to configure the ADC for channel scanning of 4 channels. 
 *****************************************************************************/
void Init_adc1( void )
{
    //TSIM = TSMP + (M ? TCONV) = 2*Tad + 4*12*Tad = 50*Tad = 1250ns
    ANSELAbits.ANSA0 = 1;
    ANSELAbits.ANSA1 = 1;
    ANSELBbits.ANSB0 = 1;
    
    AD1CON1bits.FORM = 0;                       // Data Output Format:  DOUT = 0000 00dd dddd dddd
    AD1CON1bits.SSRC = 2;                       // Sample Clock Source: GP Timer starts conversion
    AD1CON1bits.ASAM = 1;                       // ADC Sample Control: Sampling begins immediately after conversion
    AD1CON1bits.AD12B = 0;                      // 10-bit ADC operation
    AD1CON2bits.CSCNA = 0;                      // Don't Scan Input Selections for CH0+ during Sample A bit
    //need to change
    AD1CON1bits.SIMSAM = 1;
    AD1CON2bits.CHPS = 0b11;                    // Converts CH0,CH1,CH2,CH3
    AD1CON3bits.ADRC = 0;                       // ADC Clock is derived from Systems Clock
    AD1CON3bits.SAMC = 2;                       // Sample for 2 * Tad before converting
    AD1CON3bits.ADCS = 0;                       // ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M) = 25ns 

    //AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
    AD1CHS0bits.CH0SA = 0b111110;
    AD1CHS123bits.CH123SA0 = 0;
    AD1CHS123bits.CH123SA1 = 0;
    AD1CHS123bits.CH123SA2 = 0;
    AD1CHS123bits.CH123NA = 0;
    
    IEC0bits.AD1IE = 1;                         // Do Not Enable A/D interrupt
    _AD1IF = 0; 
    AD1CON1bits.ADON = 1;                       // Turn on the A/D converter
}


void Init_timer3( void )
{
    TMR3 = 0x0000;
    PR3 = 165.666666;  // 240k
    IFS0bits.T3IF = 0;  // Clear Timer 3 interrupt
    IEC0bits.T3IE = 0;  // Disable Timer 3 interrupt
    T3CONbits.TON = 0;  //Start Timer 3
}

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
 * Function:        void __attribute__((interrupt, auto_psv)) _DMA0Interrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Process the data in buffer A or in buffer B by invoking ProcessADCSamples function
 *****************************************************************************/
void __attribute__ ( (interrupt, auto_psv) ) _DMA0Interrupt( void )
{

#ifdef TEST_MODE
    test_dma0_int_flag=1;
#endif

    if( dmaBuffer == 0 )
    {
        ProcessADCSamples( &bufferA[1][0] );
        ProcessADCSamples( &bufferA[2][0] );
        ProcessADCSamples( &bufferA[3][0] );
        ProcessADCSamples( &bufferA[4][0] );
    }
    else
    {
        ProcessADCSamples( &bufferB[1][0] );
        ProcessADCSamples( &bufferB[2][0] );
        ProcessADCSamples( &bufferB[3][0] );
        ProcessADCSamples( &bufferB[4][0] );
    }

    dmaBuffer ^= 1;
    IFS0bits.DMA0IF = 0;    // Clear the DMA0 Interrupt Flag
}

/******************************************************************************
 * Function:        void ProcessADCSamples(__eds__ int16_t * AdcBuffer)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Dummy function, does nothing.
 *****************************************************************************/
void ProcessADCSamples( __eds__ int16_t *adcBuffer )
{
#ifdef TEST_MODE
    test_process_adc_samp_flag=1;
#endif
    /* Do something with ADC Samples */
}

/*******************************************************************************
 End of File
*/
