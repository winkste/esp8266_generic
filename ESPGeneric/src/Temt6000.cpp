/*****************************************************************************************
* FILENAME :        Temt6000.cpp
*
* DESCRIPTION :
*       Class file for Temt6000 brightness sensor
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
* AUTHOR :    Stephan Wink        START DATE :    09.07.20118
*
*****************************************************************************************/

/****************************************************************************************/
/* Include Interfaces */
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#include "MqttDevice.h"        
#include "Trace.h"
#include "PubSubClient.h"
#include "Utils.h"

#include "Temt6000.h" 

/****************************************************************************************/
/* Local constant defines */
#define DEFAULT_DATAPIN           0u  // A0

#define DARK_LEVEL                6U

#define MICROSEC_IN_SEC           1000000l // microseconds in seconds
#define MILLISEC_IN_SEC           1000l // milliseconds in seconds

#define MQTT_PUB_BRIGHTNESS       "s/temt6000/raw" // raw sensor data in digits
#define MQTT_PUB_BRIGHT_LEVEL     "s/temt6000/level" // brightness level data
#define MQTT_REPORT_INTERVAL      (2l * MILLISEC_IN_SEC) // 1 second between processing
#define SENSOR_AVERAGES           10U
/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief       Constructor for Temt6000
 * @author      winkste
 * @date        04 Jul. 2018
 * @param[in]   p_trace     trace object for info and error messages
 * @return      n/a
*//*-----------------------------------------------------------------------------------*/
Temt6000::Temt6000(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32          = 0UL;
    this->pwrPin_p              = NULL;
    this->brightPin_p           = NULL;
    this->level_u8              = 0U;
    this->level_chrp            = MQTT_PUB_PAY_LEVEL_DARK;
    this->lastLevel_u8          = 1U;
    this->lastBrightness_f32    = 999.0F;
    this->rawData_u16           = 0U;
    this->brightness_f32        = 0.0F;
    this->brightId_u8           = 0U;
    this->reportCycleMSec_u32   = MQTT_REPORT_INTERVAL;

    this->avgCnt_u16            = 0U;
    this->avgData_32            = 0;
}

/**---------------------------------------------------------------------------------------
 * @brief       Constructor for Temt6000
 * @author      winkste
 * @date        04 Jul. 2018
 * @param[in]   p_trace     trace object for info and error messages
 * @param[in]   pwrPin_p    handle to power pin of gen tyype GpioDevice
 * @return      n/a
*//*-----------------------------------------------------------------------------------*/
Temt6000::Temt6000(Trace *p_trace, GpioDevice *pwrPin_p) : Temt6000(p_trace)
{
    this->pwrPin_p              = pwrPin_p;
}

