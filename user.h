/******************************************************************************/
#define DELAY_100uS asm volatile ("REPEAT, #3992"); Nop(); // 100uS delay  39.92MIPS
/******************************************************************************/
#define LENGTH 600

extern uint8_t Flag,Flag1;
//extern int SenseX,SenseY,SenseZ,Temp;
extern int SenseX[LENGTH],SenseY[LENGTH],SenseZ[LENGTH],Temp[LENGTH];
extern int SenseX_Filter[LENGTH],SenseY_Filter[LENGTH],SenseZ_Filter[LENGTH];
 //SenseX_Mid,SenseY_Mid,SenseZ_Mid,
extern int SenseX_Val,SenseY_Val,SenseZ_Val;
extern uint16_t I,Cur_I,charI,Cur_charI;;

/* TODO User level functions prototypes (i.e. InitApp) go here */
extern void Delay_ms(uint32_t ms);
extern void InitApp(void);         /* I/O and Peripheral Initialization */
extern void UART_printf_byte( char c );
extern void UART_printf_str( char * str );
extern void Sample_start();
extern void Sample_stop();
