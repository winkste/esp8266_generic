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
#include "MqttDevice.h"
#include "Trace.h"

/****************************************************************************************/
/* Local constant defines */

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

MqttDevice::MqttDevice(Trace *p_trace)
{
    this->prevTime_u32 = 0;
    this->publications_u16 = 0;
    this->p_trace = p_trace;
    this->isInitialized_bol = false;
    this->isConnected_bol = false;
}

MqttDevice::~MqttDevice()
{
}

unsigned long MqttDevice::GetPrevTime_u32()
{
    return(prevTime_u32);
}

unsigned int MqttDevice::GetPublications_u16()
{
    return(publications_u16);

}

/****************************************************************************************/
/* Private functions: */
