/*****************************************************************************************
* FILENAME :        RfDht.cpp
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
*               range: 10, 01, 11 1Bit preamble. 
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
#include "RfDht.h"

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"

/****************************************************************************************/
/* Local constant defines */
#define DEFAULT_RFDHT_INPUT_PIN   0  // D3
#define MQTT_PUB_RFDHT_TEMP       "/s/temp_hum/temp"     //temperature
#define MQTT_PUB_RFDHT_HUM        "/s/temp_hum/hum" // humidity data
#define MQTT_PUB_RFDHT_BAT        "/s/temp_hum/bat" // battery capacity data

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Static Data instantiation */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the RfDht sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
RfDht::RfDht(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32      = 0;
    this->publications_u16  = 0;
    this->publishState_bol  = true;
    this->rfPin_u8         = DEFAULT_RFDHT_INPUT_PIN;     
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the RfDht sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @param     rfPin_u8   rf pin selection
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
RfDht::RfDht(Trace *p_trace, uint8_t rfPin_u8) : MqttDevice(p_trace)
{
    this->prevTime_u32      = 0;
    this->publications_u16  = 0;
    this->publishState_bol  = true;
    this->rfPin_u8         = rfPin_u8;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
RfDht::~RfDht()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the single relay object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void RfDht::Initialize()
{
    p_trace->println(trace_INFO_MSG, "<<rfdht>>RfDht initialized");

    // TODO

    this->isInitialized_bol = true;
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to initialize the MQTT interface for this module
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @param     dev_p      client device id for building the mqtt topics
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void RfDht::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->println(trace_INFO_MSG, "<<rfdht>> reconnected");
        // ... and resubscribe
    }
    else
    {
        // failure, not connected
        p_trace->println(trace_ERROR_MSG, 
                                "<<rfdht>>uninizialized MQTT client detected");
        this->isConnected_bol = false;
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Callback function to process subscribed MQTT publication
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @param     p_topic    received topic
 * @param     p_payload  attached payload message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void RfDht::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true == this->isConnected_bol)
    {
        
    }
    else
    {
        p_trace->println(trace_ERROR_MSG, "<<rfdht>>connection failure in rfdht CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool RfDht::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret = false;
    
    if(true == this->isConnected_bol)
    {
        // check if state has changed, than publish this state
        if(true == publishState_bol)
        {
            // TDOD
            publishState_bol = false;
        }
    }
    else
    {
        p_trace->println(trace_ERROR_MSG, 
                              "<<rfdht>>connection failure in relay ProcessPublishRequests "); 
    }
    return ret; 
};

/****************************************************************************************/
/* Private functions: */

/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     topic       pointer to topic string
 * @return    combined topic as char pointer, it uses buffer_stca to store the topic
*//*-----------------------------------------------------------------------------------*/
char* RfDht::build_topic(const char *topic) 
{
  sprintf(buffer_ca, "std/%s%s", this->dev_p, topic);
  return buffer_ca;
}

