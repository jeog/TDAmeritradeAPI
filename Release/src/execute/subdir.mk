################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/execute/order_hub.cpp \
../src/execute/order_leg.cpp \
../src/execute/order_ticket.cpp 

OBJS += \
./src/execute/order_hub.o \
./src/execute/order_leg.o \
./src/execute/order_ticket.o 

CPP_DEPS += \
./src/execute/order_hub.d \
./src/execute/order_leg.d \
./src/execute/order_ticket.d 


# Each subdirectory must supply rules for building sources it contributes
src/execute/%.o: ../src/execute/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTHIS_EXPORTS_INTERFACE -DNDEBUG -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


