/*****************************************************************************************
* FILENAME :        DhtSensor.cpp
*
* DESCRIPTION :
*       DHT Sensor class
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
* AUTHOR :    Stephan Wink        START DATE :    01.10.2017
*
*****************************************************************************************/

/****************************************************************************************/
/* Include Interfaces */
#include <PubSubClient.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

#include "MqttDevice.h"
#include "DhtSensor.h"         
#include "Trace.h"
#include "PubSubClient.h"

/****************************************************************************************/
/* Local constant defines */
#define DEFAULT_DHTPIN            5u  // D1

#define TEMPERATURE_CORR_FACTOR   1.00f
#define HUMIDITY_CORR_FACTOR      1.23f
#define DHTTYPE                   DHT22 // DHT11 or DHT22

#define MICROSEC_IN_SEC           1000000l // microseconds in seconds
#define MILLISEC_IN_SEC           1000l // milliseconds in seconds

#define MQTT_PUB_TEMPERATURE      "/s/temp_hum/temp" // temperature data
#define MQTT_PUB_HUMIDITY         "/s/temp_hum/hum" // humidity data
#define MQTT_PUB_BATTERY          "/s/temp_hum/bat" // battery capacity data
#define MQTT_REPORT_INTERVAL      (30l * MILLISEC_IN_SEC) // 30 seconds between reports
/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for DhtSensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DhtSensor::DhtSensor(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0;
    this->pwrPin_p = NULL;
    this->dhtId_u8 = 0;
    dht_p = new DHT(DEFAULT_DHTPIN, DHTTYPE, 11);
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for DhtSensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace             trace object for info and error messages
 * @param     dhtPin_u8           pin identifier for dht data pin
 * @param     pwrPin_p            handle to power pin of gen tyype GpioDevice
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DhtSensor::DhtSensor(Trace *p_trace, uint8_t dhtPin_u8, 
                                            GpioDevice *pwrPin_p) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0;
    this->dhtPin_u8 = dhtPin_u8;
    this->pwrPin_p = pwrPin_p;
    this->dhtId_u8 = 0;
    this->dht_p = new DHT(this->dhtPin_u8, DHTTYPE, 11);
    this->reportCycleMSec_u32 = MQTT_REPORT_INTERVAL;
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for DhtSensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace             trace object for info and error messages
 * @param     dhtPin_u8           pin identifier for dht data pin
 * @param     pwrPin_p            handle to power pin of gen tyype GpioDevice
 * @param     reportCycleSec_u16  cycle time in seconds between reports
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DhtSensor::DhtSensor(Trace *p_trace, uint8_t dhtPin_u8, GpioDevice *pwrPin_p, 
                        uint16_t reportCycleSec_u16) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0;
    this->dhtId_u8 = 0;
    this->dhtPin_u8 = dhtPin_u8;
    this->pwrPin_p = pwrPin_p;
    this->dht_p = new DHT(this->dhtPin_u8, DHTTYPE, 11);
    this->reportCycleMSec_u32 = reportCycleSec_u16 * MILLISEC_IN_SEC;
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for DhtSensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace             trace object for info and error messages
 * @param     dhtPin_u8           pin identifier for dht data pin
 * @param     pwrPin_p            handle to power pin of gen tyype GpioDevice
 * @param     reportCycleSec_u16  cycle time in seconds between reports
 * @param     dhtId_u8            id different to 0, to allow more dht's on one device
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DhtSensor::DhtSensor(Trace *p_trace, uint8_t dhtPin_u8, GpioDevice *pwrPin_p, 
                        uint16_t reportCycleSec_u16, uint8_t dhtId_u8) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0;
    this->dhtId_u8 = dhtId_u8;
    this->dhtPin_u8 = dhtPin_u8;
    this->pwrPin_p = pwrPin_p;
    this->dht_p = new DHT(this->dhtPin_u8, DHTTYPE, 11);
    this->reportCycleMSec_u32 = reportCycleSec_u16 * MILLISEC_IN_SEC;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DhtSensor::~DhtSensor()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the DHT Sensor object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void DhtSensor::Initialize()
{
    // ensure DHT is powered down
    p_trace->println(trace_INFO_MSG, "<<dht>> initialize");
    this->TurnDHTOff();   
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
void DhtSensor::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->println(trace_INFO_MSG, "DHT Sensor connected");
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
void DhtSensor::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true != this->isConnected_bol)
    {
        p_trace->println(trace_ERROR_MSG, "<<dht>> connection failure in DHT CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool DhtSensor::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret = false;

    if(this->prevTime_u32 + this->reportCycleMSec_u32 < millis() || this->prevTime_u32 == 0)
    {      
        // the temperature and humidity publication is time interval based
        if(true == this->isConnected_bol)
        {
            p_trace->println(trace_INFO_MSG, "DHT Sensor processes publish request");
            this->prevTime_u32 = millis();
            p_trace->println(trace_INFO_MSG, "reading temperature and humidity from DHT");

            // check the internal ADC
            //out = (unsigned int) analogRead(A0);

            // checking the DHT values
            this->TurnDHTOn();
            this->humidity_f32 = dht_p->readHumidity();
            this->temperature_f32 = dht_p->readTemperature();
            TurnDHTOff();

            // apply correction factor to both measurements
            this->humidity_f32 = this->humidity_f32 * HUMIDITY_CORR_FACTOR;
            this->temperature_f32 = this->temperature_f32 * TEMPERATURE_CORR_FACTOR;
            
            // Check if any reads failed and exit
            if (isnan(this->humidity_f32) || isnan(this->temperature_f32)) 
            {
                p_trace->println(trace_ERROR_MSG, "Failed to read from DHT sensor!");
            }
            else
            {      
                p_trace->print(trace_INFO_MSG, "<<mqtt>> publish temperature: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_TEMPERATURE);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_TEMPERATURE), 
                                        f2s(this->temperature_f32, 2), true);
                p_trace->println(trace_PURE_MSG, f2s(this->temperature_f32, 2));
                
                p_trace->print(trace_INFO_MSG, "<<mqtt>> publish humidity: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_HUMIDITY);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_HUMIDITY), 
                                        f2s(this->humidity_f32, 2), true);
                p_trace->println(trace_PURE_MSG, f2s(this->humidity_f32, 2));  
            }
        } 
        else
        {
            p_trace->println(trace_ERROR_MSG, 
                                  "connection failure in dht ProcessPublishRequests "); 
        }
    }

    return ret;  
}

/****************************************************************************************/
/* Private functions: */

/****************************************************************************************/
/* Protected functions: */
/**---------------------------------------------------------------------------------------
 * @brief     This function converts a float value to a string
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     f is the float to turn into a string
 * @param     p is the precision (number of decimals)
 * @return    return a string representation of the float
*//*-----------------------------------------------------------------------------------*/
char* DhtSensor::f2s(float f, int p)
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

/**---------------------------------------------------------------------------------------
 * @brief     This function turns on the DHT power pin and waits for a dedicated time.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void DhtSensor::TurnDHTOn() 
{ 
    if(NULL != pwrPin_p)
    {
        this->pwrPin_p->DigitalWrite(HIGH);
        p_trace->println(trace_INFO_MSG, "<<dht>>dht turned on");
    }
    
    delay(500);
    // start dht
    dht_p->begin(); 
    delay(500);  
}

/**---------------------------------------------------------------------------------------
 * @brief     This function turns off the DHT power pin.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void DhtSensor::TurnDHTOff() 
{ 
    if(NULL != pwrPin_p)
    {
        this->pwrPin_p->DigitalWrite(LOW);
        p_trace->println(trace_INFO_MSG, "<<dht>>dht turned off");
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
char* DhtSensor::build_topic(const char *topic) 
{
  if(0 == this->dhtId_u8)
  {
      sprintf(buffer_ca, "std/%s%s", this->dev_p, topic);
  }
  else
  {
      sprintf(buffer_ca, "std/%s%s%d", this->dev_p, topic, this->dhtId_u8);  
  }
  return buffer_ca;
}
