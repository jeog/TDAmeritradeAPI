################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/account.cpp \
../src/auth.cpp \
../src/curl_connect.cpp \
../src/data.cpp \
../src/params.cpp \
../src/query.cpp \
../src/tdma_connect.cpp \
../src/trade.cpp \
../src/util.cpp \
../src/websocket_connect.cpp 

OBJS += \
./src/account.o \
./src/auth.o \
./src/curl_connect.o \
./src/data.o \
./src/params.o \
./src/query.o \
./src/tdma_connect.o \
./src/trade.o \
./src/util.o \
./src/websocket_connect.o 

CPP_DEPS += \
./src/account.d \
./src/auth.d \
./src/curl_connect.d \
./src/data.d \
./src/params.d \
./src/query.d \
./src/tdma_connect.d \
./src/trade.d \
./src/util.d \
./src/websocket_connect.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -DDEBUG -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


