################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
compete_get_rms/%.o: ../compete_get_rms/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/TI/ccstheia141/ccs/tools/compiler/ti-cgt-armllvm_3.2.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/34808/workspace_ccstheia/compete_get_rms" -I"C:/Users/34808/workspace_ccstheia/compete_get_rms/system" -I"C:/Users/34808/workspace_ccstheia/compete_get_rms/Debug" -I"C:/TI/mspm0_sdk_2_01_00_03/source/third_party/CMSIS/Core/Include" -I"C:/TI/mspm0_sdk_2_01_00_03/source" -I"C:/TI/mspm0_sdk_2_01_00_03/source/third_party/CMSIS/DSP/Include" -gdwarf-3 -MMD -MP -MF"compete_get_rms/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-72229745: ../compete_get_rms/empty.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/TI/ccstheia141/ccs/utils/sysconfig_1.20.0/sysconfig_cli.bat" --script "C:/Users/34808/workspace_ccstheia/compete_get_rms/compete_get_rms/empty.syscfg" -o "." -s "C:/TI/mspm0_sdk_2_01_00_03/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-72229745 ../compete_get_rms/empty.syscfg
device.opt: build-72229745
device.cmd.genlibs: build-72229745
ti_msp_dl_config.c: build-72229745
ti_msp_dl_config.h: build-72229745
Event.dot: build-72229745

compete_get_rms/startup_mspm0g350x_ticlang.o: C:/TI/mspm0_sdk_2_01_00_03/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/TI/ccstheia141/ccs/tools/compiler/ti-cgt-armllvm_3.2.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/34808/workspace_ccstheia/compete_get_rms" -I"C:/Users/34808/workspace_ccstheia/compete_get_rms/system" -I"C:/Users/34808/workspace_ccstheia/compete_get_rms/Debug" -I"C:/TI/mspm0_sdk_2_01_00_03/source/third_party/CMSIS/Core/Include" -I"C:/TI/mspm0_sdk_2_01_00_03/source" -I"C:/TI/mspm0_sdk_2_01_00_03/source/third_party/CMSIS/DSP/Include" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


