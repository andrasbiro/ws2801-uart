#include "uart.h"
#include <util/setbaud.h>

void initUart(){
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  #if USE_2X
    UCSR0A |= (1 << U2X0);
  #else
    UCSR0A &= ~(1 << U2X0);
  #endif
  UCSR0B = ( ( 1 << RXEN0 ) | ( 1 << TXEN0 ) ); 
  UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

unsigned char readChar(){
  /* Wait for incomming data */
  while ( !(UCSR0A & (1<<RXC0)) )   
    ;                     
  /* Return the data */
  return UDR0;
}

void writeChar( unsigned char data )
{
  /* Wait for empty transmit buffer */
  while ( !(UCSR0A & (1<<UDRE0)) )
    ;                       
  /* Start transmittion */
  UDR0 = data;              
}