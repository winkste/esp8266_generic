/*****************************************************************************************
* FILENAME :        Sensor.h
*
* DESCRIPTION :
*       Abstract class for Sensors
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
*****************************************************************************************/
#ifndef MQTTDEVICE_H_
#define MQTTDEVICE_H_

/****************************************************************************************/
/* Imported header files: */
#include <string>
#include "Trace.h"
#include "PubSubClient.h"

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        MqttDevice(Trace *p_trace);
        unsigned long GetPrevTime_u32();
        unsigned int GetPublications_u16();

        static bool GetReconfigRequest();

        virtual ~MqttDevice();
        virtual bool ProcessPublishRequests(PubSubClient *client) = 0;
        virtual void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload) = 0;
        virtual void Initialize() = 0;
        virtual void Reconnect(PubSubClient *client_p, const char *dev_p) = 0;
        
    protected:
        /********************************************************************************/
        /* Protected data definitions */
        unsigned long       prevTime_u32;
        unsigned int        publications_u16;
        bool                isInitialized_bol;
        bool                isConnected_bol;
        Trace               *p_trace;
        const char          *dev_p;
        
        static bool         startWifiConfig_bol;

        /********************************************************************************/
        /* Protected function definitions: */
        
};

#endif /* MQTTDEVICE_H_ */
