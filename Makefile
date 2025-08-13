TARGET = firmware
BUILD_DIR = build

# Toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

# MCU specs
MCU = cortex-m3

# Include paths
INCLUDES = -Iinc -Iinc/core

# C flags
CFLAGS  = $(INCLUDES) -mcpu=$(MCU) -mthumb 
CFLAGS += --specs=nosys.specs --specs=nano.specs
CFLAGS += -static -mfloat-abi=soft 
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -O0 -g 

# Linker flags
LDFLAGS = -T linker.ld $(CFLAGS) -Wl,--gc-sections -lm -lc
LDFLAGS += -Wl,--print-memory-usage -Wl,-Map=$(BUILD_DIR)/$(TARGET).map 

# Sources
C_SOURCES  = src/main.c
C_SOURCES += src/sysmem.c src/syscalls.c
C_SOURCES += src/crsf.c
C_SOURCES += src/motors.c
C_SOURCES += src/systick.c
C_SOURCES += src/uart.c


ASM_SOURCES = startup.s

# Objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))

all: $(BUILD_DIR)/$(TARGET).elf
	$(CP) -O binary $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(SZ) $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

flash: $(BUILD_DIR)/$(TARGET).elf
	openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg \
	-c "program $< verify reset exit"

.PHONY: all clean flash

openocd:
	openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg

gdb:
	gdb-multiarch $(BUILD_DIR)/$(TARGET).elf
