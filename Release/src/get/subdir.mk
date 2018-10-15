################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/get/account.cpp \
../src/get/get.cpp \
../src/get/historical.cpp \
../src/get/instrument_info.cpp \
../src/get/market_hours.cpp \
../src/get/movers.cpp \
../src/get/options.cpp \
../src/get/quotes.cpp 

OBJS += \
./src/get/account.o \
./src/get/get.o \
./src/get/historical.o \
./src/get/instrument_info.o \
./src/get/market_hours.o \
./src/get/movers.o \
./src/get/options.o \
./src/get/quotes.o 

CPP_DEPS += \
./src/get/account.d \
./src/get/get.d \
./src/get/historical.d \
./src/get/instrument_info.d \
./src/get/market_hours.d \
./src/get/movers.d \
./src/get/options.d \
./src/get/quotes.d 


# Each subdirectory must supply rules for building sources it contributes
src/get/%.o: ../src/get/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTHIS_EXPORTS_INTERFACE -DNDEBUG -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


