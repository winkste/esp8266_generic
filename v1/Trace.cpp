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
#define MQTT_TRACE_TOPIC        "trace/"     //trace message
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
    this->channel_u8 = 0;

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
    this->channel_u8 = 0;

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
    this->buffer_p = new LinkedList<String>();
    this->println(trace_INFO_MSG, "<<trace>>MQTT channel configured");
    this->print(trace_INFO_MSG, "<<trace>>MQTT topic:");
    this->println(trace_PURE_MSG, buildTopic(MQTT_TRACE_TOPIC));
  }
  else
  {
    this->client_p = NULL;
    this->dev_p = NULL; 
  }
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
    this->channel_u8 = 1;  
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     Initializes the Mqtt structures for mqtt tracing
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client_p     mqtt client
 * @param     dev_p        device id of mqtt trace
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::SwitchToSerial()
{
  this->channel_u8 = 0;
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
/*template <typename Generic>
void Trace::print(uint8_t type_u8, Generic *msg)
{
  //printType(type_u8);
  Serial.print(msg);
  
}*/

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for String parameter with new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
/*template <typename Generic>
void Trace::println(uint8_t type_u8, Generic *msg)
{
    //printType(type_u8);
  Serial.println(msg);
}*/

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for String parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
/*template <typename Generic>
void Trace::print(uint8_t type_u8, Generic &msg)
{
  //printType(type_u8);
  Serial.print(msg);
  
}*/

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for String parameter with new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
/*template <typename Generic>
void Trace::println(uint8_t type_u8, Generic &msg)
{
    //printType(type_u8);
  Serial.println(msg);
}*/

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for String parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
/*template <typename Generic>
void Trace::print(uint8_t type_u8, Generic msg)
{
  //printType(type_u8);
  Serial.print(msg);
  
}*/

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for String parameter with new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
/*template <typename Generic>
void Trace::println(uint8_t type_u8, Generic msg)
{
    //printType(type_u8);
  Serial.println(msg);
}*/
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
 * @brief     For asynchronous data channels, this function can be used to send 
 *            queued messages to the channel at a dedicated time.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Trace::PushToChannel()
{
  boolean ret = false;

  if(0 == this->channel_u8)
  {
  }
  else if(1 == this->channel_u8)
  {
    if((buffer_p != NULL) && (this->client_p != NULL) && (this->dev_p != NULL))
    {
      while(0 != buffer_p->size())
      {
        //Serial.print("--MQTT--");
        //Serial.print(buffer_p->shift());
        (void)client_p->publish(buildTopic(MQTT_TRACE_TOPIC), 
                                      buildPayload(buffer_p->shift()), true);
      }
    }
  }
  else
  {
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
  //String temp_str = String(p_pc);
  switch(type_u8)
  {
    case trace_PURE_MSG:
      buffer_str = buffer_str + String(msg_str);
      break;
    case trace_INFO_MSG:
      buffer_str = buffer_str + String("[INFO]" + msg_str);
      break;
    case trace_WARN_MSG:
      buffer_str = buffer_str + String("[WARN]" + msg_str);
      break;
    case trace_ERROR_MSG:
      buffer_str = buffer_str + String("[ERROR]" + msg_str);
      break;
    default:
      buffer_str = buffer_str + String(msg_str);
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
void Trace::printMsg(void)
{
  switch(Trace::channel_u8)
  {
    case 0:
      Serial.print(buffer_str);
      buffer_str = "";
      break;
    case 1:
      // wait until message is complete
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
  switch(Trace::channel_u8)
  {
    case 0:
      Serial.println(buffer_str);
      break;
    case 1:
      buffer_str = buffer_str + "\n";
      buffer_p->add(buffer_str);
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
char* Trace::buildTopic(const char *topic) 
{
  sprintf(buffer_ca, "%s%s", topic, this->dev_p);
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

