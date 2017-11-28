#include <msp430g2553.h>
#include <legacymsp430.h>

// Codigo para o MSP430 configurado como
// escravo SPI

#define MISS BIT1
#define MOSS BIT2
#define SCLK BIT4

#define MOTORB1 (BIT4|BIT5|BIT6|BIT7)
#define MOTORB2 (BIT0|BIT1|BIT2|BIT3)
#define MOTORB3 BIT3

int cemsegundos=0;
int decsegundos=0;
int segundos=0;
int minutos=0;
int parar;

void Atraso(volatile unsigned int x)
{
	while(x--);
}

void Send_Data(volatile unsigned char c)
{
	while((IFG2&UCA0TXIFG)==0);
	UCA0TXBUF = c;
}

void  parainterrup()
{
  
      TA1CCR0= 625; //  625*2*8uS= 0,01s 
      TA1CTL = TASSEL_2 + MC_0 + ID_3 +TAIE; // SMCLK  ,  UP/DOWN MODE, /8
     
  
  
}

void para_movimento()
{          
          //TA1CTL &= MC_0;
          P1OUT&= ~MOTORB1;
          P2OUT&= ~MOTORB1;
          //TA1CTL &= ~TAIFG;
}

void move1() {


          P2OUT&= ~MOTORB1;
          //P2OUT|= 0x00000050;
          P2OUT=BIT4;
          P1OUT=BIT6;
        Atraso(100000);
       
          P2OUT&= ~MOTORB1;
          //P2OUT|=0x00000060;
          P2OUT=BIT5;
          P1OUT=BIT6;
        Atraso(100000);
      
          P2OUT&= ~MOTORB1;
          //P2OUT|=0x00000100; 
          P2OUT=BIT5;
          P1OUT=BIT7;
          Atraso(100000);
        
          P2OUT&= ~MOTORB1;
          //P2OUT|=0x00000090;
          P2OUT=BIT4;
          P1OUT=BIT7;
          Atraso(100000);
        
        
          P1OUT&= ~MOTORB1;

          P2OUT&= ~MOTORB1;
          
          //TA1CTL &= ~TAIFG;
}


void move2()
{
          
          P2OUT&= ~MOTORB2;
          //P2OUT|= 0x00000050;
          P2OUT=BIT0|BIT2;
          Atraso(10000000);
       
          P2OUT&= ~MOTORB2;
          //P2OUT|=0x00000060;
          P2OUT=BIT1|BIT2;
          Atraso(10000000);
      
          P2OUT&= ~MOTORB2;
          //P2OUT|=0x00000100; 
          P2OUT=BIT1|BIT3;
          Atraso(10000000);
        
          P2OUT&= ~MOTORB2;
          //P2OUT|=0x00000090;
          P2OUT=BIT0|BIT3;
          Atraso(10000000);

          P2OUT&= ~MOTORB2;

}

void move3()
{
    P1OUT=BIT3;
}

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	P1SEL2 = P1SEL = MOSS + MISS + SCLK;

        P1DIR|= BIT7|BIT6|BIT3;
        P1OUT =~(BIT7|BIT6|BIT3);
        P2DIR|= (MOTORB1|MOTORB2);
        P2OUT=~(MOTORB1|MOTORB2);

	UCA0CTL1 = UCSWRST + UCSSEL_3;
	UCA0CTL0 = UCCKPH+UCMSB+UCMODE_0+UCSYNC;
	UCA0CTL1 &= ~UCSWRST;
        
        CCTL0 = CCIE;                             // CCR0 HABILITA INTERRUPÇÃO
    
	//Send_Data(10);
	IE2 |= UCA0RXIE;
	_BIS_SR(GIE);

while(1){



        P1IE &= ~0x01;
        
  if(parar>=1){
  ////TIMERA1////
    P2OUT=BIT0|BIT2;
     para_movimento();  
   }  


}

}

interrupt(USCIAB0RX_VECTOR) Receive_Data(void)
{
	volatile unsigned char blink = UCA0RXBUF;
	//P1OUT ^= LED1;
	if(blink==1)
	{            
             parar=0;   
            ////TIMERA1////
            TA1CCR0= 625; //  625*2*8uS= 0,01s 
            TA1CTL = TASSEL_2 + MC_3 + ID_3 +TAIE; // SMCLK  ,  UP/DOWN MODE, /8
            cemsegundos=0;
            decsegundos=0;
            segundos=0;
            minutos=0;

        		
	}

	if(blink==2)
	{
	     parar=0;   
            ////TIMERA0////
            TA1CCR0= 625; //  625*2*8uS= 0,01s 
            TA1CTL = TASSEL_2 + MC_3 + ID_3 +TAIE; // SMCLK  ,  UP/DOWN MODE, /8
            cemsegundos=0;
            decsegundos=0;
            segundos=0;
            minutos=0;
	}

       if(blink==3)
	{
	     parar=0;   
            ////TIMERA0////
            TA1CCR0= 1000-1; //  10MS
            TA1CTL = TASSEL_2 + MC_3 + ID_3 +TAIE; // SMCLK  ,  UP/DOWN MODE, /8
            cemsegundos=0;
            decsegundos=0;
            segundos=0;
            minutos=0;
            
            
	}
	IFG2 &= ~UCA0RXIFG;
}

// INTERRUPÇÃO QUE CONTA A CADA 0,01 SEGUNDOS 
interrupt(TIMER1_A1_VECTOR) TA1_ISR(void)
{
  
  volatile unsigned char blink = UCA0RXBUF;
  
  cemsegundos++;
 
  //////conta o tempo/////////
 if(cemsegundos>=10) {
  cemsegundos=0;    
  decsegundos++;
   if(parar<1){
     //P2OUT=BIT0|BIT2;
      //Send_Data(blink);
      if(blink==1)
      move1();
      else if(blink==2)
      move2();
      else
      move3();
   }
    }
    
   if(decsegundos>=10) {
      decsegundos=0;
      segundos++;
    }
   if(segundos>=5){
   parar=1;
   }
  
   if(segundos>=60){
    segundos=0;
     //P2OUT=BIT0|BIT2;
    minutos++;
  }   
    
   if(minutos>=1){
   minutos=0;
   //parar=1;
   P1OUT&=~BIT3;
   } 
   
  TA1CTL &= ~TAIFG;
}



