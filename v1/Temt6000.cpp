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

#include "Temt6000.h" 

/****************************************************************************************/
/* Local constant defines */
#define DEFAULT_DATAPIN           0u  // A0

#define DARK_LEVEL                300U
#define PUB_THRESHOLD             50U

#define MICROSEC_IN_SEC           1000000l // microseconds in seconds
#define MILLISEC_IN_SEC           1000l // milliseconds in seconds
#define WARM_UP_TIME              500   // wait 500 milliseconds

#define MQTT_PUB_BRIGHTNESS       "/s/temt6000/raw" // raw sensor data in digits
#define MQTT_PUB_BRIGHT_LEVEL     "/s/temt6000/level" // brightness level data
#define MQTT_REPORT_INTERVAL      (1l * MILLISEC_IN_SEC) // 1 second between processing
#define MQTT_THRESHOLD            50.0F
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
    this->lastLevel_u8          = 1U;
    this->lastBrightness_f32    = 999.0F;
    this->rawData_u16           = 0U;
    this->brightness_f32        = 0.0F;
    this->brightId_u8           = 0U;
    this->reportCycleMSec_u32   = MQTT_REPORT_INTERVAL;
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
    this->brightPin_p->DigitalWrite(LOW);   
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
    String tPayload;
    boolean ret = false;
    uint16_t rawData_u16;


    if(this->prevTime_u32 + this->reportCycleMSec_u32 < millis() || this->prevTime_u32 == 0)
    {      
        if(true == this->isConnected_bol)
        {
            p_trace->println(trace_INFO_MSG, "<<temt6000>> processes publish request");
            this->prevTime_u32 = millis();
            p_trace->println(trace_INFO_MSG, "<<temt6000>> reading brightness");

            this->ReadData();
            
            this->ProcessBrightness();

            if((this->level_u8 != this->lastLevel_u8) || 
                (abs(this->brightness_f32 - this->lastBrightness_f32) > MQTT_THRESHOLD))
            {
                this->lastLevel_u8 = this->level_u8;
                this->lastBrightness_f32 = this->brightness_f32;
                p_trace->print(trace_INFO_MSG, "<<temt6000>> publish brithness: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_BRIGHTNESS);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_BRIGHTNESS), 
                                        f2s(this->rawData_u16, 2), true);
                p_trace->println(trace_PURE_MSG, f2s(this->rawData_u16, 2));
                
                p_trace->print(trace_INFO_MSG, "<<temt6000>> publish level: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_BRIGHT_LEVEL);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_BRIGHT_LEVEL), 
                                        f2s((float)this->level_u8, 2), true);
                p_trace->println(trace_PURE_MSG, f2s(this->level_u8, 2));
            } 
                   
        } 
        else
        {
            p_trace->println(trace_ERROR_MSG, 
                    "<<temt6000>> connection failure in dht ProcessPublishRequests "); 
        }
    }
    return ret;  
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
/**--------------------------------------------------------------------------------------
 * @brief     This function converts a float value to a string
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     f is the float to turn into a string
 * @param     p is the precision (number of decimals)
 * @return    return a string representation of the float
*//*-----------------------------------------------------------------------------------*/
char* Temt6000::f2s(float f, int p)
{
  char * pBuff;                         // use to remember which part of the buffer to use for dtostrf
  const int iSize = 10;                 // number of buffers, one for each float before wrapping around
  static char sBuff[iSize][20];         // space for 20 characters including NULL terminator for each float
  static int iCount = 0;                // keep a tab of next place in sBuff to use
  pBuff = sBuff[iCount];                // use this buffer
  if(iCount >= iSize -1)
  {                                     // check for wrap
    iCount = 0;                         // if wrapping start again and reset
  }
  else
  {
    iCount++;                           // advance the counter
  }
  return dtostrf(f, 0, p, pBuff);       // call the library function
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
        delay(WARM_UP_TIME);
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
    // power sensor device (warm up needs to be handled in the power up function)
    this->PowerOn();
    // read the internal ADC
    this->rawData_u16 = analogRead(DEFAULT_DATAPIN);

    // power down the sensor
    this->PowerOff();
}

/**--------------------------------------------------------------------------------------
 * @brief     This function processes the brigthness value and level.
 * @author    S. Wink
 * @date      04 Jul 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Temt6000::ProcessBrightness(void) 
{
    // calculate moisture in % based on min (dry) and max (wet) range
    this->brightness_f32 = this->rawData_u16;

    // calculate brightness level for dark = 0, bright = 1
    if(this->brightness_f32 <= DARK_LEVEL)
    {
        this->level_u8 = 0U;
        if(NULL != this->brightPin_p)
        {
            this->brightPin_p->DigitalWrite(HIGH);    
        }
    }
    else
    {
        this->level_u8 = 1U;
        if(NULL != this->brightPin_p)
        {
            this->brightPin_p->DigitalWrite(LOW);    
        }
    }
}
