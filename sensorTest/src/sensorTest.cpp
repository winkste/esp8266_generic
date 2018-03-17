//============================================================================
// Name        : sensorTest.cpp
// Author      : S. Wink
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "stdint.h"

#include <iostream>
#include "MqttDevice.h"
#include "Trace.h"
#include "TraceSerial.h"
#include "TraceErr.h"
#include "DeviceFactory.h"
#include "RfProcl.h"

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

    p_genSensor = factory.GenerateDevice(2);
    p_genSensor->Initialize();
    p_genSensor->Reconnect();
    p_genSensor->CallbackMqtt(&client, "This is a switch topic MCP", "This is the payload.");
    p_genSensor->ProcessPublishRequests(&client);
    p_genSensor->ProcessPublishRequests(&client);
    p_genSensor->ProcessPublishRequests(&client);

    p_genSensor = factory.GenerateDevice(3);
    p_genSensor->Initialize();
    p_genSensor->Reconnect();
    p_genSensor->CallbackMqtt(&client, "This is a switch topic ESP", "This is the payload.");
    p_genSensor->ProcessPublishRequests(&client);
    p_genSensor->ProcessPublishRequests(&client);
    p_genSensor->ProcessPublishRequests(&client);

    msg_t myMessage;

    RfProcl::InitializeMessage(&myMessage);
    RfProcl::SetFromNodeId(&myMessage, 0x03);
    RfProcl::SetToNodeId(&myMessage, 0x02);
    RfProcl::SetMsgTypeId(&myMessage, 0x01);
    RfProcl::SetMsgData(&myMessage, 0xa5a5);
    RfProcl::CalculateChkSum(&myMessage);
    RfProcl::VerifyMessage(&myMessage);

    RfProcl::InitializeMessage(&myMessage);
    RfProcl::SetFromNodeId(&myMessage, FROM_NODE_ID_03);
    RfProcl::SetToNodeId(&myMessage, TO_NODE_ID_03);
    RfProcl::SetMsgTypeId(&myMessage, MSG_ID_03);
    RfProcl::SetMsgData(&myMessage, 0xa5a5);
    RfProcl::CalculateChkSum(&myMessage);
    cout << RfProcl::GetRawData(&myMessage) << endl;

    cout << "Msg verified: " << RfProcl::VerifyMessage(&myMessage) << endl;
    cout << "FromNode Id: " << (uint32_t)RfProcl::GetFromNodeId(&myMessage) << endl;
    cout << "ToNode Id: " << (uint32_t)RfProcl::GetToNodeId(&myMessage) << endl;
    cout << "Msg Id: " << (uint32_t)RfProcl::GetMsgTypeId(&myMessage) << endl;
    cout << "Msg Data: " << (uint32_t)RfProcl::GetMsgData(&myMessage) << endl;
    cout << "Raw Msg: " << (uint32_t)RfProcl::GetRawData(&myMessage) << endl;
    RfProcl::SetMsgTypeId(&myMessage, MSG_ID_02);
    RfProcl::SetFromNodeId(&myMessage, FROM_NODE_ID_02);
    RfProcl::SetToNodeId(&myMessage, TO_NODE_ID_02);
    cout << "FromNode Id: " << (uint32_t)RfProcl::GetFromNodeId(&myMessage) << endl;
    cout << "ToNode Id: " << (uint32_t)RfProcl::GetToNodeId(&myMessage) << endl;
    cout << "Msg Id: " << (uint32_t)RfProcl::GetMsgTypeId(&myMessage) << endl;
    RfProcl::SetMsgTypeId(&myMessage, MSG_ID_01);
    RfProcl::SetFromNodeId(&myMessage, FROM_NODE_ID_01);
    RfProcl::SetToNodeId(&myMessage, TO_NODE_ID_01);
    cout << "FromNode Id: " << (uint32_t)RfProcl::GetFromNodeId(&myMessage) << endl;
    cout << "ToNode Id: " << (uint32_t)RfProcl::GetToNodeId(&myMessage) << endl;
    cout << "Msg Id: " << (uint32_t)RfProcl::GetMsgTypeId(&myMessage) << endl;

    myLogger_p->print(trace_INFO_MSG, "This is a first RfProtocol Test: FromNode: ");
    myLogger_p->print(trace_PURE_MSG, RfProcl::GetFromNodeId(&myMessage));
    myLogger_p->print(trace_PURE_MSG, "; ToNode: ");
    myLogger_p->print(trace_PURE_MSG, RfProcl::GetToNodeId(&myMessage));
    myLogger_p->print(trace_PURE_MSG, "; MsgType: ");
    myLogger_p->print(trace_PURE_MSG, RfProcl::GetMsgTypeId(&myMessage));
    myLogger_p->print(trace_PURE_MSG, "; MsgData: ");
    myLogger_p->print(trace_PURE_MSG, RfProcl::GetMsgData(&myMessage));
    myLogger_p->print(trace_PURE_MSG, "; RawData: ");
    myLogger_p->print(trace_PURE_MSG, RfProcl::GetRawData(&myMessage));

	return 0;
}
