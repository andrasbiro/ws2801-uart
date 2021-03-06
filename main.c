#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/io.h>

#include "spi.h"
#include "uart.h"
#ifdef ENABLE_CACHE
uint8_t cache[CHANNEL_COUNT];

void readToLed(uint16_t count){
  uint16_t currentChannel;
  for( currentChannel = 0; currentChannel < count; currentChannel++){
    cache[currentChannel] = readChar();
  }
  for( currentChannel = 0; currentChannel < count; currentChannel++){
    spiWrite(cache[currentChannel]);
    spiRead();
  }
}
#else
void readToLed(uint16_t count){
  uint16_t currentChannel;
  for( currentChannel = 0; currentChannel < count; currentChannel++){
    spiWrite(readChar());
    spiRead();
  }
}
#endif


void buggySeduLoop(){
  uint8_t prefix[2] = {0xa5, 0x5a};
  for(;;){
    uint8_t first = 0, second = 0;
    while( !(first == prefix[0] && second == prefix[1]) ){
      first = second;
      second = readChar();
    }
    readToLed(CHANNEL_COUNT);
    readChar();//postfix - in theory, the packet should be padded, so there might be much more characters, it doesn't really matter. But there is one at least
  }
}

void seduLoop(){
  uint8_t prefix = 0x5a;
  uint8_t prefix2[5] = {0xa1, 0xa2, 0xb0, 0xb1, 0xb2};//prefix2 should code the packet length, but we couldn't override the led states anyway, so who cares
  for(;;){
    uint8_t first = 0, second = 0;
    while( !(first == prefix && (second == prefix2[0] || second == prefix2[1] || second == prefix2[2] || second == prefix2[3] || second == prefix2[4])) ){
      first = second;
      second = readChar();
    }
    readToLed(CHANNEL_COUNT);
    readChar();//postfix - in theory, the packet should be padded, so there might be much more characters, it doesn't really matter. But there is one at least
  }
}

void tpm2Loop(){
  uint8_t prefix[2] = {0xc9, 0xda}; //the second byte is type, but we're only accepting data frames (0xCA would be command and 0xAA would be requested response)
  for(;;){
    uint16_t channelCount;
    uint8_t first = 0, second = 0;
    while( !(first == prefix[0] && second == prefix[1]) ){
      first = second;
      second = readChar();
    }
    channelCount = readChar()<<8;
    channelCount |= readChar();
    readToLed(channelCount);
    readChar();//postfix
  }
}

//TODO add a relay output to switch on/off the PSU of the leds

uint8_t getProtoJumper(){
  uint8_t ret;
  //set the middle one to output low, the others to input with pull-up
  JUMPDDR |= 1<JUMPNUM2;
  JUMPPORT &= ~(1<<JUMPNUM2);
  JUMPPORT |= 1<JUMPNUM1 | 1<JUMPNUM3;
  
  if( JUMPPIN & (1<<JUMPNUM1) ){
    ret=PROTO_SEDU_BUGGY;
  } else if( JUMPPIN & (1<<JUMPNUM3) ){
    ret=PROTO_SEDU;
  } else {
    ret=PROTO_TPM2;
  }
  
  JUMPDDR &= ~(1<<JUMPNUM2);
  JUMPPORT &= ~(1<JUMPNUM1 | 1<JUMPNUM2 | 1<JUMPNUM3);
  return ret;
}

int main(void){
  #if PROTOCOL==PROTO_JUMPER
  uint8_t autoproto;
  #endif

  MCUSR = 0; 
  wdt_disable();
  CLKPR = 0x80;
  CLKPR = 0x00; 

  initUart();
  
  spiInitMaster(SPI_CLOCKRATE);
  #if PROTOCOL==PROTO_SEDU_BUGGY
  buggySeduLoop();
  #elif  PROTOCOL==PROTO_SEDU
  seduLoop();
  #elif PROTOCOL==PROTO_TPM2
  tpm2Loop();
  #elif PROTOCOL==PROTO_JUMPER
  autoproto = getProtoJumper();
  if( autoproto == PROTO_SEDU_BUGGY )
		buggySeduLoop();
  else if( autoproto == PROTO_SEDU )
		seduLoop();
  else if( autoproto == PROTO_TPM2 )
		tpm2Loop();
  #endif
  return 0;
}
