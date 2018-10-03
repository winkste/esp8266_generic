/*****************************************************************************************
* FILENAME :        NeoPix.h
*
* DESCRIPTION :
*       Class header for a Adafruit based NeoPixel controlled via one pin 
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
#ifndef NEOPIX_H_
#define NEOPIX_H_

/****************************************************************************************/
/* Imported header files: */

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"
#include "GpioDevice.h"

#include <ESP8266WiFi.h>         
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class NeoPix : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        NeoPix(Trace *trace_pcl, GpioDevice  *gpio_pcl, char* neoChan_pch);
        // virtual functions, implementation in derived classes
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        virtual
        ~NeoPix();
    private:
        /********************************************************************************/
        /* Private data definitions */ 
        boolean             lightState_bol      = false;
        boolean             neoStateChanged_bol = true;
        uint8_t             brightness_u8       = 20; 
        char                topicBuff_cha[100];
        char                *channel_pch;
        GpioDevice          *gpio_pcl;  
        char                mqttPayload_cha[20];    
        char const          *DEVICE_NAME        = "<<neoPix>>";
        const uint8_t       defaultRed_u8c      = 0U;
        const uint8_t       defaultGreen_u8c    = 0U;
        const uint8_t       defaultBlue_u8c     = 0U;
        const uint16_t      numPixels_u16c      = 1U;
        uint8_t             red_u8              = defaultRed_u8c;
        uint8_t             green_u8            = defaultGreen_u8c;
        uint8_t             blue_u8             = defaultBlue_u8c;
        Adafruit_NeoPixel   pixels_cl;
        
        /********************************************************************************/
        /* Private function definitions: */
        void TurnOff_vd(void);
        void TurnOn_vd(void);
        void Set_vd(void);
        void Toggle_vd(void);
        char* BuildReceiveTopic_pch(const char *topic);
        boolean PublishMessage_bol(PubSubClient *client_p, const char *message_cp, const char * payload_ccp);
        void Subscribe_vd(PubSubClient *client_p, const char *topic_ccp);
    protected:
        /********************************************************************************/
        /* Protected data definitions */

        /********************************************************************************/
        /* Protected function definitions: */

};

#endif /* SINGLERELAY_H_ */

