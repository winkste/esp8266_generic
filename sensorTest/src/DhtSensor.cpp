/*****************************************************************************************
* FILENAME :        Sensor.cpp
*
* DESCRIPTION :
*       Abstract Sensor Class
*
* PUBLIC FUNCTIONS :
*
*
* NOTES :
*
*
* Copyright (c) [2017] [Stephan Wink]
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
vAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* AUTHOR :    Stephan Wink        START DATE :    01.10.2017
*
*****************************************************************************************/

/****************************************************************************************/
/* Include Interfaces */
#include "DhtSensor.h"

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"

/****************************************************************************************/
/* Local constant defines */

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for DhtSensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DhtSensor::DhtSensor(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0;
    this->publications_u16 = 0;

}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DhtSensor::~DhtSensor()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the DHT Sensor object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void DhtSensor::Initialize()
{
    p_trace->println(trace_INFO_MSG, "DHT Sensor initialized");
    this->isInitialized_bol = true;
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to initialize the MQTT interface for this sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void DhtSensor::Reconnect()
{
    p_trace->println(trace_INFO_MSG, "DHT Sensor reconnected");
    this->isConnected_bol = true;
}

/**---------------------------------------------------------------------------------------
 * @brief     Callback function to process subscribed MQTT publication
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @param     p_topic    received topic
 * @param     p_payload  attached payload message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void DhtSensor::CallbackMqtt(PubSubClient *client, char* p_topic, char* p_payload)
{
    p_trace->println(trace_INFO_MSG, "DHT Sensor mqtt callback");
    p_trace->println(trace_INFO_MSG, p_topic);
    p_trace->println(trace_INFO_MSG, p_payload);
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool DhtSensor::ProcessPublishRequests(PubSubClient *client)
{
    p_trace->println(trace_INFO_MSG, "DHT Sensor processes publish request");
    return 0;
}

void DhtSensor::TestIntegerFunc(int testVal)
{
    DhtSensor::testInteger2 = testVal;
    cout << "This is a static function: " << DhtSensor::testInteger2 << endl;
}

void DhtSensor::SetSelf(DhtSensor *myself_p)
{
    DhtSensor::myself_p = myself_p;
    //this->myself_p = myself_p;
    //DhtSensor::testInteger2 = 110;
}

void DhtSensor::StaticTestFunc()
{
    DhtSensor::myself_p->p_trace->println(trace_INFO_MSG, "This is a message from static function");
    //myself_p->p_trace->println(trace_Info_MSG, "test");
    //myself_p->p_trace->println(trace_Info_MSG, "This is a message from static function");
}

int DhtSensor::testInteger2 = 0;
DhtSensor *DhtSensor::myself_p = NULL;

/****************************************************************************************/
/* Private functions: */

