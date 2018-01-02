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
#include "TraceSerial.h"
#include "TraceErr.h"
#include "DeviceFactory.h"

using namespace std;


template <typename T> // this is the template parameter declaration
T max(T x, T y)
{
    return (x > y) ? x : y;
}

int main()
{
    TraceSerial myLogger(true, 0);
    TraceErr myErr(true, 0);
    Trace *myLogger_p = &myErr;
    //Trace *myLogger_p = &myLogger;
    PubSubClient client;
    //DhtSensor dht(&myLogger);
    //SonoffSwitch mySwitch(&myLogger);
    DeviceFactory factory(myLogger_p);
    MqttDevice *p_genSensor;


    myLogger_p->println(trace_PURE_MSG, "!!!Hello World!!!"); // prints !!!Hello World!!!

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
