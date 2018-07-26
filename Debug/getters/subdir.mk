################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../getters/account.cpp \
../getters/historical.cpp \
../getters/instrument_info.cpp \
../getters/market_hours.cpp \
../getters/movers.cpp \
../getters/options.cpp \
../getters/quotes.cpp 

OBJS += \
./getters/account.o \
./getters/historical.o \
./getters/instrument_info.o \
./getters/market_hours.o \
./getters/movers.o \
./getters/options.o \
./getters/quotes.o 

CPP_DEPS += \
./getters/account.d \
./getters/historical.d \
./getters/instrument_info.d \
./getters/market_hours.d \
./getters/movers.d \
./getters/options.d \
./getters/quotes.d 


# Each subdirectory must supply rules for building sources it contributes
getters/%.o: ../getters/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTHIS_EXPORTS_INTERFACE -O0 -g3 -Wall -c -fmessage-length=0 -DDEBUG -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


