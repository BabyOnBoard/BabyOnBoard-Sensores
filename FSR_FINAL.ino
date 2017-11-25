#include <msp430g2553.h>
#include <legacymsp430.h>
#include <stdio.h>
#define TX BIT2

// bit3 bit1

int morto=0;
int milisegundos=0;
int cemsegundos=0;
int decsegundos=0;
int segundos=0;
int variavel=0;
int var_respira=0;
int conta_tempo=0;
int BABYON=0;


long int valor_anterior=0;
long int valor_anterior0=0;
long int valor_anterior1=0;
long int valor_anterior2=0;
long int  diff = 0;
long int sensorValue = 0;        // value read from the pot


char txbuf[256];

void uart_send(int len) {
  int i;
  for(i = 0; i < len; i++) {
    while (UCA0STAT & UCBUSY);
    UCA0TXBUF = txbuf[i];
  }
}



void leitura();
int main(void)
{
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;                     // submainclock 1mhz
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TACCR0 = 1000;				    // 1ms at 1mhz
  TA0CTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
////HORAS////
  TA1CCR0= 625; //  625*2*8uS= 0,01s 
  TA1CTL = TASSEL_2 + MC_3 + ID_3 +TAIE; // SMCLK  ,  UP/DOWN MODE, /8
/////////////
  P1IFG  = 0x00;			    //clear all interrupt flags
  P1DIR &= ~0x20;                            // P1.5 input
  P1DIR &= ~0x01;                           // // P1.0 input
  P1DIR = 0x0A;                              //02-BIT1 08 BIT3 ===10 = 0x0E SAÍDAS
  P1OUT &= ~0x0A;
  /// ADC configuration
        // V+ref=3V,V-ref=0V,Channel=A0
                   
        ADC10CTL0  = ADC10ON + ADC10IE;     // Vref Vr+=3v,Vr-=VSS,
											// S&Htime = 4 X ADCCLK,ADC10 on,ADC interrupts enabled
        ADC10CTL1  = ADC10DIV_7 ;           // INCH =0000->A0,ADCCLK src = ADC10CLK,
		                                    // ADCCLK/8,Single Channel Repeat Conversion
        ADC10AE0   = INCH_0 + INCH_3+ INCH_4;                // channel A0 
                   
        ADC10CTL0 |= ENC + ADC10SC;         // Start Conversion
  ///
  
  
  
  _BIS_SR(GIE);                 	    // global interrupt enable


 while(1){
        P1SEL2 = P1SEL = TX;
	P1IE &= ~0x01;			// disable interupt
	

//////////////////////////////////////////
        // UART: desabilitar paridade, transmitir um byte
        // na ordem LSB->MSB, um bit de stop
        UCA0CTL0 = 0;
        // UART: utilizar SMCLK
        UCA0CTL1 = UCSSEL_2;
        // Baud rate de 9600 bps, utilizando oversampling
        UCA0BR0 = 6;
        UCA0BR1 = 0;
        UCA0MCTL = UCBRF_8 + UCOS16;
     
    //    while (x==0)  {  uart_send(sprintf(txbuf," %d mensagens\n",x) );x++;}
    	
   //leitura();      
     
  //      //CRIANÇA AUSENTE 
 //       if(sensorValue>150){   //berço ausente
 //       //enviar sinal que o berço está sem a criança
     //     uart_send(sprintf(txbuf," Crianca ausente\n"));
//     uart_send(sprintf(txbuf," sensor = %d\n",sensorValue));
//       }
     
   
        //CRIANÇA NO BERÇO
        //caso o berço esteja ocupado
            //verificar se está ou não em movimento
            
            
           
              if ((P1IN & 0x20) != 0){   //>vdd/2
             uart_send(sprintf(txbuf,"Ausente\n"));
                    BABYON=0; //flag criança ausente
                    P1OUT&=~0x02; //sinal criança ausente
                    P1OUT&=~0x08; 
                    morto=0;    segundos=0;  
             }
             
             else{ 
                       
               //     uart_send(sprintf(txbuf,"Presente\n"));
               BABYON=1; //flag criança no berço
               
     // uart_send(sprintf(txbuf," sensor = %d\n",sensorValue));
          
       
          if(sensorValue<100){
            
          //  uart_send(sprintf(txbuf," sensor = %d\n",sensorValue));
       
          }
          
           
                      P1OUT|=0x02; //criança no berço
                      
               }
               
               

  

        
 }
}



//
////interrupção que conta a cada 0.001 Segundos
//interrupt(TIMER0_A0_VECTOR) TA0_ISR(void)
//{
//
//
// 
// 
// //////////////////////////////
//  
//  milisegundos++;
//  
//}

// INTERRUPÇÃO QUE CONTA A CADA 0,01 SEGUNDOS 
interrupt(TIMER1_A1_VECTOR) TA1_ISR(void)
{

 
  if(BABYON=1){
    ///verifica apenas quando a criança estiver no berço
  cemsegundos++;
  
  //////a cada centesimo de segundo///// 
  
      if(sensorValue>30){
      var_respira=1;
      }
  //////////////////////////////////////
  
  
  //////conta o tempo/////////
 if(cemsegundos>=10) {
  cemsegundos=0;    
  decsegundos++;
    }
    
   if(decsegundos>=10) {
      decsegundos=0;
      segundos++;
    
 
      /////verifica se já houve detecção de um pico de respiração
       if(var_respira==1){
   
       conta_tempo=segundos;
       segundos=0;
       P1OUT&=~0x08; //criança respirando
       var_respira=0; uart_send(sprintf(txbuf,"tempo = %d \n",conta_tempo)); 
       }
   
   }
 
 if(segundos>=10) {
      
    if ((P1IN & 0x20) != 0){
    }
    else{
       P1OUT|= 0x08; /// criança sem respiração
       
             uart_send(sprintf(txbuf,"MORTO\n")); 
      segundos=0;
      morto=1;       
    }      
    }

 }
 
 
  TA1CTL &= ~TAIFG;
}

	

#pragma vector = ADC10_VECTOR
    __interrupt void adc10_interrupt(void)
{
         unsigned int adc_value = 0;
		 adc_value = ADC10MEM;
sensorValue=adc_value;


  
   
    
   ADC10CTL0 |= ENC + ADC10SC;  // Start Conversion  
		 
}
