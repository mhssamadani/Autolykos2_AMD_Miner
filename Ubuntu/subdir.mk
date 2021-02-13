################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../conversion.cc \
../cryptography.cc \
../definitions.cc \
../easylogging++.cc \
../httpapi.cc \
../processing.cc \
../request.cc \
../sysfs-gpu-controls.cc 

CPP_SRCS += \
../cpuAutolykos.cpp \
../ErgoOpenCL.cpp \
../clMining.cpp \
../clPreHash.cpp \
../cl_warpper.cpp \
../ergoAutolykos.cpp 

C_SRCS += \
../jsmn.c 

CC_DEPS += \
./conversion.d \
./cryptography.d \
./definitions.d \
./easylogging++.d \
./httpapi.d \
./processing.d \
./request.d \
./sysfs-gpu-controls.d 

OBJS += \
./cpuAutolykos.o \
./ErgoOpenCL.o \
./clMining.o \
./clPreHash.o \
./cl_warpper.o \
./conversion.o \
./cryptography.o \
./definitions.o \
./easylogging++.o \
./ergoAutolykos.o \
./httpapi.o \
./jsmn.o \
./processing.o \
./request.o \
./sysfs-gpu-controls.o 

CPP_DEPS += \
./cpuAutolykos.d \
./ErgoOpenCL.d \
./clMining.d \
./clPreHash.d \
./cl_warpper.d \
./ergoAutolykos.d 

C_DEPS += \
./jsmn.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/amdgpu-pro/lib/x86_64-linux-gnu -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/amdgpu-pro/lib/x86_64-linux-gnu -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


