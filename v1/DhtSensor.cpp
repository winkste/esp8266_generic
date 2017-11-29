/*****************************************************************************************
* FILENAME :        DhtSensor.cpp
*
* DESCRIPTION :
*       DHT Sensor class
*
* PUBLIC FUNCTIONS :
*
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
#include <PubSubClient.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

#include "MqttDevice.h"
#include "DhtSensor.h"         
#include "Trace.h"
#include "PubSubClient.h"

/****************************************************************************************/
/* Local constant defines */
#define DHTPIN                    5  // D1
#define DHT_PWR                   4  // D2

#define TEMPERATURE_CORR_FACTOR   (1.00)
#define HUMIDITY_CORR_FACTOR      (1.23)
#define DHTTYPE                   DHT22 // DHT11 or DHT22

#define POWER_SAVE                1   // 1 = activated
#define POWER_SAVE_TIME           300  // = 300 seconds power save time
#define MAX_PUBS_TILL_POWER_SAVE  1   // send two times data than go to sleep

#define MQTT_PUB_TEMPERATURE      "/temp_hum/temp" // temperature data
#define MQTT_PUB_HUMIDITY         "/temp_hum/hum" // humidity data
#define MQTT_PUB_BATTERY          "/temp_hum/bat" // battery capacity data
#define MQTT_REPORT_INTERVAL      5000 //(ms) - 5 seconds between reports
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
    this->publications_u16 = 0;
    dht_p = new DHT(DHTPIN, DHTTYPE, 11);

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
    p_trace->println(trace_INFO_MSG, "DHT sensor initialized");
    
    // power up the DHT
    p_trace->println(trace_INFO_MSG, "Power up DHT");
    pinMode(DHT_PWR, OUTPUT);
    digitalWrite(DHT_PWR, LOW);
    delay(500);   
    // start dht
    this->isInitialized_bol = true;
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to initialize the MQTT interface for this sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void DhtSensor::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
    }
    else
    {
        this->isConnected_bol = true;
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
    p_trace->print(trace_ERROR_MSG, "DHT Sensor mqtt callback, unexpected message: ");
    p_trace->print(trace_PURE_MSG, p_topic);
    p_trace->print(trace_PURE_MSG, " : ");
    p_trace->println(trace_PURE_MSG, p_payload);
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

    if(this->prevTime_u32 + MQTT_REPORT_INTERVAL < millis() || this->prevTime_u32 == 0)
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
                p_trace->print(trace_INFO_MSG, "[mqtt] publish temperature: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_TEMPERATURE);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_TEMPERATURE), f2s(this->temperature_f32, 2), true);
                p_trace->println(trace_PURE_MSG, f2s(this->temperature_f32, 2));
                
                p_trace->print(trace_INFO_MSG, "[mqtt] publish humidity: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_HUMIDITY);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_HUMIDITY), f2s(this->humidity_f32, 2), true);
                p_trace->println(trace_PURE_MSG, f2s(this->humidity_f32, 2));  
                publications_u16++;
            }
            if(MAX_PUBS_TILL_POWER_SAVE <= this->publications_u16)
            {
                this->publications_u16 = 0;
                #ifdef POWER_SAVE
                p_trace->println(trace_INFO_MSG, "GoTo Power Save Mode");
                TurnDHTOff();
                // power save time in seconds
                ESP.deepSleep(POWER_SAVE_TIME * 1000000); 
                delay(100);
            #endif
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
  digitalWrite(DHT_PWR, HIGH);
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
  digitalWrite(DHT_PWR, LOW); 
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
  sprintf(buffer_ca, "%s%s", this->dev_p, topic);
  return buffer_ca;
}