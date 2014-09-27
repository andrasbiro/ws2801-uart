#ifndef __UART_H__
#define __UART_H__
#include <inttypes.h>
#include <avr/io.h>

//baud is calculated with libc from the BAUD and F_CPU consts
void initUart();

unsigned char readChar();

void writeChar( unsigned char data );

#endif