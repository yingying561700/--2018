/******************************************************************************/
#define DELAY_100uS asm volatile ("REPEAT, #3992"); Nop(); // 100uS delay  39.92MIPS
/******************************************************************************/
#define LENGTH 600

extern uint8_t Flag;
//extern int SenseX,SenseY,SenseZ,Temp;
extern int Drive_PWML[LENGTH],Drive_MID[LENGTH],Drive_PWMH[LENGTH];
extern uint16_t I,Cur_I;

/* TODO User level functions prototypes (i.e. InitApp) go here */
extern void Delay_ms(uint32_t ms);
extern void InitApp(void);         /* I/O and Peripheral Initialization */
extern void UART_printf_byte();
extern void UART_printf_str();
extern void Sample_start();
extern void Sample_stop();
