/*****************************************************************************************
* FILENAME :        PowerSave.h
*
* DESCRIPTION :
*       Class header for power saving handler
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
#ifndef POWERSAVE_H_
#define POWERSAVE_H_

/****************************************************************************************/
/* Imported header files: */
#include <ESP8266WiFi.h>         
#include <PubSubClient.h>

#include "MqttDevice.h"
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
class PowerSave : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        PowerSave(Trace *p_trace);
        PowerSave(Trace *p_trace, bool powerSaveMode_bol);
        PowerSave(Trace *p_trace, bool powerSaveMode_bol, uint16_t pwrOnTimeSec_u16, 
                        uint16_t pwrSaveTimeSec_u16);
        // virtual functions, implementation in derived classes
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        virtual
        ~PowerSave();
    private:
        /********************************************************************************/
        /* Private data definitions */
        char buffer_ca[100];
        bool pwrSaveMode_bol;
        uint32_t             prevTime_u32 = 0;
        uint32_t pwrOnTimeMSec_u32;
        uint32_t pwrSaveTimeMSec_u32;
        uint8_t actualState_u8;
        uint32_t timer_u32;
        /********************************************************************************/
        /* Private function definitions: */
        char* build_topic(const char *topic);
        
    protected:
        /********************************************************************************/
        /* Protected data definitions */
        
        /********************************************************************************/
        /* Protected function definitions: */
};

#endif /* POWERSAVE_H_ */
