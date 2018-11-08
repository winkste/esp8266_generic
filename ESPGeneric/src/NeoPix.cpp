/*****************************************************************************************
* FILENAME :        NeoPix.cpp
*
* DESCRIPTION :
*       Class implementation for a Adafruit based NeoPixels light, controlled via one pin 
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
#include "NeoPix.h"

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"
#include "Utils.h"
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

/****************************************************************************************/
/* Local constant defines */
#define MQTT_SUB_TOGGLE           "toggle" // command message for toggle command
#define MQTT_SUB_SWITCH           "switch" // command message for digital switch
#define MQTT_PUB_LIGHT_STATE      "status" //digital state of light
#define MQTT_SUB_RGB              "color" // command message for rgb state
#define MQTT_PUB_RGB              "color" // state message for rgb state
#define MQTT_SUB_BRIGHTNESS       "brightness" // command message for button commands
#define MQTT_PUB_BRIGHTNESS       "brightness" // state message for brightness
#define MQTT_DEFAULT_CHAN         "neo_one"
#define MQTT_PAYLOAD_CMD_ON       "ON"
#define MQTT_PAYLOAD_CMD_OFF      "OFF"

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the NeoPixels light 
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     trace_pcl     pointer to class of trace object for info and error messages
 * @param     gpio_pcl      pointer to class of gpio object
 * @param     neoChan_pch   pointer to character sequence for channel identifier
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
NeoPix::NeoPix(Trace *trace_pcl, GpioDevice  *gpio_pcl, const char* neoChan_pch) 
                : MqttDevice(trace_pcl)
{
    this->deviceName_ccp        = DEVICE_NAME;
    this->prevTime_u32          = 0U;
    this->publications_u16      = 0U;
    this->lightState_bol        = false;
    this->neoStateChanged_bol   = true;
    this->channel_pch           = neoChan_pch; 
    this->gpio_pcl              = gpio_pcl;
    this->brightness_u8         = 20U;   // start with 20% brightness
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
NeoPix::~NeoPix()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the single relay object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void NeoPix::Initialize()
{
    this->isInitialized_bol = true;
    // When we setup the NeoPixel library, we tell it how many pixels, and which pin 
    // to use to send signals. Note that for older NeoPixel strips you might need to 
    // change the third parameter--see the strandtest example for more information on 
    //possible values.
    this->pixels_pcl = new Adafruit_NeoPixel(this->numPixels_u16c, 
                                    gpio_pcl->GetPinNumber_u8(), NEO_GRB + NEO_KHZ800);
    this->pixels_pcl->begin(); // This initializes the NeoPixel library.

    p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
    p_trace->println(trace_PURE_MSG, " initialized");
    this->TurnOff_vd();
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to initialize the MQTT interface for this module
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @param     dev_p      client device id for building the mqtt topics
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void NeoPix::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
        p_trace->println(trace_PURE_MSG, " reconnected");
        // ... and resubscribe
        // toggle light 
        this->Subscribe_vd(client_p, MQTT_SUB_TOGGLE);
        // change light state digital with payload
        this->Subscribe_vd(client_p, MQTT_SUB_SWITCH);  
        // command to set brightness of light 
        this->Subscribe_vd(client_p, MQTT_SUB_BRIGHTNESS); 
        // command to set RGB of light 
        this->Subscribe_vd(client_p, MQTT_SUB_RGB);
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
void NeoPix::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true == this->isConnected_bol)
    {
        // received toggle light mqtt topic
        if (String(BuildReceiveTopic_pch(MQTT_SUB_TOGGLE)).equals(p_topic)) 
        {
            p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
            p_trace->print(trace_PURE_MSG, "<<dimLight>> mqtt callback: ");
            p_trace->println(trace_PURE_MSG, p_topic);
            this->Toggle_vd();
        }
        // execute command to switch on/off the light
        else if (String(BuildReceiveTopic_pch(MQTT_SUB_SWITCH)).equals(p_topic)) 
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
                this->Set_vd();  
            }
            else if(0 == p_payload.indexOf(String(MQTT_PAYLOAD_CMD_OFF)))
            {
                this->lightState_bol = false;
                this->Set_vd();
            }
            else
            {
                p_trace->print(trace_ERROR_MSG, this->deviceName_ccp);
                p_trace->println(trace_PURE_MSG, " unexpected payload: "); 
                p_trace->println(trace_PURE_MSG, p_payload);
            }   
        } 
        // execute command to change brightness of light
        else if (String(BuildReceiveTopic_pch(MQTT_SUB_BRIGHTNESS)).equals(p_topic)) 
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
                this->Set_vd();
            }
            else
            {
                p_trace->print(trace_ERROR_MSG, this->deviceName_ccp);
                p_trace->println(trace_PURE_MSG, " unexpected payload: "); 
                p_trace->println(trace_PURE_MSG, p_payload);
            }   
        } 
        // execute command to change RGB of light
        else if (String(BuildReceiveTopic_pch(MQTT_SUB_RGB)).equals(p_topic)) 
        {
            p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
            p_trace->println(trace_PURE_MSG, " mqtt callback: ");
            p_trace->print(trace_PURE_MSG, p_topic);
            p_trace->print(trace_PURE_MSG, " : ");
            p_trace->println(trace_PURE_MSG, p_payload);

            uint8_t firstIndex_u8 = p_payload.indexOf(',');
            uint8_t lastIndex_u8 = p_payload.lastIndexOf(',');
            
            uint8_t red_u8 = p_payload.substring(0, firstIndex_u8).toInt();
            uint8_t green_u8 = p_payload.substring(firstIndex_u8 + 1, lastIndex_u8).toInt();
            uint8_t blue_u8 = p_payload.substring(lastIndex_u8 + 1).toInt();

            this->red_u8 = red_u8;
            this->green_u8 = green_u8;
            this->blue_u8 = blue_u8;

            this->Set_vd();  
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
bool NeoPix::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret = false;
    
    if(true == this->isConnected_bol)
    {
        // check if state has changed, than publish this state
        if(true == neoStateChanged_bol)
        {
            if(true == this->lightState_bol)
            {
              ret = PublishMessage_bol(client, MQTT_PUB_LIGHT_STATE, MQTT_PAYLOAD_CMD_ON);
            }
            else
            {
              ret = PublishMessage_bol(client, MQTT_PUB_LIGHT_STATE, MQTT_PAYLOAD_CMD_OFF); 
            } 

            ret = PublishMessage_bol(client, MQTT_PUB_RGB, 
                                    Utils::RGBToString(this->red_u8, this->green_u8, 
                                                        this->blue_u8,
                                                        &this->mqttPayload_cha[0]));
            if(ret)
            {
                this->neoStateChanged_bol = false;     
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
void NeoPix::Set_vd(void)
{
  if(false == this->lightState_bol) 
  {   
    this->TurnOff_vd();  
  }
  else
  {   
    this->TurnOn_vd();
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
void NeoPix::TurnOff_vd(void)
{
    if(true == this->isInitialized_bol)
    {
        this->lightState_bol = false;
        this->pixels_pcl->setPixelColor(0, 0, 0, 0);
        this->pixels_pcl->show();   
        p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
        p_trace->println(trace_PURE_MSG, "light turned off");
        this->neoStateChanged_bol = true;
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function turns the light on
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void NeoPix::TurnOn_vd(void)
{
    if(true == this->isInitialized_bol)
    {
        this->lightState_bol = true;
        
        this->pixels_pcl->setPixelColor(0, this->pixels_pcl->Color(this->red_u8, 
                                                            this->green_u8,
                                                            this->blue_u8));
        this->pixels_pcl->setBrightness(this->brightness_u8);
        this->pixels_pcl->show(); // This sends the updated pixel color to the hardware.

        p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
        p_trace->println(trace_PURE_MSG, "light turned on");
        this->neoStateChanged_bol = true;
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function toggles the light
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void NeoPix::Toggle_vd(void)
{
  if(true == this->lightState_bol)
  {   
    this->TurnOff_vd();  
  }
  else
  {   
    this->TurnOn_vd();
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
char* NeoPix::BuildReceiveTopic_pch(const char *topic) 
{
    return (Utils::BuildReceiveTopic(this->dev_p, this->channel_pch, 
                                      topic, this->topicBuff_cha));
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
boolean NeoPix::PublishMessage_bol(PubSubClient *client_p, const char *message_cp, 
                                  const char * payload_ccp)
{
    boolean ret_bol = false;

    p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
    p_trace->print(trace_PURE_MSG, "publish message: ");
    p_trace->print(trace_PURE_MSG, 
                    Utils::BuildSendTopic(this->dev_p, this->channel_pch, 
                                          message_cp, this->topicBuff_cha));
    p_trace->print(trace_PURE_MSG, "  :  ");
    ret_bol = client_p->publish(Utils::BuildSendTopic(this->dev_p, this->channel_pch, 
                                          message_cp, this->topicBuff_cha), 
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
void NeoPix::Subscribe_vd(PubSubClient *client_p, const char *topic_ccp)
{
    const char *localTopic_cpch = BuildReceiveTopic_pch(topic_ccp);

    client_p->subscribe(localTopic_cpch);  
    client_p->loop();
    p_trace->print(trace_INFO_MSG, this->deviceName_ccp);
    p_trace->print(trace_PURE_MSG, "subscribe message: ");
    p_trace->println(trace_PURE_MSG, localTopic_cpch);
}


