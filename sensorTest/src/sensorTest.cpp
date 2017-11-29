//============================================================================
// Name        : sensorTest.cpp
// Author      : S. Wink
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "MqttDevice.h"
#include "Trace.h"
#include "DeviceFactory.h"

using namespace std;

int main()
{
    Trace myLogger(true, 0);
    PubSubClient client;
    //DhtSensor dht(&myLogger);
    //SonoffSwitch mySwitch(&myLogger);
    DeviceFactory factory(&myLogger);
    MqttDevice *p_genSensor;


    myLogger.println(trace_PURE_MSG, "!!!Hello World!!!"); // prints !!!Hello World!!!

	//p_genSensor = &dht;
    p_genSensor = factory.GenerateDevice(1);
	p_genSensor->Initialize();
	p_genSensor->Reconnect();
	p_genSensor->CallbackMqtt(&client, "This is a topic", "This is the payload");
	p_genSensor->ProcessPublishRequests(&client);

    //p_genSensor = &mySwitch;
    p_genSensor = factory.GenerateDevice(0);
    p_genSensor->Initialize();
    p_genSensor->Reconnect();
    p_genSensor->CallbackMqtt(&client, "This is a switch topic", "This is the payload.");
    p_genSensor->ProcessPublishRequests(&client);

	return 0;
}
