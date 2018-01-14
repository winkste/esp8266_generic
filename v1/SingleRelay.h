/*****************************************************************************************
* FILENAME :        SingleRelay.h
*
* DESCRIPTION :
*       Class header for Single Relay
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
#ifndef SINGLERELAY_H_
#define SINGLERELAY_H_

/****************************************************************************************/
/* Imported header files: */

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"

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
class SingleRelay : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        SingleRelay(Trace *p_trace);
        SingleRelay(Trace *p_trace, uint8_t pin_u8, char* relayChan_p, boolean invert_bol);
        // virtual functions, implementation in derived classes
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        void ToggleRelay(void);
        virtual
        ~SingleRelay();
    private:
        /********************************************************************************/
        /* Private data definitions */ 
        boolean relayState_bol        = false;
        boolean publishState_bol      = true;
        char buffer_ca[100];
        uint8_t pin_u8                = 0;
        char *channel_p;
        boolean invert_bol            = false;
        
        /********************************************************************************/
        /* Private function definitions: */
        void TurnRelayOff(void);
        void TurnRelayOn(void);
        void SetRelay(void);
        char* BuildReceiveTopic(const char *topic);
        char* BuildSendTopic(const char *topic);
    protected:
        /********************************************************************************/
        /* Protected data definitions */

        /********************************************************************************/
        /* Protected function definitions: */

};

#endif /* SINGLERELAY_H_ */
