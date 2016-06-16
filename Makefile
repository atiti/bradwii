TARGET=bradwii-md3

# Bradwii sources
OBJS += src/accelerometer.o	\
	src/autotune.o 		\
	src/bradwii.o 		\
	src/checkboxes.o 	\
	src/eeprom.o 		\
	src/gyro.o 		\
	src/imu.o		\
	src/leds.o 		\
	src/output.o 		\
	src/pilotcontrol.o 	\
	src/serial.o 		\
	src/rx.o 		\
	src/vectors.o

# Hardware abstraction layer code
HAL_OBJS += lib/hal/drv_hal.o 		\
	    lib/hal/drv_gpio.o 		\
	    lib/hal/drv_pwm.o		\
	    lib/hal/drv_serial.o	\
	    lib/hal/lib_digitalio.o 	\
	    lib/hal/lib_i2c.o 		\
	    lib/hal/lib_timers.o 	\
	    lib/hal/lib_fp.o 		\
	    lib/hal/lib_serial.o

# Platform specific code
STM32F0_OBJS += lib/startup_stm32f0xx.o 						\
		lib/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_tim.o 			\
		lib/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_dma.o 			\
		lib/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_rcc.o 			\
		lib/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_gpio.o 			\
		lib/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_usart.o 			\
		lib/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_misc.o 			\
		lib/CMSIS/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.o 

OBJS += $(HAL_OBJS) $(STM32F0_OBJS)

STM32F0_INCLUDES = -Isrc/ -Ilib/hal -Ilib/STM32F0xx_StdPeriph_Driver/inc/ -Ilib/CMSIS/Include/ -Ilib/CMSIS/Device/ST/STM32F0xx/Include/

# Compiler flags
CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
CFLAGS += -mcpu=cortex-m0 -mthumb -Os -ffunction-sections -fno-common -fno-builtin -Wall -g -DMD3_BUILD -DDEBUG_ENABLE_SEMIHOST -DUSE_STDPERIPH_DRIVER


# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += $(TARGET).hex
SECONDARY_SIZE += $(TARGET).siz

# All Target
all: $(TARGET).siz $(TARGET).hex

# Tool invocations

%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	$(CC) $(CFLAGS) $(STM32F0_INCLUDES) -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"

$(TARGET): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	$(CC) $(CFLAGS) -T lib/stm32.ld -nostartfiles -Xlinker --gc-sections -Wl,-Map,"$(TARGET).map" -o $(TARGET) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(TARGET).hex: $(TARGET)
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	$(CROSS_COMPILE)objcopy -O ihex --strip-unneeded $(TARGET)  $(TARGET).hex
	@echo 'Finished building: $@'
	@echo ' '

$(TARGET).siz: $(TARGET)
	@echo 'Invoking: Cross ARM GNU Print Size'
	$(CROSS_COMPILE)size --format=berkeley $(TARGET)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-rm -f $(SECONDARY_SIZE) $(OBJS) $(OBJS:.o=.d) $(SECONDARY_FLASH) $(TARGET) $(TARGET).map
	-@echo ' '

# TODO: package up openocd changes
flash: $(TARGET)
	-openocd -c "init; halt; flash write_image erase $(TARGET) 0; reset; shutdown"

debug:
	-openocd -c "init; halt; arm semihosting enable; reset run"

.PHONY: all clean dependents
.SECONDARY:
