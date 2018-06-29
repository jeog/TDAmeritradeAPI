################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/streaming/streaming_info.cpp \
../src/streaming/streaming_session.cpp \
../src/streaming/streaming_subscriptions.cpp 

OBJS += \
./src/streaming/streaming_info.o \
./src/streaming/streaming_session.o \
./src/streaming/streaming_subscriptions.o 

CPP_DEPS += \
./src/streaming/streaming_info.d \
./src/streaming/streaming_session.d \
./src/streaming/streaming_subscriptions.d 


# Each subdirectory must supply rules for building sources it contributes
src/streaming/%.o: ../src/streaming/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTHIS_EXPORTS_INTERFACE -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


