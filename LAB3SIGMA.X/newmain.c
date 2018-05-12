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

int valores[512];
char RXbuffer[80];
int v=0,str_pos=0;
double duty=0.1;
 #define main_period 1023  //sets the maximum count for Timer_2
#define maxdac 4096;
#define maxdac2 4096/2;
  void configtime2()
         {
             T2CONbits.TON=0; //Timer off
             TMR2=0;//RESET
         //  PR2=1474;MAX TIMER COUNT
             T2CONbits.TCS=0;//INTERNAL CLOCK
             T2CONbits.TCKPS=0;//prescaler
             PR2=400;//0xFFFF;//periodo
             IEC0bits.T2IE=1;//INTERRUPT ENABLE
             T2CONbits.TON=1;//timer on
         }
  int guarda=0;
  int k=0;
  int conta=0;
  int flag=0;
 void __attribute__((interrupt, auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;    /* Clear Timer interrupt flag */

    
    if(PORTDbits.RD1==1){
              LATBbits.LATB4=1;
          }
          else{
              LATBbits.LATB4=0;
          }
    if(k<16){
        guarda=guarda|PORTDbits.RD1<<k;
    }
    ++k;
    if(k>=16){
  //valores[conta]=PORTDbits.RD1;
        valores[conta]=guarda;
        guarda=0;
           ++conta;
  k=0;
    }
    
 
    if(conta>=511){
        flag=1;
    }
  /*  if(LATBbits.LATB4==0){
        LATBbits.LATB4=1;
    }
    else{
        LATBbits.LATB4=0;
    }*/
 }
 
 
  
          
      void config_uart(void){
	   U2MODEbits.UARTEN = 0;  // Bit15 TX, RX DISABLED, ENABLE at end of func
    U2MODEbits.USIDL = 0;   // Bit13 Continue in Idle
    U2MODEbits.WAKE = 0;    // Bit7 No Wake up (since we don't sleep here)
    U2MODEbits.LPBACK = 0;  // Bit6 No Loop Back
    U2MODEbits.ABAUD = 0;   // Bit5 No Autobaud (would require sending '55')
    U2MODEbits.PDSEL = 0;   // Bits1,2 8bit, No Parity
    U2MODEbits.STSEL = 0;   // Bit0 One Stop Bit
U2STAbits.UTXEN = 1;//Enable transmissão
U2STAbits.URXISEL = 1;//flag é activada quando recebe um caracter
    // Load a value into Baud Rate Generator.  Example is for 11500.
    //  U2BRG = (Fcy/(16*BaudRate))-1
    //  U2BRG = (29491200/(16*115200))-1
    //---------  U2BRG = 15
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
      
    
int main(){
  
     _TRISF0 = 0;   
      _TRISF1 = 0; 
       _TRISF2 = 0; 
     TRISBbits.TRISB2=0;
     TRISBbits.TRISB3=0;
       TRISBbits.TRISB4=0;
 TRISFbits.TRISF6=0;
     TRISDbits.TRISD1=1;


    configtime2();
  
    int j=0;
    int bin=0;
    config_uart();
    printf("test\n\r");
     LATFbits.LATF0=0;
      LATFbits.LATF1=0;
    LATBbits.LATB4=0;
    int bit1;
    int i;
      while(1){
          
          if(flag==1){
              T2CONbits.TON=0;
              for(i=0;i<512;++i){
               
                 // printf("%d\n",valores[i]);
                 for(j=0;j<16;++j){
                      bit1=valores[i]>> j;
                    
                      if(bit1&1){
                          printf("1\n");
                      }
                      else{
                          printf("0\n");
                      }
                  }
                 
              
              }
                  
              printf("finished\n\r");
              flag=0;
              conta=0;
              __delay_ms(5000);
              T2CONbits.TON=1;
          }
          
      }
      
}

 