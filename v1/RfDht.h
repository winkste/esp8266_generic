/*****************************************************************************************
* FILENAME :        RfDht.h
*
* DESCRIPTION :
*       Implementation of the RfDht sensor device
*           The messages expected from transmitter are expected in the following format:
*           expecting the max send payload per message to be 32Bit.
*           32 bit
*           -------------------------------------------------
*           | 31 - 25   | 24 - 16   | 15 - 8    | 7 - 0     |
*           -------------------------------------------------
*           |   Header  |    16 bit DataWord    | CheckSum  |
*           -------------------------------------------------
*
*           The Header includes node and payload identification:
*           -------------------------------------------------
*           | 7     | 6 - 5   | 4 - 3   | 2 - 1   | 0       |
*           -------------------------------------------------
*           | PreAmb| ToNode  | FrNode  | MsgId   | PostAmb |
*           -------------------------------------------------           
*           PreAmb: The message header starts with a 1 bit preamble set to 1.

*           ToNode: This is the receiver identifier to generate a pointed message. 
*               The system currently supports 4 different receivers within one network 
*               range: 00, 10, 01, 11
*
*           FrNode: it is possible to connect up to 4 nodes to one base, the node id's 
*               have to be unique, therefore the identifier can be defined out of two
*               bits: 00, 01, 10, 11
*           MsgId: Is used to identify the payload of this node. 4 different
*               payloads are currently supported:
*                   00 : Temperature
*                   01 : Humidity
*                   10 : Battery Voltage
*                   11 : Package send counter
*           PostAmb: is the header end identifier and is always 1
*
*           CheckSum: A bytewise and of the three bytes message. 
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
#ifndef RFDHT_H_
#define RFDHT_H_

/****************************************************************************************/
/* Imported header files: */

#include "MqttDevice.h"
#include "Trace.h"

#include <ESP8266WiFi.h>         
#include <PubSubClient.h>
#include <RCSwitch.h>

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

typedef struct rfDht_msg_tag
{
    uint8_t header_u8;
    uint16_t data_u16;
    uint8_t chkSum_u8;
}rfDht_msg_t;

typedef union rfDht_msgConverter_tag
{
    uint32_t rawData_u32;
    rfDht_msg_t msg_s;
}rfDht_msgConverter_t;

/****************************************************************************************/
/* Class definition: */
class RfDht : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */
        static RfDht *mySelf_p;
        static uint32_t pirMotionDetect_u32;
        static uint8_t counterButton_u8;
        /********************************************************************************/
        /* Public function definitions: */
        static bool verifyMessage(rfDht_msg_t *msg_p);
        static uint8_t getToNodeId(rfDht_msg_t *msg_);
        static uint8_t getFromNodeId(rfDht_msg_t *msg_p);
        static uint8_t getMsgTypeId(rfDht_msg_t *msg_p);
        static uint16_t getMsgData(rfDht_msg_t *msg_p);
        
        RfDht(Trace *p_trace);
        RfDht(Trace *p_trace, uint8_t rfPin_u8);
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        virtual
        ~RfDht();
    private:
        /********************************************************************************/
        /* Private data definitions */ 
        boolean publishState_bol      = true;
        uint8_t rfPin_u8             = 0u;
        char buffer_ca[100];
        
        /********************************************************************************/
        /* Private function definitions: */
        char* build_topic(const char *topic);
    protected:
        /********************************************************************************/
        /* Protected data definitions */

        /********************************************************************************/
        /* Protected function definitions: */

};

#endif /* RFDHT_H_ */
