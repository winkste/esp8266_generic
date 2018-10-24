/*****************************************************************************************
* FILENAME :        DimLight.h
*
* DESCRIPTION :
*       Class header for a dimmable light controlled via one pin 
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
#ifndef DIMLIGHT_H_
#define DIMLIGHT_H_

/****************************************************************************************/
/* Imported header files: */

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"
#include "GpioDevice.h"

#include <ESP8266WiFi.h>         
#include <PubSubClient.h>

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class DimLight : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        DimLight(Trace *p_trace, GpioDevice  *gpio_p, char* relayChan_p);
        // virtual functions, implementation in derived classes
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        virtual
        ~DimLight();
    private:
        /********************************************************************************/
        /* Private data definitions */ 
        boolean     lightState_bol      = false;
        boolean     publishState_bol    = true;
        char        topicBuff_ca[100];
        char        *channel_p;
        GpioDevice  *gpio_p;
        uint8_t     brightness_u8       = 20;  
        char        mqttPayload[20];    
        char const  *DEVICE_NAME        = "<<dimLight>>";
        
        /********************************************************************************/
        /* Private function definitions: */
        void TurnLightOff(void);
        void TurnLightOn(void);
        void SetLight(void);
        void ToggleLight(void);
        char* BuildReceiveTopic(const char *topic);
        boolean PublishMessage(PubSubClient *client_p, const char *message_cp, const char * payload_ccp);
        void Subscribe(PubSubClient *client_p, const char *topic_ccp);
    protected:
        /********************************************************************************/
        /* Protected data definitions */

        /********************************************************************************/
        /* Protected function definitions: */

};

#endif /* SINGLERELAY_H_ */

