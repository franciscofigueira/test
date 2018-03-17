#define LED1    _LATF0
#define LED2    _LATF1


#define FCY 29491200L
#define FOSC (FCY*4)            //number os clock cycles
/* Include List */
#include <libpic30.h>
#include <p30F4011.h>
#include <stdio.h>

/* Global Definitions */

/* For XC16 Ver <= 1.24 */
//_FOSC(CSW_FSCM_OFF & XT_PLL16); //oscillator at 16x crystal frequency
//_FWDT(WDT_OFF);                 //watchdog timer off
//_FBORPOR(MCLR_EN & PWRT_OFF);   //Turn MCLR pin ON and Energy Timers OFF

/* For XC16 Ver >= 1.24 */
#pragma config FCKSMEN=CSW_FSCM_OFF
#pragma config FOS=PRI
#pragma config FPR=XT_PLL16 
#pragma config WDT=WDT_OFF
#pragma config MCLRE=MCLR_EN
#pragma config FPWRT=PWRT_OFF

//char RXbuffer[RXBUFFSIZE];	//buffer used to store characters from serial port
int str_pos = 0; 	//position in the RXbuffer
int adc_vals[256]; 	//position in the RXbuffer


void config_uart(void){
	   U2MODEbits.UARTEN = 0;  // Bit15 TX, RX DISABLED, ENABLE at end of func
    U2MODEbits.USIDL = 0;   // Bit13 Continue in Idle
    U2MODEbits.WAKE = 0;    // Bit7 No Wake up (since we don't sleep here)
    U2MODEbits.LPBACK = 0;  // Bit6 No Loop Back
    U2MODEbits.ABAUD = 0;   // Bit5 No Autobaud (would require sending '55')
    U2MODEbits.PDSEL = 0;   // Bits1,2 8bit, No Parity
    U2MODEbits.STSEL = 0;   // Bit0 One Stop Bit

    // Load a value into Baud Rate Generator.  Example is for 11500.
    //  U2BRG = (Fcy/(16*BaudRate))-1
    //  U2BRG = (29491200/(16*115200))-1
    //  U2BRG = 15
    U2BRG = 15;

    IFS1bits.U2RXIF = 0;    // Clear the Receive Interrupt Flag
    IEC1bits.U2RXIE = 1;    // Enable Receive Interrupts

    U2MODEbits.UARTEN = 1;  // And turn the peripheral on

    __C30_UART = 2;
}
void __attribute__((__interrupt__, __no_auto_psv__)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;  	//resets RX2 interrupt flag
    
}

void timer2(){
     T2CONbits.TON = 0;      //Timer_2 is OFF
    TMR2 = 0;               //resets Timer_2
    PR2 = 4608;             	//sets the maximum count for Timer_2  90us
    T2CONbits.TCS = 0;      //choose FCY as clock source for Timer_2
    T2CONbits.TCKPS = 0; //sets the Timer_2 pre-scaler to 1
    IFS0bits.T2IF = 0;      //clears Timer_2 interrupt flag
    _T2IE = 1;     	       	//enable Timer_2 Interrupts
    T2CONbits.TON = 1;      //turns Timer_2 OFF
}

void adc(){
    ADCON1bits.ADON = 0; 	//turns the ADC module OFF
	ADCON1bits.FORM = 0;	//result as an integer
	ADCON1bits.SSRC = 0;	//clearing sampling bit starts convertion
	ADCON1bits.SIMSAM = 1;	//samples each channel at a time
	ADCON1bits.ASAM = 0;	//sampling begins when samp bit is set
//ADCON1 = 0;
	ADCON2bits.VCFG = 0;	//VrefH+ = AVcc and Vref- = AGND
	ADCON2bits.CHPS = 1;	//samples CH0 only
	ADCON2bits.SMPI = 0;	//interrups at every sample sequence
	ADCON2bits.BUFM = 0;	//result buffer used as a 16bit word
	ADCON2bits.ALTS = 0;	//always use MUX-A
	ADCON2bits.CSCNA = 1;	//enable scan
//ADCON2 = 0x0400;

	ADCON3bits.SAMC = 0xF;	//16 Tad
	ADCON3bits.ADRC = 0;	//use system clock for convertion
	ADCON3bits.ADCS = 0xF;	//16 Tcy for convertion time
//ADCON2 = 0x0F0F;
	

	ADCHSbits.CH123NB = 0;	//negative input is Vref-
	ADCHSbits.CH123SB = 0;	//positive input is AN0 and so on
	ADCHSbits.CH0NB = 0;	//negative input is Vref-
	ADCHSbits.CH0SB = 0;	//positive input is AN0 and so on
	ADCHSbits.CH123NA = 0;	//negative input is Vref-
	ADCHSbits.CH123SA = 1;	//positive input is AN0 and so on
	ADCHSbits.CH0NA = 0;	//negative input is Vref-
	ADCHSbits.CH0SA = 0;	//positive input is AN0 and so on
	IEC0bits.ADIE=1;//enable interrupt
//	ADPCFG = 0xFC;			//AN0 and AN1 are analog inputs

	ADCSSL = 0x01;			//AN0 and AN1 are enabled for scan

	ADCON1bits.ADON = 1; 	//turns the ADC module ON
       
}




void __attribute__((interrupt, auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;    /* Clear Timer interrupt flag */
  
    if(LED1==1){
        LED1=0;
    }
    else{
        LED1=1;
    }

ADCON1bits.SAMP = 0;	
    
}

int i=0;
int valor;
int flag=0;
int adc_vals2[256];
void __attribute__((__interrupt__, auto_psv)) _ADCInterrupt(void) {
IFS0bits.ADIF = 0;
adc_vals[i]=ADCBUF0;
adc_vals2[i]=ADCBUF1;


valor=ADCBUF0;
ADCON1bits.SAMP = 1;
++i;
flag=1;
}




int main(void) {
 
    _TRISF0 = 0;                //this configures the PORTF pin 0 an output

    LED1 = 0;                   //this turns the LED1 ON
    config_uart();
    adc();

   
    printf("Serial port says: Hello\n\r");
    ADCON1bits.SAMP = 1;
     timer2();
     int p;
     int numero=256;//numero de conversoes a ser realizadas
    while(1){

        if(i==numero){
             //T2CONbits.TON = 0;
             	ADCON1bits.ADON = 0; 
             for(p=0;p<numero;++p) 
            printf("%d %d\n",p,adc_vals[p]);
                for(p=0;p<numero;++p) 
                 printf("%d %d\n",p,adc_vals2[p]);
           
                i=0;
        }
        /*if(flag==1){
            printf("%d %d\n",i,valor);
            flag=0;
            if(i==50000){
               T2CONbits.TON = 0;
               	ADCON1bits.ADON = 0; 
                
            }
        }*/
       
    }
    return 0;
}
