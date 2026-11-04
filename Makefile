TARGET  = shannon
BUILD   = build

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU     = -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
INCLUDES= -Ithird_party/cmsis -Ibsp -Ilib
DEFINES = -DSTM32H7A3xxQ

ASFLAGS = $(CPU) -g3
CFLAGS  = $(CPU) -g3 -O0 -Wall -Wextra -ffreestanding -ffunction-sections -fdata-sections $(INCLUDES) $(DEFINES)
LDFLAGS = $(CPU) -nostdlib -T bsp/h7a3.ld -Wl,-Map=$(BUILD)/$(TARGET).map -Wl,--gc-sections

OBJS    = $(BUILD)/bsp/startup.o \
          $(BUILD)/bsp/board.o   \
          $(BUILD)/bsp/uart.o    \
          $(BUILD)/lib/kprintf.o \
          $(BUILD)/main.o 

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin

$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@


$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


$(BUILD)/$(TARGET).elf: $(OBJS) bsp/h7a3.ld
	$(CC) $(LDFLAGS) $(OBJS) -o $@
	$(SIZE) $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD):
	mkdir -p $@

flash: $(BUILD)/$(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32h7x.cfg -c "program $< verify reset exit"

clean:
	rm -rf $(BUILD)

.PHONY: all flash clean
