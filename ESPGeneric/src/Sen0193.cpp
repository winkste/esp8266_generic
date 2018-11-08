/*****************************************************************************************
* FILENAME :        Sen0193.cpp
*
* DESCRIPTION :
*       Class file for Sen0193 capacitive moisture sensor
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
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#include "MqttDevice.h"        
#include "Trace.h"
#include "PubSubClient.h"

#include "Sen0193.h" 

/****************************************************************************************/
/* Local constant defines */
#define DEFAULT_DATAPIN           0u  // A0

#define MICROSEC_IN_SEC           1000000l // microseconds in seconds
#define MILLISEC_IN_SEC           1000l // milliseconds in seconds
#define WARM_UP_TIME              500   // wait 500 milliseconds

#define MQTT_PUB_MOISTURE         "/s/sen0193/moisture" // moisture data
#define MQTT_PUB_LEVEL            "/s/sen0193/level" // moisture level data
#define MQTT_PUB_STATUS           "/s/temp_hum/stat" // status of sensor
#define MQTT_REPORT_INTERVAL      (30l * MILLISEC_IN_SEC) // 30 seconds between reports
/*#define MQTT_PUB_PAY_STATUS_OK    "OK"
#define MQTT_PUB_PAY_STATUS_ERR   "ERR"
#define MQTT_PUB_PAY_LEVEL_LOW    "LOW"
#define MQTT_PUB_PAY_LEVEL_MED    "MED"
#define MQTT_PUB_PAY_LEVEL_HIGH   "HIGH"*/

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief       Constructor for Sen0193
 * @author      winkste
 * @date        04 Jul. 2018
 * @param[in]   p_trace     trace object for info and error messages
 * @return      n/a
*//*-----------------------------------------------------------------------------------*/
Sen0193::Sen0193(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32          = 0UL;
    this->pwrPin_p              = NULL;
    this->lowMoistPin_p         = NULL;
    this->medMoistPin_p         = NULL;
    this->highMoistPin_p        = NULL;
    this->medMoistPin_p         = NULL;
    this->moisture_f32          = 0.0F;
    this->level_u8              = 0U;
    this->level_chrp            = MQTT_PUB_PAY_LEVEL_LOW;
    this->rawData_u16           = 0U;
    this->moistureId_u8         = 0U;
    this->mode_u8               = 0U;
    this->status_chrp           = MQTT_PUB_PAY_STATUS_ERR;
    this->reportCycleMSec_u32   = MQTT_REPORT_INTERVAL;
    this->mode_u8               = 0;
}

/**---------------------------------------------------------------------------------------
 * @brief       Constructor for Sen0193
 * @author      winkste
 * @date        04 Jul. 2018
 * @param[in]   p_trace     trace object for info and error messages
 * @param[in]   pwrPin_p    handle to power pin of gen tyype GpioDevice
 * @return      n/a
*//*-----------------------------------------------------------------------------------*/
Sen0193::Sen0193(Trace *p_trace, GpioDevice *pwrPin_p) : Sen0193(p_trace)
{
    this->pwrPin_p              = pwrPin_p;
}

