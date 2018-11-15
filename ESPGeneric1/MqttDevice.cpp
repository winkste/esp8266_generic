/*****************************************************************************************
* FILENAME :        Sensor.cpp
*
* DESCRIPTION :
*       Abstract Sensor Class
*
* NOTES :
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
/* Static Data instantiation */
bool MqttDevice::startWifiConfig_bol = false;

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructur for MqttDevice
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace   trace object for debug output
 * @return    a MqttDevice object
*//*-----------------------------------------------------------------------------------*/
MqttDevice::MqttDevice(Trace *p_trace)
{
    this->prevTime_u32 = 0;
    this->publications_u16 = 0;
    this->p_trace = p_trace;
    this->isInitialized_bol = false;
    this->isConnected_bol = false;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
MqttDevice::~MqttDevice()
{
}

/**---------------------------------------------------------------------------------------
 * @brief     Getter method for previous time publication member variable
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    time since last publication
*//*-----------------------------------------------------------------------------------*/
unsigned long MqttDevice::GetPrevTime_u32()
{
    return(prevTime_u32);
}

/**---------------------------------------------------------------------------------------
 * @brief     Getter method for the count of publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    count of publications
*//*-----------------------------------------------------------------------------------*/
unsigned int MqttDevice::GetPublications_u16()
{
    return(publications_u16);

}

/**---------------------------------------------------------------------------------------
 * @brief     Returns re-config request status and sets it to false
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    re-config request
*//*-----------------------------------------------------------------------------------*/
bool MqttDevice::GetReconfigRequest() 
{
    bool ret_bol = MqttDevice::startWifiConfig_bol;

    MqttDevice::startWifiConfig_bol = false;
    return(ret_bol);
}

/****************************************************************************************/
/* Private functions: */
