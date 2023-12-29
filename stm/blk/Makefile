TARGET = baremetal_blink
MCU = cortex-m3
SRC = blink.c
OBJ = $(SRC:.c=.o)

CFLAGS = -mthumb -mcpu=$(MCU) -Wall -std=c99
LDFLAGS = -TSTM32F103C8Tx_FLASH.ld
LDLIBS = -lm

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

all: $(TARGET).hex

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)
	$(SIZE) $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $^ $@

flash: $(TARGET).hex
	st-flash write $^ 0x8000000

clean:
	rm -f $(TARGET).elf $(TARGET).hex

.PHONY: all flash clean

