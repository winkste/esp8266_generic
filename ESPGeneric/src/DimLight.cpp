/*****************************************************************************************
* FILENAME :        DimLight.cpp
*
* DESCRIPTION :
*       Class header for a dimmable light controlled via one pin 
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
* AUTHOR :    Stephan Wink        START DATE :    09.09.2018
*
*****************************************************************************************/

/****************************************************************************************/
/* Include Interfaces */
#include "DimLight.h"

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"
#include "Utils.h"

#include <math.h>

/****************************************************************************************/
/* Local constant defines */
#define MQTT_SUB_TOGGLE           "toggle" // command message for toggle command
#define MQTT_SUB_SWITCH           "switch" // command message for digital switch
#define MQTT_PUB_LIGHT_STATE      "status" //digital state of light
#define MQTT_SUB_BRIGHTNESS       "brightness" // command message for button commands
#define MQTT_PUB_BRIGHTNESS       "brightness" // state message for brightness
#define MQTT_DEFAULT_CHAN         "light_one"
#define MQTT_PAYLOAD_CMD_ON       "ON"
#define MQTT_PAYLOAD_CMD_OFF      "OFF"

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the single dimmable light 
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace       trace object for info and error messages
 * @param     gpio_p        gpio object configured as output
 * @param     light_Chan_p  light topic message with channel information
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DimLight::DimLight(Trace *p_trace, GpioDevice  *gpio_p, 
                                const char* lightChan_p) : MqttDevice(p_trace)
{
    this->deviceName_ccp    = DEVICE_NAME;
    this->prevTime_u32      = 0;
    this->publications_u16  = 0;
    this->lightState_bol    = false;
    this->publishState_bol  = true;
    this->channel_p         = lightChan_p; 
    this->gpio_p            = gpio_p;
    this->maxDigit_u16      = 1023U;
    this->brightness_u8     = 20;   // start with 20% brightness
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the single dimmable light 
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace       trace object for info and error messages
 * @param     gpio_p        gpio object configured as output
 * @param     light_Chan_p  light topic message with channel information
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DimLight::DimLight(Trace *p_trace, GpioDevice  *gpio_p, const char* lightChan_p, 
                    uint16_t maxDigit_u16) 
                    : DimLight(p_trace, gpio_p, lightChan_p)
{
    this->maxDigit_u16      = 1023U;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DimLight::~DimLight()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the single relay object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void DimLight::Initialize()
{
    this->isInitialized_bol = true;
    p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
    p_trace->println(trace_PURE_MSG, " initialized");
    this->TurnLightOff();
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to initialize the MQTT interface for this module
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @param     dev_p      client device id for building the mqtt topics
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void DimLight::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
        p_trace->println(trace_PURE_MSG, " reconnected");
        // ... and resubscribe
        // toggle light 
        this->Subscribe(client_p, MQTT_SUB_TOGGLE);
        // change light state digital with payload
        this->Subscribe(client_p, MQTT_SUB_SWITCH);  
        // command to set brightness of light 
        this->Subscribe(client_p, MQTT_SUB_BRIGHTNESS); 
    }
    else
    {
        // failure, not connected
        p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
        p_trace->println(trace_PURE_MSG, " uninizialized MQTT client detected");
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
void DimLight::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true == this->isConnected_bol)
    {
        // received toggle light mqtt topic
        if (String(BuildReceiveTopic(MQTT_SUB_TOGGLE)).equals(p_topic)) 
        {
            p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
            p_trace->print(trace_PURE_MSG, "<<dimLight>> mqtt callback: ");
            p_trace->println(trace_PURE_MSG, p_topic);
            this->ToggleLight();
        }
        // execute command to switch on/off the light
        else if (String(BuildReceiveTopic(MQTT_SUB_SWITCH)).equals(p_topic)) 
        {
            p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
            p_trace->println(trace_PURE_MSG, " mqtt callback: ");
            p_trace->print(trace_PURE_MSG, p_topic);
            p_trace->print(trace_PURE_MSG, " : ");
            p_trace->println(trace_PURE_MSG, p_payload);
            // test if the payload is equal to "ON" or "OFF"
            if(0 == p_payload.indexOf(String(MQTT_PAYLOAD_CMD_ON))) 
            {
                this->lightState_bol = true;
                this->SetLight();  
            }
            else if(0 == p_payload.indexOf(String(MQTT_PAYLOAD_CMD_OFF)))
            {
                this->lightState_bol = false;
                this->SetLight();
            }
            else
            {
                p_trace->print(trace_ERROR_MSG, this->deviceName_ccp);
                p_trace->println(trace_PURE_MSG, " unexpected payload: "); 
                p_trace->println(trace_PURE_MSG, p_payload);
            }   
        } 
                // execute command to change brightness of light
        else if (String(BuildReceiveTopic(MQTT_SUB_BRIGHTNESS)).equals(p_topic)) 
        {
            p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
            p_trace->println(trace_PURE_MSG, " mqtt callback: ");
            p_trace->print(trace_PURE_MSG, p_topic);
            p_trace->print(trace_PURE_MSG, " : ");
            p_trace->println(trace_PURE_MSG, p_payload);

            uint32_t payLoad_u32 = p_payload.toInt(); 
            // test if the payload is integer and in range
            if(payLoad_u32 < 100 && payLoad_u32 >= 0)
            {
                this->brightness_u8 = (uint8_t)payLoad_u32;
                this->SetLight();
            }
            else
            {
                p_trace->print(trace_ERROR_MSG, this->deviceName_ccp);
                p_trace->println(trace_PURE_MSG, " unexpected payload: "); 
                p_trace->println(trace_PURE_MSG, p_payload);
            }   
        } 
    }
    else
    {
        p_trace->print(trace_ERROR_MSG, this->deviceName_ccp);
        p_trace->println(trace_PURE_MSG, "connection failure in CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool DimLight::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret = false;
    
    if(true == this->isConnected_bol)
    {
        // check if state has changed, than publish this state
        if(true == publishState_bol)
        {
            if(true == this->lightState_bol)
            {
              ret = PublishMessage(client, MQTT_PUB_LIGHT_STATE, MQTT_PAYLOAD_CMD_ON);
            }
            else
            {
              ret = PublishMessage(client, MQTT_PUB_LIGHT_STATE, MQTT_PAYLOAD_CMD_OFF); 
            } 

            ret = PublishMessage(client, MQTT_PUB_BRIGHTNESS, 
                                    Utils::IntegerToDecString(this->brightness_u8, 
                                                                &this->mqttPayload[0]));
            if(ret)
            {
                publishState_bol = false;     
            }
        }
    }
    else
    {
        p_trace->print(trace_ERROR_MSG, this->deviceName_ccp);
        p_trace->println(trace_PURE_MSG, " connection failure in ProcessPublishRequests "); 
    }
    return ret; 
};

/**---------------------------------------------------------------------------------------
 * @brief     This function updates the light
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void DimLight::SetLight(void)
{
  if((false == this->lightState_bol) || (0 == this->brightness_u8)) 
  {   
    this->TurnLightOff();  
  }
  else
  {   
    this->TurnLightOn();
  }
}

/****************************************************************************************/
/* Private functions: */
/**---------------------------------------------------------------------------------------
 * @brief     This function turns the light off
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void DimLight::TurnLightOff(void)
{
    if(true == this->isInitialized_bol)
    {
        this->lightState_bol = false;
        this->gpio_p->AnalogWrite(0);    
        p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
        p_trace->println(trace_PURE_MSG, "light turned off");
        this->publishState_bol = true;
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function turns the light on
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void DimLight::TurnLightOn(void)
{
    if(true == this->isInitialized_bol)
    {
        this->lightState_bol = true;
        /*this->gpio_p->AnalogWrite(
            (uint16_t)(((float)this->brightness_u8 * (1023.0F/100.0F)) + 0.5F));*/
        this->gpio_p->AnalogWrite(Utils::CalcLogDigitsFromPercent(this->brightness_u8, 
                                                                    this->maxDigit_u16));
        p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
        p_trace->println(trace_PURE_MSG, "light turned on");
        this->publishState_bol = true;
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function toggles the light
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void DimLight::ToggleLight(void)
{
  if(true == this->lightState_bol)
  {   
    this->TurnLightOff();  
  }
  else
  {   
    this->TurnLightOn();
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     topic       pointer to topic string
 * @return    combined topic as char pointer, it uses buffer_stca to store the topic
*//*-----------------------------------------------------------------------------------*/
char* DimLight::BuildReceiveTopic(const char *topic) 
{
    return (Utils::BuildReceiveTopic(this->dev_p, this->channel_p, 
                                      topic, this->topicBuff_ca));
}

/**---------------------------------------------------------------------------------------
 * @brief     This function publishes a message
 * @author    winkste
 * @date      24 Sep. 2018
 * @param     client_p       pointer to pub sub client
 * @param     message_cp       pointer to topic string
 * @param     payload_ccp       pointer to topic payload string
 * @return    true, if message was successful send
*//*-----------------------------------------------------------------------------------*/
boolean DimLight::PublishMessage(PubSubClient *client_p, const char *message_cp, 
                                  const char * payload_ccp)
{
    boolean ret_bol = false;

    p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
    p_trace->print(trace_PURE_MSG, "publish message: ");
    p_trace->print(trace_PURE_MSG, 
                    Utils::BuildSendTopic(this->dev_p, this->channel_p, 
                                          message_cp, this->topicBuff_ca));
    p_trace->print(trace_PURE_MSG, "  :  ");
    ret_bol = client_p->publish(Utils::BuildSendTopic(this->dev_p, this->channel_p, 
                                          message_cp, this->topicBuff_ca), 
                                  payload_ccp, true);
    p_trace->println(trace_PURE_MSG, payload_ccp);
    
    return(ret_bol);
}

/**---------------------------------------------------------------------------------------
 * @brief     This function publishes a message
 * @author    winkste
 * @date      24 Sep. 2018
 * @param     client_p       pointer to pub sub client
 * @param     topic_ccp      topic fragment
 * @return    N/A
*//*-----------------------------------------------------------------------------------*/
void DimLight::Subscribe(PubSubClient *client_p, const char *topic_ccp)
{
    const char *localTopic_ccp = BuildReceiveTopic(topic_ccp);

    client_p->subscribe(localTopic_ccp);  
    client_p->loop();
    p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
    p_trace->print(trace_PURE_MSG, "subscribe message: ");
    p_trace->println(trace_PURE_MSG, localTopic_ccp);
}

