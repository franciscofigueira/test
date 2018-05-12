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

int arr[128];
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
             PR2=0xFFFF;//periodo
             IEC0bits.T2IE=1;//INTERRUPT ENABLE
             T2CONbits.TON=1;//timer on
         }
  
    int resultado=0;
      int i=7;
   int dacflag=0;
 int portflag=0;
 int valor=0;
 int x=4;
 int valor1=maxdac;
  
 void __attribute__((interrupt, auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;    /* Clear Timer interrupt flag */
    dacflag=1;
    
 
    
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
      
      void spi(){
         /* SPI1CONbits.MSTEN=1;
          SPI1STATbits.SPIROV=0;
          SPI1CONbits.PPRE=0b10;
          SPI1CONbits.SPRE=0b110;
          SPI1STATbits.SPIEN=1;
          IEC0bits.SPI1IE=0;*/
          SPI1CONbits.MSTEN = 1;	//**Master Mode Enable bit**//
							//1 = Master mode
							//0 = Slave mode

	SPI1STATbits.SPISIDL = 0;	//**Stop in Idle Mode bit**//
								//1 = Discontinue module operation when device enters Idle mode
								//0 = Continue module operation in Idle mode

	SPI1STATbits.SPITBF = 0;	//**SPI Transmit Buffer Full Status bit**//
								//1 = Transmit not yet started, SPIxTXB is full
								//0 = Transmit started, SPIxTXB is empty
								//Automatically set in hardware when CPU writes SPIxBUF location, loading SPIxTXB.
								//Automatically cleared in hardware when SPIx module transfers data from SPIxTXB to SPIxSR.

//	SPI1STATbits.SPIRBF = 0	//**SPI Receive Buffer Full Status bit**//
							//1 = Receive complete, SPIxRXB is full
							//0 = Receive is not complete, SPIxRXB is empty
							//Automatically set in hardware when SPIx transfers data from SPIxSR to SPIxRXB.
							//Automatically cleared in hardware when core reads SPIxBUF location, reading SPIxRXB.

	SPI1CONbits.FRMEN = 0;	//**Framed SPI Support bit**//
							//1 = Framed SPI support enabled
							//0 = Framed SPI support disabled

	SPI1CONbits.SPIFSD = 0;	//**Frame Sync Pulse Direction Control on SSx pin bit**//
							//1 = Frame sync pulse input (slave)
							//0 = Frame sync pulse output (master)

	SPI1CONbits.DISSDO = 0;	//**Disable SDOx pin bit**//
							//1 = SDOx pin is not used by module. Pin is controlled by associated port register.
							//0 = SDOx pin is controlled by the module

	SPI1CONbits.MODE16 = 1;	//**Word/Byte Communication Select bit**//
							//1 = Communication is word-wide (16 bits)
							//0 = Communication is byte-wide (8 bits)

	SPI1CONbits.SMP = 1;	//**SPI Data Input Sample Phase bit**//
							//Master mode:
							//1 = Input data sampled at end of data output time
							//0 = Input data sampled at middle of data output time
							//Slave mode:
							//SMP must be cleared when SPI is used in Slave mode.

	SPI1CONbits.CKE = 1;	//**SPI Clock Edge Select bit**//
							//1 = Serial output data changes on transition from active clock state to Idle clock state (see bit 6)
							//0 = Serial output data changes on transition from Idle clock state to active clock state (see bit 6)
							//Note: The CKE bit is not used in the Framed SPI modes. The user should program this bit to 0 for the
							//Framed SPI modes (FRMEN = 1).

	SPI1CONbits.SSEN = 0;	//**Slave Select Enable (Slave mode) bit**//
							//1 = SS pin used for Slave mode
							//0 = SS pin not used by module. Pin controlled by port function.

	SPI1CONbits.CKP = 0;	//**Clock Polarity Select bit**//
							//1 = Idle state for clock is a high level; active state is a low level
							//0 = Idle state for clock is a low level; active state is a high level

	SPI1CONbits.SPRE = 4;	//**Secondary Prescale (Master Mode) bits**//
							//(Supported settings: 1:1, 2:1 through 8:1, all inclusive)
							//111 = Secondary prescale 1:1
							//110 = Secondary prescale 2:1
							//...
							//000 = Secondary prescale 8:1
							
	SPI1CONbits.PPRE = 2;	//**Primary Prescale (Master Mode) bits**//
							//11 = Primary prescale 1:1
							//10 = Primary prescale 4:1
							//01 = Primary prescale 16:1
							//00 = Primary prescale 64:1

	IFS0bits.SPI1IF = 0;	//**SPI1 Interrupt Flag Status bit**//
							//1 = Interrupt request has occurred
							//0 = Interrupt request has not occurred

	IEC0bits.SPI1IE = 1;	//**SPI1 Interrupt Enable bit**//
							//1 = Interrupt request enabled
							//0 = Interrupt request not enabled

	IPC2bits.SPI1IP = 3;	//**SPI1 Interrupt Priority bits**//
							//111 = Interrupt is priority 7 (highest priority interrupt)
							//...
							//001 = Interrupt is priority 1
							//000 = Interrupt source is disabled

	SPI1STATbits.SPIROV = 0;	//**Receive Overflow Flag bit**//
								//1 = A new byte/word is completely received and discarded. The user software has not read the previous data in the SPIxBUF register.
								//0 = No overflow has occurred
							
	SPI1STATbits.SPIEN = 1;	//**SPI Enable bit**//
							//1 = Enables module and configures SCKx, SDOx, SDIx and SSx as serial port pins
							//0 = Disables module
      }
      
      void __attribute__((__interrupt__, __no_auto_psv__)) _SPI1Interrupt(void)
{
    IFS0bits.SPI1IF = 0;  	//resets RX2 interrupt flag
   
    }
   
      
                    void setDAC(int val) {
	val &= 0x0FFF;		//Format data according to MCP4921 serial interface protocol
	val |= 0x1000;		//Idem
	sendSPI(val);
}
                    void sendSPI(int data) {
	_LATB2 = 0;         			//Set chip select to zero so that dac will listen
	while(SPI1STATbits.SPITBF == 1);	//Wait until transmit buffer is empty
	SPI1BUF =data;				//Copy data to SPI buffer and start transmission
	while(SPI1STATbits.SPIRBF == 0);	//Wait until transmission (and reception) finishes
	data = SPI1BUF;				//Read and discard received word, to avoid overflow condition 
	_LATB2= 1;                      		//Deselect chip
}

int main(){
  
     _TRISF0 = 0;   
      _TRISF1 = 0; 
       _TRISF2 = 0; 
     TRISBbits.TRISB2=0;
     TRISBbits.TRISB3=0;
 TRISFbits.TRISF6=0;
     TRISDbits.TRISD1=1;


    configtime2();
  
    spi();
    
    config_uart();
    printf("test\n\r");
     LATFbits.LATF0=0;
      LATFbits.LATF1=0;
      valor=maxdac2;
      setDAC(valor);
      int bet=0;
      int val=0;
      int j=0;
      int numero=0;
      while(1){
          LATBbits.LATB3=0;
          
          for(j=11;j>=0;j--){
             
              bet=val | 1<<j;
              setDAC(bet);
         //  __delay_ms(1);
              while(dacflag!=1){}
              
              if(PORTDbits.RD1==0){
                  val=val| 1<<j;
              }
              dacflag=0;
              
            //  printf("%d %d\n\r",bet,j);
            //  printf("%d\n\r",PORTDbits.RD1);
             
         // }
          }
           LATBbits.LATB3=1;
          printf("%d %d\n",numero,val);
          ++numero;
          val=0;
          bet=0;
          /*
          if(dacflag==1){
               setDAC(valor);
             printf("%d \n\r",valor);
              if(i==9){
                  printf("%d  %d\n\r",valor,resultado);
                  i=0;
                  valor=maxdac2;
                  x=4;
                  resultado=0;
              }
              dacflag=0;
          }*/
      
      
    
      }
      
}
