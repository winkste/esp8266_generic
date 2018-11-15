/*****************************************************************************************
* FILENAME :        Trace.cpp         
*
* DESCRIPTION :
*       File to support simple wemos relay shield
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

/*****************************************************************************************
 * Include Interfaces
*****************************************************************************************/
#include <ESP8266WiFi.h>
#include "Trace.h"
#include <PubSubClient.h>
#include <LinkedList.h>

/*****************************************************************************************
 * Local constant defines
*****************************************************************************************/
#define MQTT_TRACE_TOPIC        "s/trace/"     //trace message
/*****************************************************************************************
 * Local function like makros 
*****************************************************************************************/

/*****************************************************************************************
 * Local type definitions (enum, struct, union):
*****************************************************************************************/

/*****************************************************************************************
* Public functions (unlimited visibility): 
*****************************************************************************************/

/**---------------------------------------------------------------------------------------
 * @brief     Default constructor   
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Trace::Trace()
{
    this->isActive_bol = true;
    this->channel_u8 = trace_CHANNEL_SERIAL;

}

/**---------------------------------------------------------------------------------------
 * @brief     Default constructor for channel 0
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     isActive_bol     trace active or not
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Trace::Trace(bool isActive_bol)
{
    this->isActive_bol = isActive_bol;
    this->channel_u8 = trace_CHANNEL_SERIAL;

}

/**---------------------------------------------------------------------------------------
 * @brief     Initializes the Mqtt structures for mqtt tracing
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client_p     mqtt client
 * @param     dev_p        device id of mqtt trace
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::InitializeMqtt(PubSubClient *client_p, const char *dev_p)
{
  if((NULL != client_p) && (NULL != dev_p))
  {
    this->client_p = client_p;
    this->dev_p = dev_p;
    this->msgList_p = new LinkedList<Message*>;
    this->println(trace_INFO_MSG, "<<trace>>MQTT channel configured");
    this->print(trace_INFO_MSG, "<<trace>>MQTT topics:");
    this->print(trace_PURE_MSG, buildTopic(MQTT_TRACE_TOPIC, trace_ERROR_MSG));
    this->print(trace_PURE_MSG," or ");
    this->print(trace_PURE_MSG, buildTopic(MQTT_TRACE_TOPIC, trace_INFO_MSG));
  }
  else
  {
    this->client_p = NULL;
    this->dev_p = NULL; 
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     Switch trace off
 * @author    winkste
 * @date      18. Jan. 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::SwitchToOff()
{
  this->println(trace_INFO_MSG, "<<trace>> trace switch received off");
  this->channel_u8 = trace_CHANNEL_OFF;
}

/**---------------------------------------------------------------------------------------
 * @brief     Switch trace to mqtt if the mqtt structures are valid
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::SwitchToMqtt()
{
  if((NULL != client_p) && (NULL != dev_p))
  {
    this->println(trace_INFO_MSG, "<<trace>> trace switch received to MQTT:");
    this->channel_u8 = trace_CHANNEL_MQTT;  
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     Switch to serial channel for trace messages
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::SwitchToSerial()
{
  this->println(trace_INFO_MSG, "<<trace>> trace switch received to serial:");
  this->channel_u8 = trace_CHANNEL_SERIAL;
}

/**---------------------------------------------------------------------------------------
 * @brief     Switch trace channel
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     chan_u8 channel for trace messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::SelectTraceChannel(uint8_t chan_u8)
{
  switch(chan_u8)
  {
    case trace_CHANNEL_OFF:
      this->SwitchToOff();
      break;
    case trace_CHANNEL_SERIAL:
      this->SwitchToSerial();
      break;
    case trace_CHANNEL_MQTT:
      this->SwitchToMqtt();
      break;
    default:
      this->SwitchToSerial();
      break;
  } 
}

/**---------------------------------------------------------------------------------------
 * @brief     Full destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Trace::~Trace()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the trace interface selected with the constructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::Initialize()
{
    // init the serial
    Serial.begin(115200);
    Serial.println("");
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for String parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::print(uint8_t type_u8, String msg_str)
{
  this->prepareMsg(type_u8, String(msg_str));
  this->printMsg();
  
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for String parameter with new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::println(uint8_t type_u8, String msg_str)
{
  this->prepareMsg(type_u8, String(msg_str));
  this->printlnMsg();
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for char pointer parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_pc          message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::print(uint8_t type_u8, char *msg_pc)
{
  this->prepareMsg(type_u8, String(msg_pc));
  this->printMsg();
  
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for char pointer parameter with new line 
 *              at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_pc          message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::println(uint8_t type_u8, char *msg_pc)
{
  this->prepareMsg(type_u8, String(msg_pc));
  this->printlnMsg();
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for uint8_t parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     value_u8        data value
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::print(uint8_t type_u8, uint8_t value_u8)
{
  this->prepareMsg(type_u8, String(value_u8));
  this->printMsg();
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for uint8_t parameter with new line 
 *              at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     value_u8        data value
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::println(uint8_t type_u8, uint8_t value_u8)
{
  this->prepareMsg(type_u8, String(value_u8 ));
  this->printlnMsg();
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for uint8_t parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     value_u16       data value
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::print(uint8_t type_u8, uint16_t value_u16)
{
  this->prepareMsg(type_u8, String(value_u16));
  this->printMsg();
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for uint8_t parameter with new line 
 *              at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     value_u16       data value
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::println(uint8_t type_u8, uint16_t value_u16)
{
  this->prepareMsg(type_u8, String(value_u16));
  this->printlnMsg();
}

/**---------------------------------------------------------------------------------------
 * @brief     For asynchronous data channels, this function can be used to send 
 *            queued messages to the channel at a dedicated time.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::PushToChannel()
{
  boolean ret = false;
  Message *msg_p;

  switch(this->channel_u8)
  {
    case trace_CHANNEL_OFF:
      break;
    case trace_CHANNEL_SERIAL:
      break;
    case trace_CHANNEL_MQTT:
      if((msgList_p != NULL) && (this->client_p != NULL) && (this->dev_p != NULL))
      {
        while(0 != msgList_p->size())
        {
          msg_p = msgList_p->shift();
          (void)client_p->publish(buildTopic(MQTT_TRACE_TOPIC, msg_p->type_u8), 
                                      buildPayload(msg_p->msg), true);
          delete msg_p;
        }    
      }
      break;
    default:
      // unsupported channel selected, set to default channel = serial
      this->channel_u8 = 0; 
  }
}

/****************************************************************************************/
/* Private functions: */
/****************************************************************************************/
/**---------------------------------------------------------------------------------------
 * @brief     Trace preparation function to combine message type and message 
 *              at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::prepareMsg(uint8_t type_u8, String msg_str)
{
  // check if a message was already started
  if(NULL == message_p)
  {
    // new start of a trace message
    buffer_str = "";
    message_p = new Message();    
    if(message_p == NULL)
    { 
      // error in memory handling, switch to serial trace
      this->channel_u8 = trace_CHANNEL_SERIAL;
    }
    else
    {
      message_p->type_u8 = type_u8;
    }   
  } 
  else
  {
    // continue existing message   
  }

  buffer_str = buffer_str + String(msg_str);
}

/**---------------------------------------------------------------------------------------
 * @brief     Depending on the channel, this function adds the message to the buffer
 *            list or direct plots it to the serial interface
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::printMsg(void)
{
   switch(this->channel_u8)
  {
    case trace_CHANNEL_OFF:
      break;
    case trace_CHANNEL_SERIAL:
      Serial.print(buffer_str);
      buffer_str = "";
      break;
    case trace_CHANNEL_MQTT:
      break;
    default:
      break;  
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     Depending on the channel, this function adds the message to the buffer
 *            list or direct plots it to the serial interface
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::printlnMsg(void)
{
  switch(this->channel_u8)
  {
    case trace_CHANNEL_OFF:
      break;
    case trace_CHANNEL_SERIAL:
      Serial.println(buffer_str);
      break;
    case trace_CHANNEL_MQTT:
      message_p->msg = String(buffer_str + "\n");
      msgList_p->add(message_p);
      // no free message_p, because the object is now in the linked list
      message_p = NULL;
      break;
    default:
      break;
  }

  buffer_str = "";
}

/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     topic       pointer to topic string
 * @return    combined topic as char pointer, it uses buffer_stca to store the topic
*//*-----------------------------------------------------------------------------------*/
char* Trace::buildTopic(const char *topic, uint8_t type_u8) 
{
  if(type_u8 == trace_ERROR_MSG)
  {
    sprintf(buffer_ca, "err/%s/%s", this->dev_p, topic);
  }
  else
  {
    sprintf(buffer_ca, "inf/%s/%s", this->dev_p, topic);
  }
  
  return buffer_ca;
}

/**---------------------------------------------------------------------------------------
 * @brief     This function converts the payload string to a char pointer, the char 
 *            buffer is part of the object
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     payload       pointer to payload string
 * @return    pointer to the payload string
*//*-----------------------------------------------------------------------------------*/
char* Trace::buildPayload(String payload) 
{
  payload.toCharArray(payload_ca, payload.length());
  return payload_ca;
}


