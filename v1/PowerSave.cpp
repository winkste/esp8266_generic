/*****************************************************************************************
* FILENAME :        PowerSave.cpp
*
* DESCRIPTION :
*       Power Save Mode Handler class
*
* PUBLIC FUNCTIONS :
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
* AUTHOR :    Stephan Wink        START DATE :    26.05.2018
*
*****************************************************************************************/

/****************************************************************************************/
/* Include Interfaces */
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#include "MqttDevice.h"        
#include "Trace.h"
#include "PubSubClient.h"

#include "PowerSave.h"

/****************************************************************************************/
/* Local constant defines */

#define MICROSEC_IN_SEC           1000000l // microseconds in seconds
#define MILLISEC_IN_SEC           1000l // milliseconds in seconds
#define DEFAULT_POWER_SAVE_TIME   900l * MICROSEC_IN_SEC  // = 900 secs = 15mins power save
#define DEFAULT_POWER_ON_TIME     60l * MILLISEC_IN_SEC  // = 60 secs = 1mins power on

#define MQTT_SUB_PWR_SAVE_CMD     "/r/pwr/cmd" // command message for power save handler
#define MQTT_PAYLOAD_CMD_ON       "ON"
#define MQTT_PAYLOAD_CMD_OFF      "OFF"

#define MQTT_PUB_PWR_SAVE_STATE   "/s/pwr/state" // state

