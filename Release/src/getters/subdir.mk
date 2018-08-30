################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/getters/account.cpp \
../src/getters/historical.cpp \
../src/getters/instrument_info.cpp \
../src/getters/market_hours.cpp \
../src/getters/movers.cpp \
../src/getters/options.cpp \
../src/getters/quotes.cpp 

OBJS += \
./src/getters/account.o \
./src/getters/historical.o \
./src/getters/instrument_info.o \
./src/getters/market_hours.o \
./src/getters/movers.o \
./src/getters/options.o \
./src/getters/quotes.o 

CPP_DEPS += \
./src/getters/account.d \
./src/getters/historical.d \
./src/getters/instrument_info.d \
./src/getters/market_hours.d \
./src/getters/movers.d \
./src/getters/options.d \
./src/getters/quotes.d 


# Each subdirectory must supply rules for building sources it contributes
src/getters/%.o: ../src/getters/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTHIS_EXPORTS_INTERFACE -DNDEBUG -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