/**---------------------------------------------------------------------------------------
 * @brief       Constructor for Sen0193
 * @author      winkste
 * @date        04 Jul. 2018
 * @param[in]   p_trace         trace object for info and error messages
 * @param[in]   pwrPin_p        handle to power pin of gen tyype GpioDevice
 * @param[in]   lowMoistPin_p   toggle pin for low moisture detection
 * @param[in]   medMoistPin_p   toggle pin for medium moisture detection
 * @param[in]   highMoistPin_p  toggle pin for high moisture detection
 * @param[in]   mode_u8         0 = toggle only actual level, 1 = set all levels to the 
 *                              actual level
 * @return      n/a
*//*-----------------------------------------------------------------------------------*/
Sen0193::Sen0193(Trace *p_trace, GpioDevice *pwrPin_p, GpioDevice *lowMoistPin_p, 
                                GpioDevice *medMoistPin_p, GpioDevice *highMoistPin_p, 
                                uint8_t mode_u8, uint8_t moistureId_u8) : Sen0193(p_trace)
{
    this->pwrPin_p              = pwrPin_p;
    this->lowMoistPin_p         = lowMoistPin_p;
    this->medMoistPin_p         = medMoistPin_p;
    this->highMoistPin_p        = highMoistPin_p;
    this->mode_u8               = mode_u8;
    this->moistureId_u8         = moistureId_u8;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Sen0193::~Sen0193()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the DHT Sensor object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Sen0193::Initialize()
{
    p_trace->println(trace_INFO_MSG, "<<sen0193>> initialize");
    this->PowerOff();
    this->SetMoistureLevelPins();   
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
void Sen0193::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->println(trace_INFO_MSG, "<<sen0193>> connected");
    }
    else
    {
        // failure, not connected
        p_trace->println(trace_ERROR_MSG, 
                    "<<sen0193>> uninizialized MQTT client in single relay detected");
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
void Sen0193::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true != this->isConnected_bol)
    {
        p_trace->println(trace_ERROR_MSG, 
                                "<<sen0193>> connection failure in DHT CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool Sen0193::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret = false;

    if(this->prevTime_u32 + this->reportCycleMSec_u32 < millis() || this->prevTime_u32 == 0)
    {      
        // the moisture and moisture level publication is time interval based
        if(true == this->isConnected_bol)
        {
            p_trace->println(trace_INFO_MSG, "<<sen0193>> processes publish request");
            this->prevTime_u32 = millis();
            p_trace->println(trace_INFO_MSG, "<<sen0193>> reading moisture");

            this->ReadData();
            
            this->ProcessMoisture();

            this->SetMoistureLevelPins();
            
            // Check if any reads failed and exit
            if (isnan(this->moisture_f32)) 
            {
                p_trace->println(trace_ERROR_MSG, "<<sen0193>> Failed to read from sensor!");
            }
            else
            {      
                p_trace->print(trace_INFO_MSG, "<<sen0193>> publish moisture: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_MOISTURE);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_MOISTURE), 
                                        f2s(this->moisture_f32, 2), true);
                p_trace->println(trace_PURE_MSG, f2s(this->moisture_f32, 2));
                
                p_trace->print(trace_INFO_MSG, "<<sen0193>> publish level: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_LEVEL);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_LEVEL), 
                                        this->level_chrp, true);
                p_trace->println(trace_PURE_MSG, this->level_chrp);  

                p_trace->print(trace_INFO_MSG, "<<sen0193>> publish status: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_STATUS);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_STATUS), 
                                        this->status_chrp, true);
                p_trace->println(trace_PURE_MSG, this->level_chrp);
            }
        } 
        else
        {
            p_trace->println(trace_ERROR_MSG, 
                    "<<sen0193>> connection failure in dht ProcessPublishRequests "); 
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
char* Sen0193::build_topic(const char *topic) 
{
  if(0 == this->moistureId_u8)
  {
      sprintf(buffer_ca, "std/%s%s", this->dev_p, topic);
  }
  else
  {
      sprintf(buffer_ca, "std/%s%s%d", this->dev_p, topic, this->moistureId_u8);  
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
char* Sen0193::f2s(float f, int p)
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
 * @brief     This function turns on the Sen0193 power pin and waits for a dedicated 
 *              time.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Sen0193::PowerOn() 
{ 
    if(NULL != pwrPin_p)
    {
        this->pwrPin_p->DigitalWrite(HIGH);
        p_trace->println(trace_INFO_MSG, "<<dht>>dht turned on");
        delay(WARM_UP_TIME);
    } 
}

/**--------------------------------------------------------------------------------------
 * @brief     This function turns off the Sen0193 power pin.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Sen0193::PowerOff() 
{ 
    if(NULL != this->pwrPin_p)
    {
        this->pwrPin_p->DigitalWrite(LOW);
        p_trace->println(trace_INFO_MSG, "<<sen0193>> turned off");
    }
}

/**--------------------------------------------------------------------------------------
 * @brief     This function reads the internal adc channel.
 * @author    S. Wink
 * @date      04 Jul 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Sen0193::ReadData(void) 
{
    // power sensor device (warm up needs to be handled in the power up function)
    this->PowerOn();
    // read the internal ADC
    this->rawData_u16 = analogRead(DEFAULT_DATAPIN);
    // power down the sensor
    this->PowerOff();
    p_trace->print(trace_INFO_MSG, "<<sen0193>> raw data: ");
    p_trace->println(trace_PURE_MSG, this->rawData_u16);
}

/**--------------------------------------------------------------------------------------
 * @brief     This function processes the moisture value in % and level.
 * @author    S. Wink
 * @date      04 Jul 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Sen0193::ProcessMoisture(void) 
{
    if(this->rawData_u16 > this->maxValueRawData_u16c)
    {
        this->moisture_f32 = 0.0f;
        this->level_u8 = 0U;
        this->status_chrp = MQTT_PUB_PAY_STATUS_ERR;       
    }
    else if(this->rawData_u16 < this->zeroValueRawData_u16c)
    {
        this->moisture_f32 = 100.0f;
        this->level_u8 = 2U;
        this->status_chrp = MQTT_PUB_PAY_STATUS_ERR;
    }
    else
    {
        this->status_chrp = MQTT_PUB_PAY_STATUS_OK;

        
        
        // calculate moisture in % based on min (dry) and max (wet) range
        this->moisture_f32 = (this->rawData_u16 - this->zeroValueRawData_u16c);
        this->moisture_f32 = this->moisture_f32 / (this->maxValueRawData_u16c - this->zeroValueRawData_u16c);
        this->moisture_f32 = this->moisture_f32 * 100.0F;
        this->moisture_f32 = 100.0F - this->moisture_f32;

        p_trace->print(trace_INFO_MSG, "<<sen0193>> moisture processed: ");
        p_trace->println(trace_PURE_MSG, (uint16_t)(this->moisture_f32));
    
        // calculate moisture level for dry/low = 0, med wet = 1 and wet = 2
        if(this->rawData_u16 < this->highLevel_u16c)  // 500
        {
            this->level_u8 = 2U;
            this->level_chrp = MQTT_PUB_PAY_LEVEL_HIGH;
            p_trace->println(trace_INFO_MSG, "<<sen0193>> moisture level high");
        }     
        else if(this->rawData_u16 < this->medLevel_u16c)  // 650
        {
            this->level_u8 = 1U;
            this->level_chrp = MQTT_PUB_PAY_LEVEL_MED;
            p_trace->println(trace_INFO_MSG, "<<sen0193>> moisture level medium");
        }
        else
        {
            this->level_u8 = 0U;
            this->level_chrp = MQTT_PUB_PAY_LEVEL_LOW;
            p_trace->println(trace_INFO_MSG, "<<sen0193>> moisture level low");
        }    
    }
}

/**--------------------------------------------------------------------------------------
 * @brief     This function sets the moisture pins according to the calculated level.
 * @author    S. Wink
 * @date      04 Jul 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Sen0193::SetMoistureLevelPins(void) 
{
    if((NULL != this->lowMoistPin_p) &&
                (NULL != this->medMoistPin_p) &&
                (NULL != this->highMoistPin_p))
    {
        if(0U == this->mode_u8)
        {
            switch(this->level_u8)
            {
                case 0U:
                    this->lowMoistPin_p->DigitalWrite(HIGH);
                    this->medMoistPin_p->DigitalWrite(LOW);
                    this->highMoistPin_p->DigitalWrite(LOW);
                    break;
                case 1U:
                    this->lowMoistPin_p->DigitalWrite(LOW);
                    this->medMoistPin_p->DigitalWrite(HIGH);
                    this->highMoistPin_p->DigitalWrite(LOW);
                    break;
                case 2U:
                    this->lowMoistPin_p->DigitalWrite(LOW);
                    this->medMoistPin_p->DigitalWrite(LOW);
                    this->highMoistPin_p->DigitalWrite(HIGH);
                    break;
                default:
                    this->lowMoistPin_p->DigitalWrite(LOW);
                    this->medMoistPin_p->DigitalWrite(LOW);
                    this->highMoistPin_p->DigitalWrite(LOW);
                    break;
            }
        }
        else
        {
            switch(this->level_u8)
            {
                case 0U:
                    this->lowMoistPin_p->DigitalWrite(HIGH);
                    this->medMoistPin_p->DigitalWrite(LOW);
                    this->highMoistPin_p->DigitalWrite(LOW);
                    break;
                case 1U:
                    this->lowMoistPin_p->DigitalWrite(HIGH);
                    this->medMoistPin_p->DigitalWrite(HIGH);
                    this->highMoistPin_p->DigitalWrite(LOW);
                    break;
                case 2U:
                    this->lowMoistPin_p->DigitalWrite(HIGH);
                    this->medMoistPin_p->DigitalWrite(HIGH);
                    this->highMoistPin_p->DigitalWrite(HIGH);
                    break;
                default:
                    this->lowMoistPin_p->DigitalWrite(LOW);
                    this->medMoistPin_p->DigitalWrite(LOW);
                    this->highMoistPin_p->DigitalWrite(LOW);
                    break;
            }
        }
    }
}
