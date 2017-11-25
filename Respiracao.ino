#include <msp430g2553.h>
#include <legacymsp430.h>
#include <stdio.h>

int morto=0;
int milisegundos=0;
int cemsegundos=0;
int decsegundos=0;
int segundos=0;
int variavel=0;
int var_respira=0;
int conta_tempo=0;
int BABYON=0;


long int sensorValue = 0;     



int main(void)
{
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;                     // submainclock 1mhz
  WDTCTL = WDTPW + WDTHOLD;                 // STOP WDT
  
  CCTL0 = CCIE;                             // CCR0 HABILITA INTERRUPÇÃO

////TIMERA1////
  TA1CCR0= 625; //  625*2*8uS= 0,01s 
  TA1CTL = TASSEL_2 + MC_3 + ID_3 +TAIE; // SMCLK  ,  UP/DOWN MODE, /8


///////DECLARAÇÃO DE ENTRADAS E SAÍDAS//////
  P1IFG  = 0x00;			    //LIMPA FLAGS DE INTERRUPÇÃO
  P1DIR &= ~0x20;                            // P1.5 INPUT
  P1DIR &= ~0x01;                           // // P1.0 INPUT
  P1DIR = 0x0A;                              //02-BIT1 08 BIT3 ===10 = 0x0E SAÍDAS
  P1OUT &= ~0x0A;
  
/////// CONFIGURAÇÃO DO ADC ///////////
        // V+ref=3V,V-ref=0V,CANAL=A0
                   
        ADC10CTL0  = ADC10ON + ADC10IE;     // Vref Vr+=3v,Vr-=VSS,
											// S&Htime = 4 X ADCCLK,ADC10 on,ADC interrupts enabled
        ADC10CTL1  = ADC10DIV_7 ;           // INCH =0000->A0,ADCCLK src = ADC10CLK,
		                                    // ADCCLK/8
        ADC10AE0   = INCH_0 + INCH_3+ INCH_4;                // CANAL A0 
                   
        ADC10CTL0 |= ENC + ADC10SC;         // INICIA CONVERSÃO
 
/////////////////HABILITAR INTERRUPÇÃO GLOBAL ///////////////////  
    
  _BIS_SR(GIE);             


 while(1){
      
        P1IE &= ~0x01;			// disable interupt

      
      
     
     /////////////////LÓGICA DE DETECÇÃO////////////////////// 	
               
           
              if ((P1IN & 0x20) != 0){  //>VDD/2 
                    BABYON=0;           //FLAG CRIANÇA AUSENTE
                    P1OUT&=~0x02;       //SAÍDA INDICANDO CRIANÇA AUSENTE
                    P1OUT&=~0x08; 
                    morto=0;    segundos=0;  
             }
             
             else{ 
                       
               BABYON=1;                 //FLAG CRIANÇA NO BERÇO
               
               
       
              if(sensorValue<100){}
          
           
                      P1OUT|=0x02;       //SAÍDA INDICANDO CRIANÇA NO BERÇO
                      
               }

        
 }
}



// INTERRUPÇÃO QUE CONTA A CADA 0,01 SEGUNDOS 
interrupt(TIMER1_A1_VECTOR) TA1_ISR(void)
{

     if ((P1IN & 0x20) != 0){   //>vdd/2
    
                    BABYON=0; //flag criança ausente
                    P1OUT&=~0x02; //sinal criança ausente
                    P1OUT&=~0x08; 
                    morto=0;    segundos=0;  
             }
             
     else{ 
            BABYON=1; //flag criança no berço
            P1OUT|=0x02; //criança no berço
                      
               
 
          if(BABYON>=1){
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
       if(var_respira>=1){
   
       conta_tempo=segundos;
       segundos=0;
       P1OUT&=~0x08; //criança respirando
       var_respira=0;
     }
       
        if(morto>=1){
       P1OUT|= 0x08; /// criança sem respiração
        }
   }
 
 if(segundos>=10) {
      
    if ((P1IN & 0x20) != 0){
    }
    else{
       P1OUT|= 0x08; /// criança sem respiração
       
      segundos=0;
      morto=1;       
    }      
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


  
   
    
   ADC10CTL0 |= ENC + ADC10SC;  // INICIA CONVERSÃO
		 
}
