/*****************************************************************************************
* FILENAME :        DeviceFactory.cpp
*
* DESCRIPTION :
*       Device generator class
*       This class has got the information of the different devices and generates
*       them on request. The user has to specify which type of device he needs.
*
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

#include "DeviceFactory.h"

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"
#include <LinkedList.h>

#include "DhtSensor.h"
#include "SingleRelay.h"
#include "SonoffBasic.h"
#include "Pir.h"

/****************************************************************************************/
/* Local constant defines */
#define CAPABILITY_MQTT_TRACE           0x80u
#define CAPABILITY_0x40                 0x40u
#define CAPABILITY_0x20                 0x20u
#define CAPABILITY_0x10                 0x10u
#define CAPABILITY_FOUR_RELAY           0x08u
#define CAPABILITY_PIR_RELAY            0x05u
#define CAPABILITY_DHT_SENSOR_BAT       0x04u
#define CAPABILITY_PIR                  0x03u
#define CAPABILITY_SONOFF_BASIC         0x02u
#define CAPABILITY_DHT_SENSOR           0x01u
#define CAPABILITY_SINGLE_RELAY         0x00u

#define WEMOS_PIN_D0                    16u // D0
#define WEMOS_PIN_D1                    5u  // D1
#define WEMOS_PIN_D2                    4u  // D2
#define WEMOS_PIN_D3                    0u  // D3
#define WEMOS_PIN_D4                    2u  // D4
#define WEMOS_PIN_D5                    14u // D5
#define WEMOS_PIN_D6                    12u // D6
#define WEMOS_PIN_D7                    13u // D7
#define WEMOS_PIN_D8                    15u // D8

#define RELAY_PIN_ONE                   WEMOS_PIN_D1
#define RELAY_PIN_TWO                   WEMOS_PIN_D2
#define RELAY_PIN_THREE                 WEMOS_PIN_D3
#define RELAY_PIN_FOUR                  WEMOS_PIN_D4

#define PIR_INPUT_PIN                   WEMOS_PIN_D3
#define PIR_LED_PIN                     WEMOS_PIN_D4


#define MQTT_CHAN_ONE                   "relay_one"
#define MQTT_CHAN_TWO                   "relay_two"
#define MQTT_CHAN_THREE                 "relay_three"
#define MQTT_CHAN_FOUR                  "relay_four"


/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructorfor the device fectory
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DeviceFactory::DeviceFactory(Trace * p_trace)
{
    this->trace_p = p_trace;
}

/**---------------------------------------------------------------------------------------
 * @brief     Method to generate devices based on the specified type
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8     type of device
 * @return    a MqttDevice if successful, else NULL
*//*-----------------------------------------------------------------------------------*/
LinkedList<MqttDevice*> * DeviceFactory::GenerateDevice(uint8_t type_u8)
{
    LinkedList<MqttDevice*> *deviceList_p = new LinkedList<MqttDevice*>();
    
    MqttDevice * device_p = NULL;
    SonoffBasic *sonoffDevice_p = NULL;
    Pir *pirDevice_p = NULL;

    if(CAPABILITY_MQTT_TRACE == (type_u8 & CAPABILITY_MQTT_TRACE))
    {
        // mqtt trace requested
        type_u8 = type_u8 - CAPABILITY_MQTT_TRACE;
        // switch to mqtt trace
        this->trace_p->SwitchToMqtt();
    }
    else
    {
      // switch to serial
      this->trace_p->SwitchToSerial();
    }

    switch(type_u8)
    {
        case CAPABILITY_SINGLE_RELAY:
            device_p = new SingleRelay(trace_p);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_DHT_SENSOR:
            device_p = new DhtSensor(trace_p);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated dht device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_SONOFF_BASIC:
            sonoffDevice_p = new SonoffBasic(trace_p);
            sonoffDevice_p->SetSelf(sonoffDevice_p);
            device_p = sonoffDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated sonoff basic device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_PIR:
            //pirDevice_p = new Pir(trace_p);
            pirDevice_p = new Pir(trace_p, PIR_INPUT_PIN, PIR_LED_PIN);
            pirDevice_p->SetSelf(pirDevice_p);
            device_p = pirDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated pir device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_DHT_SENSOR_BAT:
            device_p = new DhtSensor(trace_p, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated dht device with bat support");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_FOUR_RELAY:
            device_p = new SingleRelay(trace_p, RELAY_PIN_ONE, MQTT_CHAN_ONE, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device one");
            deviceList_p->add(device_p);
            device_p = new SingleRelay(trace_p, RELAY_PIN_TWO, MQTT_CHAN_TWO, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device two");
            deviceList_p->add(device_p);
            device_p = new SingleRelay(trace_p, RELAY_PIN_THREE, MQTT_CHAN_THREE, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device three");
            deviceList_p->add(device_p);
            device_p = new SingleRelay(trace_p, RELAY_PIN_FOUR, MQTT_CHAN_FOUR, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device four");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_PIR_RELAY:
            device_p = new SingleRelay(trace_p);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device");
            deviceList_p->add(device_p);
            pirDevice_p = new Pir(trace_p, PIR_INPUT_PIN, PIR_LED_PIN);
            pirDevice_p->SetSelf(pirDevice_p);
            device_p = pirDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated pir device");
            deviceList_p->add(device_p);
            break;
        default:
            break;
    }

    return(deviceList_p);
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DeviceFactory::~DeviceFactory()
{
    // TODO Auto-generated destructor stub
}

/****************************************************************************************/
/* Private functions: */

