################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
compete_get_rms/Debug/%.o: ../compete_get_rms/Debug/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/TI/ccstheia141/ccs/tools/compiler/ti-cgt-armllvm_3.2.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/34808/workspace_ccstheia/compete_get_rms" -I"C:/Users/34808/workspace_ccstheia/compete_get_rms/system" -I"C:/Users/34808/workspace_ccstheia/compete_get_rms/Debug" -I"C:/TI/mspm0_sdk_2_01_00_03/source/third_party/CMSIS/Core/Include" -I"C:/TI/mspm0_sdk_2_01_00_03/source" -I"C:/TI/mspm0_sdk_2_01_00_03/source/third_party/CMSIS/DSP/Include" -gdwarf-3 -MMD -MP -MF"compete_get_rms/Debug/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


