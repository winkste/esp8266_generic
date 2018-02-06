/*****************************************************************************************
* FILENAME :        SingleRelay.cpp
*
* DESCRIPTION :
*       Implementation of the single relay shield.
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
#include "SingleRelay.h"

#include "MqttDevice.h"
#include "GpioDevice.h"
#include "Trace.h"

/****************************************************************************************/
/* Local constant defines */
#define RELAY_PIN                 5
#define MQTT_SUB_TOGGLE           "toggle" // command message for toggle command
#define MQTT_SUB_BUTTON           "switch" // command message for button commands
#define MQTT_PUB_LIGHT_STATE      "status" //state of relay
#define MQTT_DEFAULT_CHAN         "relay_one"
#define MQTT_PAYLOAD_CMD_ON       "ON"
#define MQTT_PAYLOAD_CMD_OFF      "OFF"

#define HIGH    1
#define LOW     0
#define OUTPUT  1

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the single relay shield
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @param     relayChan_p relay topic message with channel information
 * @param     invert_bol  false = pin HIGH = relay ON = relay led on
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
SingleRelay::SingleRelay(Trace *p_trace, GpioDevice  *gpio_p, char* relayChan_p,
                                              bool invert_bol) : MqttDevice(p_trace)
{
    this->prevTime_u32      = 0;
    this->publications_u16  = 0;
    this->relayState_bol    = false;
    this->publishState_bol  = true;
    this->channel_p         = relayChan_p; 
    this->invert_bol        = invert_bol; 
    this->gpio_p            = gpio_p;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
SingleRelay::~SingleRelay()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the single relay object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void SingleRelay::Initialize()
{
    this->isInitialized_bol = true;
    p_trace->println(trace_INFO_MSG, "<<singRel>>Single relay initialized");
    //this->setRelay();
    this->TurnRelayOff();
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to initialize the MQTT interface for this module
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @param     dev_p      client device id for building the mqtt topics
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void SingleRelay::Reconnect()
{
    p_trace->println(trace_INFO_MSG, "<<singRel>> switch reconnected");
    this->isConnected_bol = true;
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
void SingleRelay::CallbackMqtt(PubSubClient *client, char* p_topic, char* p_payload)
{
    p_trace->println(trace_INFO_MSG, "<<singRel>> callback");
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool SingleRelay::ProcessPublishRequests(PubSubClient *client)
{
    p_trace->println(trace_INFO_MSG, "<<singRel>> switch processes publish request");
    this->ToggleRelay();
        return 0;
};

/**---------------------------------------------------------------------------------------
 * @brief     This function toggles the relay
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void SingleRelay::ToggleRelay(void)
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
void SingleRelay::TurnRelayOff(void)
{
  if(true == this->isInitialized_bol)
  {
      this->relayState_bol = false;
      if(true == this->invert_bol)
      {
        this->gpio_p->DigitalWrite(gpioDevice_HIGH);
      }
      else
      {
        this->gpio_p->DigitalWrite(gpioDevice_LOW);
      }     
      p_trace->println(trace_INFO_MSG, "<<singRel>>relay turned off");
      this->publishState_bol = true;
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function turns the relay on
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    void
*//*-----------------------------------------------------------------------------------*/
void SingleRelay::TurnRelayOn(void)
{
  if(true == this->isInitialized_bol)
  {
      this->relayState_bol = true;
      if(true == this->invert_bol)
      {
          this->gpio_p->DigitalWrite(gpioDevice_LOW);
      }
      else
      {
          this->gpio_p->DigitalWrite(gpioDevice_HIGH);
      }
      p_trace->println(trace_INFO_MSG, "<<singRel>>relay turned on");
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
void SingleRelay::SetRelay(void)
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



