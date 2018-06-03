/*****************************************************************************************
* FILENAME :        Trace.h          
*
* DESCRIPTION :
*       Class header for trace functionality
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

#ifndef TRACE_H_
#define TRACE_H_

/****************************************************************************************/
/* Imported header files: */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LinkedList.h>

/****************************************************************************************/
/* Global constant defines: */
#define trace_PURE_MSG  0u
#define trace_INFO_MSG  1u
#define trace_WARN_MSG  2u
#define trace_ERROR_MSG 3u

#define trace_CHANNEL_OFF     0
#define trace_CHANNEL_SERIAL  1
#define trace_CHANNEL_MQTT    2
/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class Trace
{
    public:
        /********************************************************************************/
        /* Public data definitions */
        const uint8_t PURE_MSG  = 0;
        const uint8_t INFO_MSG  = 1;
        const uint8_t WARN_MSG  = 2;
        const uint8_t ERROR_MSG = 3;

        /********************************************************************************/
        /* Public function definitions: */
        Trace();
        Trace(bool isActive_bol);
        void InitializeMqtt(PubSubClient *client_p, const char *dev_p);
        void SwitchToOff();
        void SwitchToMqtt();
        void SwitchToSerial();
        void SelectTraceChannel(uint8_t chan_u8);
        void PushToChannel();
        void Initialize();
        //boolean ConfigMqttTrace(PubSubClient *client_p, const char *dev_p);
        //boolean SwitchChannel(uint8_t channel_u8);
        virtual ~Trace();
        /*template <typename Generic>
        void print(uint8_t type_u8, Generic *msg);
        template <typename Generic>
        void println(uint8_t type_u8, Generic *msg);
        template <typename Generic>
        void print(uint8_t type_u8, Generic &msg);
        template <typename Generic>
        void println(uint8_t type_u8, Generic &msg);
        template <typename Generic>
        void print(uint8_t type_u8, Generic msg);
        template <typename Generic>
        void println(uint8_t type_u8, Generic msg);*/
        
        void print(uint8_t type_u8, char * msg_pc);
        void println(uint8_t type_u8, char * msg_pc);
        void print(uint8_t type_u8, String msg_str);
        void println(uint8_t type_u8, String msg_str);
        void print(uint8_t type_u8, uint8_t value_u8);
        void println(uint8_t type_u8, uint8_t value_u8);
        void print(uint8_t type_u8, uint16_t value_u16);
        void println(uint8_t type_u8, uint16_t value_u16);

    private:
        /********************************************************************************/
        /* Private data definitions */
        class Message
        {
            public:
                String msg;
                uint8_t type_u8;
        };
        
        bool isActive_bol;
        uint8_t channel_u8;
        String buffer_str;
        PubSubClient *client_p; 
        const char *dev_p;
        LinkedList<Message*> *msgList_p;
        Message *message_p;
        uint8_t type_u8;
        char buffer_ca[70];
        char payload_ca[150];
               
        /********************************************************************************/
        /* Private function definitions: */
        void prepareMsg(uint8_t type_u8, String msg_str);
        void printMsg(void);
        void printlnMsg(void);
        char* buildTopic(const char *topic, uint8_t type_u8) ;
        char* buildPayload(String payload); 
};

#endif /* TRACE_H_ */
