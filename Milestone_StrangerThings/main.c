/***********************************
* msp430G2553 milestone 1
*
* Richard Dell
* Created: 10/14/19
* Last Updated: 10/15/19
*
***********************************/
#include <msp430.h>
#include <stdint.h>

volatile uint8_t byte = 0; //byte being looked at
volatile uint8_t len = 0; //length of message
volatile uint8_t i = 0; //location of current byte
volatile int RED_LED = 0; //red LED duty cycle
volatile int GREEN_LED = 0; //green LED duty cycle
volatile int BLUE_LED = 0; //blue LED duty cycle


int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  P1DIR |= BIT6; //set P1.6 to output
  P1SEL |= BIT6; //set P1.6 to use PWM
  P2DIR |= BIT1; //set P2.1 to output
  P2SEL |= BIT1; //set P2.1 to use PWM
  P2DIR |= BIT5; //set P2.5 to output
  P2DIR |= BIT5; //set P2.5 to use PWM

  TA0CTL |= TASSEL_2; //select SMCLK
  TA0CTL |= MC_1; //set to up mode
  TA0CCR0 = 255; //max pwm value 16^2 -1
  TA0CCR1 = RED_LED; //set interrupt for red duty cycle
  TA0CCTL1 |= OUTMOD_7; //set pwm output mode to mode 7


  TA1CTL |= TASSEL_2; //select SMCLK
  TA1CTL |= MC_1; //set to up mode
  TA1CCR0 = 255; //max pwm value 16^2 -1
  TA1CCR1 = GREEN_LED; //set interrupt for green duty cycle
  TA1CCTL1 |= OUTMOD_7; //set pwm output mode to mode 7

  TA2CTL |= TASSEL_2; //select SMCLK
  TA2CTL |= MC_1; //set to up mode
  TA2CCR0 = 255; //max pwm value 16^2 -1
  TA2CCR1 = BLUE_LED; //set interrupt for blue duty cycle
  TA2CCTL1 |= OUTMOD_7; //set pwm output mode to mode 7


  if (CALBC1_1MHZ==0xFF) // If calibration constant erased
  {
    while(1);                               // do not load, trap CPU!!
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 104;                            // 1MHz 9600
  UCA0BR1 = 0;                              // 1MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
}

//don't think this is needed
//  Echo back RXed character, confirm TX buffer is ready first
//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCI0RX_ISR(void)
//#elif defined(__GNUC__)


void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
{
    byte = UCA0RXBUF;   //save message to byte variable

    if(i==0)
        length = byte;          //save the number of bytes in the transmission
    else if(i==1)
        RED_LED = byte;         //set red LED duty cycle
    else if(i==2)
        GREEN_LED = byte;       //set green LED duty cycle
    else if(i==3)
    {
        BLUE_LED = byte;        //set blue LED duty cycle
        UCA0TXBUF = byte;       //send byte
    }
    else
        UCA0TXBUF = byte;       //send byte

    if(i<(length-1))
        i++;
    else
        i=0;
}
