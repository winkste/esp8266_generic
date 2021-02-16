/*****************************************************************************************
* FILENAME :        SonoffBasic.cpp
*
* DESCRIPTION :
*       Implementation of the sonoff basic device
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
#include "SonoffBasic.h"

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"

/****************************************************************************************/
/* Local constant defines */
#define RELAY_PIN                 12 // D6
//#define LED_PIN                   13 // D7
#define BUTTON_INPUT_PIN          0  // D3
#define MQTT_SUB_TOGGLE           "/r/so_basic/toggle" // command message for toggle command
#define MQTT_SUB_BUTTON           "/r/so_basic/switch" // command message for button commands
#define MQTT_PUB_LIGHT_STATE      "/s/so_basic/status" //state of relay
#define MQTT_PAYLOAD_CMD_ON       "ON"
#define MQTT_PAYLOAD_CMD_OFF      "OFF"

#define BUTTON_TIMEOUT            1500  // max 1500ms timeout between each button press to count up (start of config)
#define BUTTON_DEBOUNCE           400  // ms debouncing for the botton

#define LED_PIN_UNUSED            0xFF

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Static Data instantiation */
SonoffBasic *SonoffBasic::mySelf_p          = NULL;
uint32_t SonoffBasic::timerButtonDown_u32   = 0;
uint8_t SonoffBasic::counterButton_u8       = 0;

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the single relay shield
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
SonoffBasic::SonoffBasic(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32      = 0;
    this->publications_u16  = 0;
    this->relayState_bol    = false;
    this->publishState_bol  = true;
    this->ledPin_u8         = LED_PIN_UNUSED;
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the single SONOFF board
 * @author    winkste
 * @date      16 Feb. 2021
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
SonoffBasic::SonoffBasic(Trace *p_trace, uint8_t ledPin_u8) : SonoffBasic(p_trace)
{
    this->ledPin_u8         = ledPin_u8;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
SonoffBasic::~SonoffBasic()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the single relay object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void SonoffBasic::Initialize()
{
    p_trace->println(trace_INFO_MSG, "Single relay initialized");
    
    pinMode(RELAY_PIN, OUTPUT);
    if(LED_PIN_UNUSED != this->ledPin_u8)
    {
      pinMode(this->ledPin_u8, OUTPUT);
    }
    this->setRelay();

    pinMode(BUTTON_INPUT_PIN, INPUT);
    digitalWrite(BUTTON_INPUT_PIN, HIGH); // pull up to avoid interrupts without sensor
    attachInterrupt(digitalPinToInterrupt(BUTTON_INPUT_PIN), 
                                            SonoffBasic::UpdateBUTTONstate, CHANGE);

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
void SonoffBasic::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->println(trace_INFO_MSG, "Single relay reconnected");
        // ... and resubscribe
        // toggle relay
        client_p->subscribe(build_topic(MQTT_SUB_TOGGLE));  
        client_p->loop();
        p_trace->print(trace_INFO_MSG, "<<mqtt>> subscribed 1: ");
        p_trace->println(trace_PURE_MSG, MQTT_SUB_TOGGLE);
        // change relay state with payload
        client_p->subscribe(build_topic(MQTT_SUB_BUTTON));  
        client_p->loop();
        p_trace->print(trace_INFO_MSG, "<<mqtt>> subscribed 2: ");
        p_trace->println(trace_PURE_MSG, MQTT_SUB_BUTTON);
        client_p->loop();
    }
    else
    {
        // failure, not connected
        p_trace->println(trace_ERROR_MSG, 
                                "uninizialized MQTT client in single relay detected");
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
void SonoffBasic::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true == this->isConnected_bol)
    {
        // received toggle relay mqtt topic
        if (String(build_topic(MQTT_SUB_TOGGLE)).equals(p_topic)) 
        {
            p_trace->println(trace_INFO_MSG, "Single relay mqtt callback");
            p_trace->println(trace_INFO_MSG, p_topic);
            p_trace->println(trace_INFO_MSG, p_payload);
            this->ToggleRelay();
        }
        // execute command to switch on/off the relay
        else if (String(build_topic(MQTT_SUB_BUTTON)).equals(p_topic)) 
        {
            p_trace->println(trace_INFO_MSG, "Single relay mqtt callback");
            p_trace->println(trace_INFO_MSG, p_topic);
            p_trace->println(trace_INFO_MSG, p_payload);
            // test if the payload is equal to "ON" or "OFF"
            if(0 == p_payload.indexOf(String(MQTT_PAYLOAD_CMD_ON))) 
            {
                this->relayState_bol = true;
                this->setRelay();  
            }
            else if(0 == p_payload.indexOf(String(MQTT_PAYLOAD_CMD_OFF)))
            {
                this->relayState_bol = false;
                this->setRelay();
            }
            else
            {
                p_trace->print(trace_ERROR_MSG, "<<mqtt>> unexpected payload: "); 
                p_trace->println(trace_PURE_MSG, p_payload);
            }   
        } 
    }
    else
    {
        p_trace->println(trace_ERROR_MSG, "connection failure in sonoff CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool SonoffBasic::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret = false;
    
    if(true == this->isConnected_bol)
    {
        // check if state has changed, than publish this state
        if(true == publishState_bol)
        {
            p_trace->print(trace_INFO_MSG, "<<mqtt>> publish requested state: ");
            p_trace->print(trace_PURE_MSG, MQTT_PUB_LIGHT_STATE);
            p_trace->print(trace_PURE_MSG, "  :  ");
            if(true == this->relayState_bol)
            {
              ret = client->publish(build_topic(MQTT_PUB_LIGHT_STATE), 
                                        MQTT_PAYLOAD_CMD_ON, true);
              p_trace->println(trace_PURE_MSG, MQTT_PAYLOAD_CMD_ON);
            }
            else
            {
                ret = client->publish(build_topic(MQTT_PUB_LIGHT_STATE), 
                                          MQTT_PAYLOAD_CMD_OFF, true); 
                p_trace->println(trace_PURE_MSG, MQTT_PAYLOAD_CMD_OFF); 
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
                              "connection failure in relay ProcessPublishRequests "); 
    }
    return ret; 
};

/**---------------------------------------------------------------------------------------
 * @brief     This function toggles the relay
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void SonoffBasic::ToggleRelay(void)
{
  if(true == this->relayState_bol)
  {   
    this->TurnRelayOff();  
  }
  else
  {   
    this->TurnRelayOn();
  }
}

/****************************************************************************************/
/* Private functions: */
/**---------------------------------------------------------------------------------------
 * @brief     This function turns the relay off
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void SonoffBasic::TurnRelayOff(void)
{
  if(true == this->isInitialized_bol)
  {
      this->relayState_bol = false;
      digitalWrite(RELAY_PIN, LOW);
      if(LED_PIN_UNUSED != this->ledPin_u8)
      {
        digitalWrite(this->ledPin_u8, HIGH);
      }
      p_trace->println(trace_INFO_MSG, "relay turned off");
      this->publishState_bol = true;
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function turns the relay on
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void SonoffBasic::TurnRelayOn(void)
{
  if(true == this->isInitialized_bol)
  {
      this->relayState_bol = true;
      digitalWrite(RELAY_PIN, HIGH);
      if(LED_PIN_UNUSED != this->ledPin_u8)
      {
        digitalWrite(this->ledPin_u8, LOW);
      }
      p_trace->println(trace_INFO_MSG, "relay turned on");
      this->publishState_bol = true;
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function sets the relay based on the state of the relayState_bol
 *            attribute
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void SonoffBasic::setRelay(void)
{
  if(true == this->relayState_bol)
  {
    TurnRelayOn(); 
  }
  else
  {
    TurnRelayOff(); 
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
char* SonoffBasic::build_topic(const char *topic) 
{
  sprintf(buffer_ca, "std/%s%s", this->dev_p, topic);
  return buffer_ca;
}

/**---------------------------------------------------------------------------------------
 * @brief     This function handles the external button input and updates the value
 *              of the simpleLightState_bolst variable. If the value changed the corresponding
 *              set function is called. Debouncing is also handled by this function
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void SonoffBasic::SetSelf(SonoffBasic *mySelf_p)
{
    SonoffBasic::mySelf_p = mySelf_p;
} 

/**---------------------------------------------------------------------------------------
 * @brief     This function handles the external button input and updates the value
 *              of the simpleLightState_bolst variable. If the value changed the corresponding
 *              set function is called. Debouncing is also handled by this function
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
ICACHE_RAM_ATTR void SonoffBasic::UpdateBUTTONstate() 
{
  // toggle, write to pin, publish to server
  if(LOW == digitalRead(BUTTON_INPUT_PIN))
  {
    if((millis() - SonoffBasic::mySelf_p->timerButtonDown_u32) > BUTTON_DEBOUNCE)
    { // avoid bouncing
      // button down
      // toggle status of both lights
      SonoffBasic::mySelf_p->ToggleRelay();

      if(millis() - SonoffBasic::mySelf_p->timerButtonDown_u32 < BUTTON_TIMEOUT)
      {
        SonoffBasic::mySelf_p->counterButton_u8++;
        if(SonoffBasic::mySelf_p->counterButton_u8 >= 10)
        {
            MqttDevice::startWifiConfig_bol = true;
            SonoffBasic::mySelf_p->p_trace->println(trace_INFO_MSG, 
                                                "<<sonoff>> Reconfig request received");
            SonoffBasic::mySelf_p->counterButton_u8 = 1;
        }
      } 
      else 
      {
        SonoffBasic::counterButton_u8 = 1;
      }
      SonoffBasic::mySelf_p->p_trace->print(trace_INFO_MSG, "<<sonoff>> button push nr ");
      SonoffBasic::mySelf_p->p_trace->println(trace_PURE_MSG, mySelf_p->counterButton_u8);

    };
    SonoffBasic::timerButtonDown_u32 = millis();
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     Returns re-config request status and sets it to false
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    re-config request
*//*-----------------------------------------------------------------------------------*/
/*bool SonoffBasic::GetReconfigRequest() 
{
    bool ret_bol = SonoffBasic::startWifiConfig_bol;

    SonoffBasic::startWifiConfig_bol = false;
    return(ret_bol);
}*/


