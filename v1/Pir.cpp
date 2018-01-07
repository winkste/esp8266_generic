/*****************************************************************************************
* FILENAME :        Pir.cpp
*
* DESCRIPTION :
*       Implementation of the Pir sensor device
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
#include "Pir.h"

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"

/****************************************************************************************/
/* Local constant defines */
#define DEFAULT_PIR_INPUT_PIN     0  // D3
#define MQTT_PUB_PIR_STATE        "/pir/status"     //state of pir
#define MQTT_PUB_PIR_GOTO_LOW     "/pir/motion"     // goto low state event  
#define MQTT_PUB_PIR_GOTO_HIGH    "/pir/no_motion"  // goto high state event
#define MQTT_PAYLOAD_MOTION       "ON"
#define MQTT_PAYLOAD_NO_MOTION    "OFF"

#define PIR_DEBOUNCE              400  // ms debouncing for the botton

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Static Data instantiation */
Pir *Pir::mySelf_p                  = NULL;
uint32_t Pir::pirMotionDetect_u32   = 0;
uint8_t Pir::counterButton_u8       = 0;

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the pir sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Pir::Pir(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32      = 0;
    this->publications_u16  = 0;
    this->pirState_bol      = false;
    this->publishState_bol  = true;
    this->pirPin_u8         = DEFAULT_PIR_INPUT_PIN;
    this->ledPin_u8         = 0xff; // not used      
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the pir sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @param     pirPin_u8   pir pin selection
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Pir::Pir(Trace *p_trace, uint8_t pirPin_u8) : MqttDevice(p_trace)
{
    this->prevTime_u32      = 0;
    this->publications_u16  = 0;
    this->pirState_bol      = false;
    this->publishState_bol  = true;
    this->pirPin_u8         = pirPin_u8;
    this->ledPin_u8         = 0xff; // not used
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the pir sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @param     pirPin_u8   pir pin selection
 * @param     ledPin_u8   led pin, set to high if motion was detected
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Pir::Pir(Trace *p_trace, uint8_t pirPin_u8, uint8_t ledPin_u8) : MqttDevice(p_trace)
{
    this->prevTime_u32      = 0;
    this->publications_u16  = 0;
    this->pirState_bol      = false;
    this->publishState_bol  = true;
    this->pirPin_u8         = pirPin_u8;
    this->ledPin_u8         = ledPin_u8;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Pir::~Pir()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the single relay object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Pir::Initialize()
{
    p_trace->println(trace_INFO_MSG, "<<pir>>Pir initialized");

    pinMode(this->pirPin_u8, INPUT);
    digitalWrite(this->pirPin_u8, HIGH); // pull up to avoid interrupts without sensor
    attachInterrupt(digitalPinToInterrupt(this->pirPin_u8), 
                                            Pir::UpdatePirState, CHANGE);
    if(0xff != this->ledPin_u8)
    {
      pinMode(this->ledPin_u8, OUTPUT);
      digitalWrite(this->ledPin_u8, HIGH);
    }
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
void Pir::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->println(trace_INFO_MSG, "<<pir>> reconnected");
        // ... and resubscribe
    }
    else
    {
        // failure, not connected
        p_trace->println(trace_ERROR_MSG, 
                                "<<pir>>uninizialized MQTT client in single relay detected");
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
void Pir::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true == this->isConnected_bol)
    {
        
    }
    else
    {
        p_trace->println(trace_ERROR_MSG, "<<pir>>connection failure in pir CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool Pir::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret = false;
    
    if(true == this->isConnected_bol)
    {
        // check if state has changed, than publish this state
        if(true == publishState_bol)
        {
            p_trace->print(trace_INFO_MSG, "<<pir>> publish requested state: ");
            p_trace->print(trace_PURE_MSG, MQTT_PUB_PIR_STATE);
            p_trace->print(trace_PURE_MSG, "  :  ");
            if(true == this->pirState_bol)
            {
              ret = client->publish(build_topic(MQTT_PUB_PIR_STATE), 
                                        MQTT_PAYLOAD_MOTION, true);
              p_trace->println(trace_PURE_MSG, MQTT_PAYLOAD_MOTION);
            }
            else
            {
                ret = client->publish(build_topic(MQTT_PUB_PIR_STATE), 
                                          MQTT_PAYLOAD_NO_MOTION, true); 
                p_trace->println(trace_PURE_MSG, MQTT_PAYLOAD_NO_MOTION); 
            } 
            if(ret)
            {
                publishState_bol = false;     
            }
        }
    }
    else
    {
        p_trace->println(trace_ERROR_MSG, 
                              "<<pir>>connection failure in relay ProcessPublishRequests "); 
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
char* Pir::build_topic(const char *topic) 
{
  sprintf(buffer_ca, "%s%s", this->dev_p, topic);
  return buffer_ca;
}

/**---------------------------------------------------------------------------------------
 * @brief     This function handles the external pir input and updates the value
 *              of the state variable. If the value changed the corresponding
 *              set function is called. Debouncing is also handled by this function
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Pir::SetSelf(Pir *mySelf_p)
{
    Pir::mySelf_p = mySelf_p;
} 

/**---------------------------------------------------------------------------------------
 * @brief     This function handles the external button input and updates the value
 *              of the simpleLightState_bolst variable. If the value changed the corresponding
 *              set function is called. Debouncing is also handled by this function
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Pir::UpdatePirState() 
{
  // check if we changed PIR state
  if((HIGH == digitalRead(Pir::mySelf_p->pirPin_u8)) && (false == Pir::mySelf_p->pirState_bol))
  {
    Pir::mySelf_p->pirState_bol = true;
    Pir::mySelf_p->p_trace->println(trace_INFO_MSG, 
                                                "<<pir>>Motion detected");
    Pir::mySelf_p->publishState_bol = true;  
    if(0xff != Pir::mySelf_p->ledPin_u8)
    {
      digitalWrite(Pir::mySelf_p->ledPin_u8, LOW);
    }                                              
  }
  else if((LOW == digitalRead(Pir::mySelf_p->pirPin_u8)) && (true == Pir::mySelf_p->pirState_bol))
  {
    Pir::mySelf_p->pirState_bol = false;
    Pir::mySelf_p->p_trace->println(trace_INFO_MSG, 
                                                "<<pir>>No Motion detected");
    Pir::mySelf_p->publishState_bol = true; 
    if(0xff != Pir::mySelf_p->ledPin_u8)
    {
      digitalWrite(Pir::mySelf_p->ledPin_u8, HIGH);
    }                                            
  }
  else
  {
    Pir::mySelf_p->pirState_bol = false;  
  }
}

