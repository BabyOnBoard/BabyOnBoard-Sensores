#include <msp430g2553.h>
#include <legacymsp430.h>

// Codigo para o MSP430 configurado como
// escravo SPI

#define MISS BIT1
#define MOSS BIT2
#define SCLK BIT4

#define MOTORB1 (BIT4|BIT5|BIT6|BIT7)
#define MOTORB2 (BIT0|BIT1|BIT2|BIT3)

void Atraso(volatile unsigned int x)
{
	while(x--);
}

void Send_Data(volatile unsigned char c)
{
	while((IFG2&UCA0TXIFG)==0);
	UCA0TXBUF = c;
}



void move1() {


          P2OUT&= ~MOTORB1;
          //P2OUT|= 0x00000050;
          P2OUT=BIT4;
          P1OUT=BIT6;
        Atraso(10000000);
       
          P2OUT&= ~MOTORB1;
          //P2OUT|=0x00000060;
          P2OUT=BIT5;
          P1OUT=BIT6;
        Atraso(10000000);
      
          P2OUT&= ~MOTORB1;
          //P2OUT|=0x00000100; 
          P2OUT=BIT5;
          P1OUT=BIT7;
          Atraso(10000000);
        
          P2OUT&= ~MOTORB1;
          //P2OUT|=0x00000090;
          P2OUT=BIT4;
          P1OUT=BIT7;
          Atraso(10000000);
        
        
          P1OUT&= ~MOTORB1;

          P2OUT&= ~MOTORB1;

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

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	P1SEL2 = P1SEL = MOSS + MISS + SCLK;

        P1DIR|= BIT7|BIT6;
        P1OUT =~(BIT7|BIT6);
        P2DIR|= (MOTORB1|MOTORB2);
        P2OUT=~(MOTORB1|MOTORB2);

	UCA0CTL1 = UCSWRST + UCSSEL_3;
	UCA0CTL0 = UCCKPH+UCMSB+UCMODE_0+UCSYNC;
	UCA0CTL1 &= ~UCSWRST;

	//Send_Data(10);
	IE2 |= UCA0RXIE;
	_BIS_SR(LPM0_bits + GIE);
	return 0;
}

interrupt(USCIAB0RX_VECTOR) Receive_Data(void)
{
	volatile unsigned char blink = UCA0RXBUF;
	//P1OUT ^= LED1;
	if(blink==1)
	{            
                 //Send_Data(blink);
                 move1();
                   
		
	}

	if(blink==2)
	{
	           
                 move2();
                 //Send_Data(blink);     
		
	}
	IFG2 &= ~UCA0RXIFG;
}

