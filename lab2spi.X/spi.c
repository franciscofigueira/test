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
  void configtime2()
         {
             T2CONbits.TON=0; //Timer off
             TMR2=0;//RESET
         //  PR2=1474;MAX TIMER COUNT
             T2CONbits.TCS=0;//INTERNAL CLOCK
             T2CONbits.TCKPS=0;//prescaler
             PR2=main_period;//periodo
             IEC0bits.T2IE=1;//INTERRUPT ENABLE
             T2CONbits.TON=1;//timer on
         }
  
 void __attribute__((interrupt, auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;    /* Clear Timer interrupt flag */

 }
     
          void configoutput4(){
             OC4CONbits.OCTSEL=0;//timer 2
             OC4CONbits.OCM=5;//pwm mode
             OC4RS=0;//initial period
             OC4R=main_period/2;
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
      
      int valor1=0;
      int flag1=0;
      int aux=0;
      void __attribute__((__interrupt__, __no_auto_psv__)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;  	//resets RX2 interrupt flag
 while (U2STAbits.URXDA) {
RXbuffer[str_pos] = U2RXREG; //stores the last received char in the buffer
if (RXbuffer[str_pos] == '\r') {//verifica que foi carregado ENTER
v = 1;
valor1=valor1+204;
flag1=1;
//if(valor1>=4095){valor1=0;}
}
str_pos++; //increments the position in the buffer to store the next char
if (str_pos >= 2) {
str_pos = 0;
} //if the last position is reached then return to initial position
} 
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
   
    if(LED1==1){
        LED1==0;}
    
    else{
        LED1==1;}
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
    arr[	0	]=	2048	;
arr[	1	]=	2148	;
arr[	2	]=	2249	;
arr[	3	]=	2349	;
arr[	4	]=	2448	;
arr[	5	]=	2546	;
arr[	6	]=	2643	;
arr[	7	]=	2738	;
arr[	8	]=	2832	;
arr[	9	]=	2924	;
arr[	10	]=	3013	;
arr[	11	]=	3101	;
arr[	12	]=	3186	;
arr[	13	]=	3268	;
arr[	14	]=	3347	;
arr[	15	]=	3423	;
arr[	16	]=	3496	;
arr[	17	]=	3565	;
arr[	18	]=	3631	;
arr[	19	]=	3693	;
arr[	20	]=	3751	;
arr[	21	]=	3805	;
arr[	22	]=	3854	;
arr[	23	]=	3899	;
arr[	24	]=	3940	;
arr[	25	]=	3976	;
arr[	26	]=	4008	;
arr[	27	]=	4035	;
arr[	28	]=	4057	;
arr[	29	]=	4074	;
arr[	30	]=	4086	;
arr[	31	]=	4094	;
arr[	32	]=	4095	;
arr[	33	]=	4094	;
arr[	34	]=	4086	;
arr[	35	]=	4074	;
arr[	36	]=	4057	;
arr[	37	]=	4035	;
arr[	38	]=	4008	;
arr[	39	]=	3976	;
arr[	40	]=	3940	;
arr[	41	]=	3899	;
arr[	42	]=	3854	;
arr[	43	]=	3805	;
arr[	44	]=	3751	;
arr[	45	]=	3693	;
arr[	46	]=	3631	;
arr[	47	]=	3565	;
arr[	48	]=	3496	;
arr[	49	]=	3423	;
arr[	50	]=	3347	;
arr[	51	]=	3268	;
arr[	52	]=	3186	;
arr[	53	]=	3101	;
arr[	54	]=	3013	;
arr[	55	]=	2924	;
arr[	56	]=	2832	;
arr[	57	]=	2738	;
arr[	58	]=	2643	;
arr[	59	]=	2546	;
arr[	60	]=	2448	;
arr[	61	]=	2349	;
arr[	62	]=	2249	;
arr[	63	]=	2148	;
arr[	64	]=	2048	;
arr[	65	]=	1948	;
arr[	66	]=	1847	;
arr[	67	]=	1747	;
arr[	68	]=	1648	;
arr[	69	]=	1550	;
arr[	70	]=	1453	;
arr[	71	]=	1358	;
arr[	72	]=	1264	;
arr[	73	]=	1172	;
arr[	74	]=	1083	;
arr[	75	]=	995	;
arr[	76	]=	910	;
arr[	77	]=	828	;
arr[	78	]=	749	;
arr[	79	]=	673	;
arr[	80	]=	600	;
arr[	81	]=	531	;
arr[	82	]=	465	;
arr[	83	]=	403	;
arr[	84	]=	345	;
arr[	85	]=	291	;
arr[	86	]=	242	;
arr[	87	]=	197	;
arr[	88	]=	156	;
arr[	89	]=	120	;
arr[	90	]=	88	;
arr[	91	]=	61	;
arr[	92	]=	39	;
arr[	93	]=	22	;
arr[	94	]=	10	;
arr[	95	]=	2	;
arr[	96	]=	0	;
arr[	97	]=	2	;
arr[	98	]=	10	;
arr[	99	]=	22	;
arr[	100	]=	39	;
arr[	101	]=	61	;
arr[	102	]=	88	;
arr[	103	]=	120	;
arr[	104	]=	156	;
arr[	105	]=	197	;
arr[	106	]=	242	;
arr[	107	]=	291	;
arr[	108	]=	345	;
arr[	109	]=	403	;
arr[	110	]=	465	;
arr[	111	]=	531	;
arr[	112	]=	600	;
arr[	113	]=	673	;
arr[	114	]=	749	;
arr[	115	]=	828	;
arr[	116	]=	910	;
arr[	117	]=	995	;
arr[	118	]=	1083	;
arr[	119	]=	1172	;
arr[	120	]=	1264	;
arr[	121	]=	1358	;
arr[	122	]=	1454	;
arr[	123	]=	1550	;
arr[	124	]=	1648	;
arr[	125	]=	1747	;
arr[	126	]=	1847	;
arr[	127	]=	1948	;

     _TRISF0 = 0;   
     TRISB=0;

     
int valor=0;
TRISFbits.TRISF6=0;
    configtime2();
    spi();
    configoutput4();
    config_uart();
    printf("test\n\r");
     LATFbits.LATF0=0;
      LATFbits.LATF1=0;
      int i=0;
      while(1){

       /*   for(i=0;i<128;++i){

     setDAC(arr[i]);               		//Deselect chip
     __delay_ms(0.05);
    
          }
          valor=0;*/
          
      setDAC(valor1);
      printf("%d\n\r",valor1);
      if(flag1==1){
          aux=aux+204;
          valor1=aux;
          printf("%d\n\r",valor1);
          
flag1=0;
if(aux>=4095){aux=0;}
      }
          __delay_ms(1);
          /*
          for(i=0;i<32;++i){
              LATB=arr[i++];
              __delay_ms(0.1);
              if(i>=32) i=0;*/

             
    /*  while(1){
    
          
//sscanf(RXbuffer, "%d",&duty);
if(v==1){printf("%d\n\r",OC4R);
    break;} //Verificada a recepção de um enter sai do loop
}
          
v=0;*/
              }   
    
}