TARGET  = shannon
BUILD   = build

CC      = arm-none-eabi-gcc
HOST_CC = gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU     = -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
INCLUDES= -Ithird_party/cmsis -Ibsp -Ilib -Iarch -Ikernel
DEFINES = -DSTM32H7A3xxQ

ASFLAGS = $(CPU) -g3
CFLAGS  = $(CPU) -g3 -O0 -Wall -Wextra -ffreestanding -ffunction-sections -fdata-sections $(INCLUDES) $(DEFINES)
LDFLAGS = $(CPU) -nostdlib -T bsp/h7a3.ld -Wl,-Map=$(BUILD)/$(TARGET).map -Wl,--gc-sections

OBJS    = $(BUILD)/bsp/startup.o \
          $(BUILD)/bsp/board.o   \
          $(BUILD)/bsp/uart.o    \
          $(BUILD)/bsp/timer.o    \
          $(BUILD)/lib/kprintf.o \
					$(BUILD)/bsp/timing.o  \
          $(BUILD)/arch/fault_entry.o \
					$(BUILD)/arch/context.o \
					$(BUILD)/tests/target/fpu_asm.o \
					$(BUILD)/arch/fault.o \
					$(BUILD)/arch/port.o \
					$(BUILD)/arch/fpu.o \
          $(BUILD)/kernel/task.o  \
					$(BUILD)/kernel/sched.o \
					$(BUILD)/kernel/idle.o \
					$(BUILD)/kernel/time.o \
					$(BUILD)/kernel/wait.o \
					$(BUILD)/kernel/semaphore.o \
					$(BUILD)/kernel/mutex.o \
					$(BUILD)/kernel/mqueue.o \
          $(BUILD)/main.o 

ifeq ($(TEST),fpu)
OBJS    += $(BUILD)/tests/target/fpu_asm.o
CFLAGS  += -DENABLE_FPU_TEST
endif

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

test-target:
	$(MAKE) clean
	$(MAKE) TEST=fpu all

test:
	@mkdir -p $(BUILD)/host
	$(HOST_CC) -std=c11 -O2 -g -Wall -Wextra -Ikernel tests/host/test_list.c -o $(BUILD)/host/test_list
	./$(BUILD)/host/test_list

clean:
	rm -rf $(BUILD)

.PHONY: all flash clean test test-target
