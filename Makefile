#-------------------------------
#sets the uart protocol. Available: 
#  PROTO_SEDU_BUGGY compatible with boblight svn in sedu mode
#  PROTO_SEDU sedu for fixed boblight trees like Speedy1985's boblight-for-raspberry or hyperion
#  PROTO_TPM2 tpm2, it is only supported in hyperion, but it's much cleaner
#  PROTO_JUMPER automatically selects the protocol at startup based on jumper settings
CFLAGS+=-DPROTOCOL=PROTO_JUMPER
#uart baudrate
CFLAGS+= -DBAUD=57600UL
#spi clockrate in kHz
CFLAGS+= -DSPI_CLOCKRATE=125
#sedu protocol need an exact channel count in TPM protocol it is only used for cache size, so it is the maximum size if cache is enabled
CFLAGS+= -DCHANNEL_COUNT=312
#if your spi is slower than your uart, you should enable the cache
# CFLAGS+= -DENABLE_CACHE
#the port for programming with avrdude
PROGRAMMING_PORT=/dev/ttyUSB0

#----------------------------------------
#compilation specific stuff
#optimization. It's usually set to -Os on MCUs, but this code is really small, we can easily use -O2
OPTIMIZATION=-O2
EXTRA_FLAGS=-mno-interrupts -mrelax

#----------------------------------------
#these parts defines the MCU platform. If you're using arduino nano v3.0 (or clones) don't touch it
#mcu and clock
MCU=atmega328p
CFLAGS+= -DF_CPU=16000000
#avrdude flags
DUDEMCU=m328p
DUDEFLAGS=-carduino -P$(PROGRAMMING_PORT) -b57600
#spi ports
CFLAGS+= -DMOSIDDR=DDRB
CFLAGS+= -DMISODDR=DDRB
CFLAGS+= -DSCKDDR=DDRB
CFLAGS+= -DSSDDR=DDRB
#spi port pins
CFLAGS+= -DMOSINUM=PB3
CFLAGS+= -DMISONUM=PB4
CFLAGS+= -DSCKNUM=PB5
CFLAGS+= -DSSNUM=PB2
#jumper pins
CFLAGS+= -DJUMPNUM1=PD2
CFLAGS+= -DJUMPNUM2=PD3
CFLAGS+= -DJUMPNUM3=PD4
CFLAGS+= -DJUMPDDR=DDRD
CFLAGS+= -DJUMPPORT=PORTD
CFLAGS+= -DJUMPPIN=PIND
#----------------------------------------
#definition of available modes
CFLAGS+=-DPROTO_SEDU_BUGGY=1
CFLAGS+=-DPROTO_SEDU=2
CFLAGS+=-DPROTO_TPM2=3
CFLAGS+=-DPROTO_JUMPER=4
#----------------------------------------
#compilation stuff. Don't touch this unless you add a new header
CFLAGS+=$(OPTIMIZATION) $(EXTRA_FLAGS) -mmcu=$(MCU)
all: main.hex

main.hex: main.elf
	avr-objcopy -O ihex main.elf main.hex
main.elf: main.o spi.o uart.o
	avr-gcc $(CFLAGS) -o $@ $?

main.o: main.c 
	avr-gcc $(CFLAGS) -c $<	

uart.o: uart.c 
	avr-gcc $(CFLAGS) -c $<	

spi.o: spi.c 
	avr-gcc $(CFLAGS) -c $<	


prog: main.hex
	avrdude $(DUDEFLAGS) -p$(DUDEMCU) -Uflash:w:main.hex:i 
	#-Ulfuse:w:0xef:m

dumpasm: main.elf
	avr-objdump -h -S main.elf > main.lst

memcheck: main.elf
	avr-objdump -h main.elf

clean:
	rm -f *.o 
	rm -f main.elf 
	rm -f main.hex