/**---------------------------------------------------------------------------------------
 * @brief       Constructor for Temt6000
 * @author      winkste
 * @date        04 Jul. 2018
 * @param[in]   p_trace         trace object for info and error messages
 * @param[in]   pwrPin_p        handle to power pin of gen tyype GpioDevice
 * @param[in]   brightPin_p     toggle pin for darkness / brightness detection
 * @param[in]   brightId_u8     sensor id if more than one are used in a device
 * @return      n/a
*//*-----------------------------------------------------------------------------------*/
Temt6000::Temt6000(Trace *p_trace, GpioDevice *pwrPin_p, GpioDevice *brightPin_p,
                                uint8_t brightId_u8) : Temt6000(p_trace)
{
    this->pwrPin_p      = pwrPin_p;
    this->brightPin_p   = brightPin_p;
    this->brightId_u8   = brightId_u8;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Temt6000::~Temt6000()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the DHT Sensor object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Temt6000::Initialize()
{
    p_trace->println(trace_INFO_MSG, "<<temt6000>> initialize");
    this->PowerOff();
    if(NULL != this->brightPin_p)
    {
        this->brightPin_p->DigitalWrite(LOW);   
    }
    this->isInitialized_bol = true;
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to initialize the MQTT interface for this sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client_p  MQTT object for message transfer
 * @param     dev_p     string identifier of the MQTT device id
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Temt6000::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->println(trace_INFO_MSG, "<<temt6000>> connected");
    }
    else
    {
        // failure, not connected
        p_trace->println(trace_ERROR_MSG, 
                    "<<temt6000>> uninizialized MQTT client detected");
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
void Temt6000::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true != this->isConnected_bol)
    {
        p_trace->println(trace_ERROR_MSG, 
                                "<<temt6000>> connection failure in CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool Temt6000::ProcessPublishRequests(PubSubClient *client)
{
    boolean ret_bol = true;
    uint32_t actualTime_u32 = millis();

    if(    (this->prevTime_u32 + this->STATE_LOOP_CYCLE < actualTime_u32) 
        || (0 == this->prevTime_u32))
    {
        this->prevTime_u32 = actualTime_u32;
        ret_bol = ret_bol && ProcessSensorStateMachine(client);
    }

    return(ret_bol);
}

/****************************************************************************************/
/* Private functions: */

/**--------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     topic       pointer to topic string
 * @return    combined topic as char pointer, it uses buffer_stca to store the topic
*//*-----------------------------------------------------------------------------------*/
char* Temt6000::build_topic(const char *topic) 
{
  if(0 == this->brightId_u8)
  {
      sprintf(buffer_ca, "std/%s%s", this->dev_p, topic);
  }
  else
  {
      sprintf(buffer_ca, "std/%s%s%d", this->dev_p, topic, this->brightId_u8);  
  }
  return buffer_ca;
}

/****************************************************************************************/
/* Protected functions: */

/**---------------------------------------------------------------------------------------
 * @brief     This function checks if we have to start a new measurement cycle
 * @author    winkste
 * @date      13 Jul. 2020
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Temt6000::CheckForMeasRequest(void)
{
    // no trigger needed here, directly start a new measurement cycle
    this->state_en = TEMT6000_MEAS_REQ;
    this->avgCnt_u16 = 0U;
    this->avgData_32 = 0;
}

/**--------------------------------------------------------------------------------------
 * @brief     This function turns on the Temt6000 power pin and waits for a dedicated 
 *              time.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Temt6000::PowerOn() 
{ 
    if(NULL != pwrPin_p)
    {
        this->pwrPin_p->DigitalWrite(HIGH);
        p_trace->println(trace_INFO_MSG, "<<temt6000>> turned on");
    } 
}

/**--------------------------------------------------------------------------------------
 * @brief     This function turns off the Temt6000 power pin.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Temt6000::PowerOff() 
{ 
    if(NULL != this->pwrPin_p)
    {
        this->pwrPin_p->DigitalWrite(LOW);
        p_trace->println(trace_INFO_MSG, "<<temt6000>> turned off");
    }
}

/**--------------------------------------------------------------------------------------
 * @brief     This function reads the internal adc channel.
 * @author    S. Wink
 * @date      04 Jul 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Temt6000::ReadData(void) 
{
    uint16_t temData_u16;

    // read data from internal ADC and summarize it
    temData_u16 = analogRead(DEFAULT_DATAPIN);
    this->avgData_32 += temData_u16;
    this->avgCnt_u16++;

    if(SENSOR_AVERAGES < this->avgCnt_u16)
    {
        // measurement cycle completed
        this->rawData_u16 = (uint16_t) this->avgData_32 / this->avgCnt_u16;
        this->avgData_32 = 0;
        this->avgCnt_u16 = 0U;
        this->state_en = TEMT6000_SAMPLE_COMPLETED;
    }
}

/**--------------------------------------------------------------------------------------
 * @brief     This function processes the brigthness value and level.
 * @author    S. Wink
 * @date      04 Jul 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Temt6000::ProcessBrightness(void) 
{
    // calculate moisture in % based on min (dark) and max (bright) range
    this->brightness_f32 = this->rawData_u16;

    // calculate brightness level for dark = 0, bright = 1
    if(this->brightness_f32 <= DARK_LEVEL)
    {
        this->level_u8 = 0U;
        this->level_chrp = MQTT_PUB_PAY_LEVEL_DARK;
        if(NULL != this->brightPin_p)
        {
            this->brightPin_p->DigitalWrite(HIGH);    
        }
    }
    else
    {
        this->level_u8 = 1U;
        this->level_chrp = MQTT_PUB_PAY_LEVEL_BRIGHT;
        if(NULL != this->brightPin_p)
        {
            this->brightPin_p->DigitalWrite(LOW);    
        }
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function publishes the data to the broker
 * @author    winkste
 * @date      13 Jul. 2020
 * @param     client     mqtt client object
 * @return    true if transmission was successful
*//*-----------------------------------------------------------------------------------*/
boolean Temt6000::PublishData(PubSubClient *client)
{
    String tPayload;
    boolean ret_bol = true;
    char buff_ca[20];

    if(true == this->isConnected_bol)
    {
        if(    (this->level_u8 != this->lastLevel_u8) 
            || (this->brightness_f32 != this->lastBrightness_f32))
        {
            this->lastLevel_u8 = this->level_u8;
            this->lastBrightness_f32 = this->brightness_f32;
            p_trace->print(trace_INFO_MSG, "<<temt6000>> publish brigthness: ");
            p_trace->print(trace_PURE_MSG, Utils::BuildSendTopic(this->dev_p, 
                                            MQTT_PUB_BRIGHTNESS, this->buffer_ca));
            p_trace->print(trace_PURE_MSG, "  :  ");
            p_trace->println(trace_PURE_MSG, this->rawData_u16);
            /*ret_bol = client->publish(build_topic(MQTT_PUB_BRIGHTNESS), 
                                Utils::IntegerToDecString(this->rawData_u16, &buff_ca[0]), true);*/
            ret_bol = client->publish(
                    Utils::BuildSendTopic(this->dev_p, MQTT_PUB_BRIGHTNESS, this->buffer_ca), 
                    Utils::IntegerToDecString(this->rawData_u16, &buff_ca[0]), true);

            

            p_trace->print(trace_INFO_MSG, "<<temt6000>> publish level: ");
            p_trace->print(trace_PURE_MSG, Utils::BuildSendTopic(this->dev_p, 
                                            MQTT_PUB_BRIGHT_LEVEL, this->buffer_ca));
            p_trace->print(trace_PURE_MSG, "  :  ");
            ret_bol = client->publish(Utils::BuildSendTopic(this->dev_p, 
                                            MQTT_PUB_BRIGHT_LEVEL, this->buffer_ca), 
                                this->level_chrp, true);
            p_trace->println(trace_PURE_MSG, this->level_chrp);
        } 
    }
    else
    {
        ret_bol = false;
        p_trace->println(trace_ERROR_MSG, 
                    "<<temt6000>> connection failure in dht ProcessPublishRequests ");  
    }
    this->state_en = TEMT6000_MEAS_PUBLISHED;

    return(ret_bol);
} 

/**---------------------------------------------------------------------------------------
 * @brief     This function handles the sensor state machine for a measurement interval
 * @author    winkste
 * @date      13 Jul. 2020
 * @param     client     mqtt client object
 * @return    true if transmission was successful
*//*-----------------------------------------------------------------------------------*/
boolean Temt6000::ProcessSensorStateMachine(PubSubClient *client)
{
    boolean ret_bol = true;

    switch(this->state_en)
    {
        case TEMT6000_OFF:
            CheckForMeasRequest();
            break;
        case TEMT6000_MEAS_REQ:
            PowerOn();
            this->state_en = TEMT6000_POWER_STARTED;
            break;
        case TEMT6000_POWER_STARTED:
            ReadData();
            break;
        case TEMT6000_SAMPLE_COMPLETED:
            ProcessBrightness();
            PowerOff();
            this->state_en = TEMT6000_MEAS_COMPLETED;
            break;
        case TEMT6000_MEAS_COMPLETED:
            ret_bol = ret_bol && PublishData(client);
            break;
        case TEMT6000_MEAS_PUBLISHED:
            this->state_en = TEMT6000_OFF;
            break;
        case TEMT6000_UNKNOWN_STATE:
        default:
            this->state_en = TEMT6000_OFF;
            break;
    }

    return(ret_bol);
}

