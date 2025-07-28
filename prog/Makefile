CC=avr-gcc
LD=avr-ld
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
MCU_TARGET=attiny85
PROGRAMMER=arduino-ft232r
F_CPU=1000000UL
DEFS= -DF_CPU=$(F_CPU)
CFLAGS= -Wall -O2 -mmcu=$(MCU_TARGET) -nostartfiles -ffunction-sections -fdata-sections -g $(DEFS)
LDFLAGS= -mmcu=$(MCU_TARGET) -Wl,-gc-sections
LIB_SRCS=lib/uart.c lib/dbg.c libir/ir_ctrl.c
LIB_OBJS=$(LIB_SRCS:%.c=%.o)
TARGET=irswitch.hex

.SUFFIXES:
.PHONY: all clean upload upload_serial

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.S: %.c
	$(CC) -S -o $@ $< $(CFLAGS)

%.o: %.S
	$(CC) -c -o $@ $< $(CFLAGS)

%.elf: %.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.hex: %.elf
	avr-size -C --mcu=$(MCU_TARGET) $<
	$(OBJDUMP) -h -S $< > $(@:%.hex=%.lst)
	$(OBJCOPY) -R .eeprom -O ihex $< $@

%.upload: %.hex
	@echo "*** NOTE: You may need to ran as root ***"
	avrdude -p $(MCU_TARGET) -c $(PROGRAMMER) -U flash:w:$<:i

%.upload_serial: %.hex
	avrdude -p $(MCU_TARGET) -c arduino -b 19200 -P /dev/ttyUSB0 -U flash:w:$<:i

all: $(TARGET)

upload: $(TARGET:%.hex=%.upload)
upload_serial: $(TARGET:%.hex=%.upload_serial)

$(TARGET:%.hex=%.elf): $(TARGET:%.hex=%.o) $(LIB_OBJS)

clean:
	rm -fv *.o *.elf *.hex *.lst
