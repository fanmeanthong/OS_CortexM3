# Makefile để build dự án Blink LED cho STM32F103 (no HAL/SPL)

# Tên file đầu ra
TARGET = blinkled

# Trình biên dịch và các flags
CC      = arm-none-eabi-gcc
CFLAGS  = -mcpu=cortex-m3 -mthumb -O0 -g -Wall -ffreestanding -nostdlib \
			-ISPL/inc \
			-ICore \
			-DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER
LDFLAGS = -T stm32f103.ld -nostdlib -Wl,--gc-sections

# Danh sách file nguồn
SRCS_C := main.c $(wildcard SPL/src/*.c)
SRCS_S := startup_stm32f103.s $(wildcard SPL/asm/*.s)
SRCS   := $(SRCS_C) $(SRCS_S)
OBJS   := $(SRCS:.c=.o)
OBJS   := $(OBJS:.s=.o)

# Mục tiêu mặc định
all: $(TARGET).bin

# Compile file C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble file ASM
%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

# Link thành ELF
$(TARGET).elf: $(OBJS) stm32f103.ld
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Tạo file .bin từ .elf
$(TARGET).bin: $(TARGET).elf
	arm-none-eabi-objcopy -O binary $< $@

# Nạp firmware vào Blue Pill (dùng file .bin)
flash: $(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(TARGET).bin 0x08000000 verify reset exit"

# Xóa file tạm
clean:
	del /Q *.o *.elf *.bin 2>nul
	del /Q SPL\\src\\*.o 2>nul

.PHONY: all clean flash
