################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ELFSIZE += \
lab_5_sw.elf.size \


# All Target
all: lab_5_sw.elf secondary-outputs

# Tool invocations
lab_5_sw.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MicroBlaze gcc linker'
	mb-gcc -Wl,-T -Wl,../src/lscript.ld -LU:/EECE387/Lab5/workspace/design_1_wrapper/export/design_1_wrapper/sw/design_1_wrapper/standalone_microblaze_0/bsplib/lib -mlittle-endian -mcpu=v11.0 -mxl-soft-mul -Wl,--no-relax -Wl,--gc-sections -o "lab_5_sw.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

lab_5_sw.elf.size: lab_5_sw.elf
	@echo 'Invoking: MicroBlaze Print Size'
	mb-size lab_5_sw.elf  |tee "lab_5_sw.elf.size"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(S_UPPER_DEPS)$(C_DEPS)$(ELFSIZE) lab_5_sw.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE)

.PHONY: all clean dependents

-include ../makefile.targets
