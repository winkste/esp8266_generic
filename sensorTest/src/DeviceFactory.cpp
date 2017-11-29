/*****************************************************************************************
* FILENAME :        DeviceFactory.cpp
*
* DESCRIPTION :
*       Device generator
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
#include "SonoffSwitch.h"
#include "DhtSensor.h"
#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"

#include "DeviceFactory.h"

/****************************************************************************************/
/* Local constant defines */
#define CAPABILITY_0x80         0x80u
#define CAPABILITY_0x40         0x40u
#define CAPABILITY_0x20         0x20u
#define CAPABILITY_0x10         0x10u
#define CAPABILITY_0x08         0x08u
#define CAPABILITY_0x04         0x04u
#define CAPABILITY_0x02         0x02u
#define CAPABILITY_0x01         0x01u

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for SonoffSwitch
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DeviceFactory::DeviceFactory(Trace * p_trace)
{
    this->trace_p = p_trace;
}

MqttDevice * DeviceFactory::GenerateDevice(uint8_t type_u8)
{
    MqttDevice * device_p = NULL;
    DhtSensor *sensor_p = NULL;

    switch(type_u8)
    {
        case 0:
            device_p = new SonoffSwitch(trace_p);
            break;
        case 1:
            sensor_p = new DhtSensor(trace_p);
            device_p = new DhtSensor(trace_p);
            DhtSensor::TestIntegerFunc(100);
            DhtSensor::SetSelf(sensor_p);
            DhtSensor::StaticTestFunc();
            break;
        default:
            break;
    }

    return(device_p);
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

