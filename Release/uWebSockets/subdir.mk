################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../uWebSockets/Epoll.cpp \
../uWebSockets/Extensions.cpp \
../uWebSockets/Group.cpp \
../uWebSockets/HTTPSocket.cpp \
../uWebSockets/Hub.cpp \
../uWebSockets/Networking.cpp \
../uWebSockets/Node.cpp \
../uWebSockets/Room.cpp \
../uWebSockets/Socket.cpp \
../uWebSockets/WebSocket.cpp 

OBJS += \
./uWebSockets/Epoll.o \
./uWebSockets/Extensions.o \
./uWebSockets/Group.o \
./uWebSockets/HTTPSocket.o \
./uWebSockets/Hub.o \
./uWebSockets/Networking.o \
./uWebSockets/Node.o \
./uWebSockets/Room.o \
./uWebSockets/Socket.o \
./uWebSockets/WebSocket.o 

CPP_DEPS += \
./uWebSockets/Epoll.d \
./uWebSockets/Extensions.d \
./uWebSockets/Group.d \
./uWebSockets/HTTPSocket.d \
./uWebSockets/Hub.d \
./uWebSockets/Networking.d \
./uWebSockets/Node.d \
./uWebSockets/Room.d \
./uWebSockets/Socket.d \
./uWebSockets/WebSocket.d 


# Each subdirectory must supply rules for building sources it contributes
uWebSockets/%.o: ../uWebSockets/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DTHIS_EXPORTS_INTERFACE -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