#define POWER_UP                  0u
#define POWER_INITIALIZED         1u
#define POWER_TIMER_ACTIVE        2u
#define POWER_SLEEPING            3u
#define POWER_DEACTIVATED         4u

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for PowerSave
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
PowerSave::PowerSave(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0l;
    this->publications_u16 = 0l;
    this->pwrSaveMode_bol = false;
    this->pwrOnTimeMSec_u32 = DEFAULT_POWER_ON_TIME;
    this->pwrSaveTimeMSec_u32 = DEFAULT_POWER_SAVE_TIME;
    this->actualState_u8 = POWER_UP;
    this->timer_u32 = 0l;
    p_trace->println(trace_INFO_MSG, "<<pwr>> power up");
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for PowerSave
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace             trace object for info and error messages
 * @param     powerSaveMode_bol   true = power save mode active, else false
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
PowerSave::PowerSave(Trace *p_trace, bool powerSaveMode_bol) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0l;
    this->publications_u16 = 0l;
    this->pwrSaveMode_bol = powerSaveMode_bol;
    this->pwrOnTimeMSec_u32 = DEFAULT_POWER_ON_TIME;
    this->pwrSaveTimeMSec_u32 = DEFAULT_POWER_SAVE_TIME;
    this->actualState_u8 = POWER_UP;
    this->timer_u32 = 0l;
    p_trace->println(trace_INFO_MSG, "<<pwr>> power up");
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for PowerSave
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace             trace object for info and error messages
 * @param     powerSaveMode_bol   true = power save mode active, else false
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
PowerSave::PowerSave(Trace *p_trace, bool powerSaveMode_bol, uint16_t pwrOnTimeSec_u16, 
                                uint16_t pwrSaveTimeSec_u16) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0l;
    this->publications_u16 = 0l;
    this->pwrSaveMode_bol = powerSaveMode_bol;
    this->actualState_u8 = POWER_UP;
    this->timer_u32 = 0l;
    p_trace->println(trace_INFO_MSG, "<<pwr>> power up");
    this->pwrOnTimeMSec_u32 = pwrOnTimeSec_u16 * MILLISEC_IN_SEC;
    this->pwrSaveTimeMSec_u32 = pwrSaveTimeSec_u16 * MICROSEC_IN_SEC;

}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
PowerSave::~PowerSave()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the Power Save object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void PowerSave::Initialize()
{
    // ensure DHT is powered down
    p_trace->println(trace_INFO_MSG, "<<pwr>> initialize"); 
    this->isInitialized_bol = true;
    this->actualState_u8 = POWER_INITIALIZED;
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to initialize the MQTT interface for this device
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client_p  MQTT object for message transfer
 * @param     dev_p     string identifier of the MQTT device id
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void PowerSave::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->println(trace_INFO_MSG, "<<pwr>> connected");
        // ... and resubscribe
        client_p->subscribe(build_topic(MQTT_SUB_PWR_SAVE_CMD));  
        client_p->loop();
        p_trace->print(trace_INFO_MSG, "<<pwr>> subscribed 1: ");
        p_trace->println(trace_PURE_MSG, MQTT_SUB_PWR_SAVE_CMD);
    }
    else
    {
        // failure, not connected
        p_trace->println(trace_ERROR_MSG, "<<pwr>> uninizialized MQTT device detected");
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
void PowerSave::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true == this->isConnected_bol)
    {
        // received DHT command
        if (String(build_topic(MQTT_SUB_PWR_SAVE_CMD)).equals(p_topic)) 
        {
            p_trace->println(trace_INFO_MSG, "<<pwr>> mqtt callback");
            p_trace->println(trace_INFO_MSG, p_topic);
            p_trace->println(trace_INFO_MSG, p_payload);
            // test if the payload is equal to "ON" or "OFF"
            if(0 == p_payload.indexOf(String(MQTT_PAYLOAD_CMD_ON))) 
            {
                this->pwrSaveMode_bol = false; 
                this->actualState_u8 = POWER_DEACTIVATED;
            }
            else if(0 == p_payload.indexOf(String(MQTT_PAYLOAD_CMD_OFF)))
            {
                this->pwrSaveMode_bol = true;
            }
            else
            {
                p_trace->print(trace_ERROR_MSG, "<<pwr>> unexpected payload: "); 
                p_trace->println(trace_PURE_MSG, p_payload);
            }   
        } 
    }
    else
    {
        p_trace->println(trace_ERROR_MSG, "<<pwr>>connection failure in CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool PowerSave::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret_bol = true;

    switch (this->actualState_u8)
    {
        case POWER_INITIALIZED:
            this->prevTime_u32 = millis();
            this->timer_u32 = 0u;
            this->actualState_u8 = POWER_TIMER_ACTIVE;
            p_trace->println(trace_INFO_MSG, "<<pwr>> timer activated");
            if(true == this->isConnected_bol)
            {
                ret_bol = client->publish(build_topic(MQTT_PUB_PWR_SAVE_STATE), 
                                        MQTT_PAYLOAD_CMD_ON, true);
            }
            else
            {
                ret_bol = false;
            }
            break;
        case POWER_TIMER_ACTIVE:
            this->timer_u32 += (millis() - this->prevTime_u32);
            this->prevTime_u32 = millis();
            if(this->timer_u32 > this->pwrOnTimeMSec_u32)
            {
                this->actualState_u8 = POWER_SLEEPING;
                this->timer_u32 = 0u;
                p_trace->println(trace_INFO_MSG, "<<pwr>> go to sleep mode");
                if(true == this->isConnected_bol)
                {
                    ret_bol = client->publish(build_topic(MQTT_PUB_PWR_SAVE_STATE), 
                                            MQTT_PAYLOAD_CMD_OFF, true);
                }
                else
                {
                    ret_bol = false;
                }
                delay(500);
                // power save time in seconds
                ESP.deepSleep(this->pwrSaveTimeMSec_u32); 
                delay(100);
            }
            else
            {
                /*p_trace->print(trace_INFO_MSG, "<<pwr>> actual timer counter in secs:");
                p_trace->println(trace_PURE_MSG, 
                                            (uint16_t)(this->timer_u32 / MILLISEC_IN_SEC));*/
            }
            break;
        default:
            p_trace->println(trace_INFO_MSG, "<<pwr>> unexpected state reached");
            break;
    }

    return ret_bol;  
}

/****************************************************************************************/
/* Private functions: */

/****************************************************************************************/
/* Protected functions: */
/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     topic       pointer to topic string
 * @return    combined topic as char pointer, it uses buffer_stca to store the topic
*//*-----------------------------------------------------------------------------------*/
char* PowerSave::build_topic(const char *topic) 
{
  sprintf(buffer_ca, "std/%s%s", this->dev_p, topic);
  return buffer_ca;
}
