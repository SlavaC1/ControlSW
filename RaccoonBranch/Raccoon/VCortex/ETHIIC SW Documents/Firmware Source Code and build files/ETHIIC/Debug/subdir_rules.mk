################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
ETHIIC.obj: ../ETHIIC.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/ETHIIC" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c1294xl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include/ipv4" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps/httpserver_raw" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="ETHIIC.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

cgifuncs.obj: ../cgifuncs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/ETHIIC" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c1294xl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include/ipv4" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps/httpserver_raw" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="cgifuncs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

enet_fs.obj: ../enet_fs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/ETHIIC" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c1294xl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include/ipv4" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps/httpserver_raw" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="enet_fs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

io.obj: ../io.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/ETHIIC" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c1294xl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include/ipv4" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps/httpserver_raw" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="io.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup_ccs.obj: ../startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/ETHIIC" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c1294xl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include/ipv4" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps/httpserver_raw" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

swupdate.obj: ../swupdate.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/ETHIIC" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c1294xl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/src/include/ipv4" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/lwip-1.4.1/apps/httpserver_raw" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --display_error_number --diag_warning=225 --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="swupdate.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

