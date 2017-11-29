################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DeviceFactory.cpp \
../src/DhtSensor.cpp \
../src/MqttDevice.cpp \
../src/PubSubClient.cpp \
../src/SonoffSwitch.cpp \
../src/Trace.cpp \
../src/sensorTest.cpp 

OBJS += \
./src/DeviceFactory.o \
./src/DhtSensor.o \
./src/MqttDevice.o \
./src/PubSubClient.o \
./src/SonoffSwitch.o \
./src/Trace.o \
./src/sensorTest.o 

CPP_DEPS += \
./src/DeviceFactory.d \
./src/DhtSensor.d \
./src/MqttDevice.d \
./src/PubSubClient.d \
./src/SonoffSwitch.d \
./src/Trace.d \
./src/sensorTest.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


