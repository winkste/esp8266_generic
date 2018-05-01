/*****************************************************************************************
* FILENAME :        Bme280Sensor.cpp
*
* DESCRIPTION :
*       BME280 Sensor class
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
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "MqttDevice.h"
#include "Bme280Sensor.h"         
#include "Trace.h"
#include "PubSubClient.h"

/****************************************************************************************/
/* Local constant defines */
#define MAX_PUBS_TILL_POWER_SAVE  1u   // send two times data than go to sleep
#define MICROSEC_IN_SEC           1000000l // microseconds in seconds
#define POWER_SAVE_TIME           900l * MICROSEC_IN_SEC  // = 900 secs = 15mins power save

#define MQTT_SUB_BME_CMD          "/r/bme/cmd" // command message for dht
#define MQTT_PAYLOAD_CMD_ON       "ON"
#define MQTT_PAYLOAD_CMD_OFF      "OFF"

#define MQTT_PUB_TEMPERATURE      "/s/bme/temp" // temperature data
#define MQTT_PUB_HUMIDITY         "/s/bme/hum" // humidity data
#define MQTT_PUB_ALTITUDE         "/s/bme/alt" // altitude data
#define MQTT_PUB_PRESSURE         "/s/bme/pres" // pressure data
#define MQTT_PUB_BATTERY          "/s/bme/bat" // battery capacity data
#define MQTT_REPORT_INTERVAL      30000l //(ms) - 30 seconds between reports

#define SEALEVELPRESSURE_HPA      1013.25f

#define WEMOS_PIN_D4              2u  // D4
#define BME_PWR                   WEMOS_PIN_D4

#define HUMIDITY_CORR_FACTOR      1.0f
#define TEMPERATURE_CORR_FACTOR   1.0f
#define PRESSURE_CORR_FACTOR      1.0f
#define ALTITUDE_CORR_FACTOR      1.0f

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for Bme280Sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Bme280Sensor::Bme280Sensor(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0;
    this->publications_u16 = 0;
    this->powerSaveMode_bol = false;
    bme_p = new Adafruit_BME280();

}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for Bme280Sensor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace             trace object for info and error messages
 * @param     powerSaveMode_bol   true = power save mode active, else false
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Bme280Sensor::Bme280Sensor(Trace *p_trace, bool powerSaveMode_bol) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0;
    this->publications_u16 = 0;
    this->powerSaveMode_bol = powerSaveMode_bol;
    bme_p = new Adafruit_BME280();

}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
Bme280Sensor::~Bme280Sensor()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the DHT Sensor object
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Bme280Sensor::Initialize()
{
    p_trace->println(trace_INFO_MSG, "BME sensor initialized");
    
    // power up the DHT
    p_trace->println(trace_INFO_MSG, "Power up BME");
    pinMode(BME_PWR, OUTPUT);
    digitalWrite(BME_PWR, LOW);
    delay(500);   
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
void Bme280Sensor::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->println(trace_INFO_MSG, "BME Sensor connected");
        // ... and resubscribe
        // dht sensor
        client_p->subscribe(build_topic(MQTT_SUB_BME_CMD));  
        client_p->loop();
        p_trace->print(trace_INFO_MSG, "<<mqtt>> subscribed 1: ");
        p_trace->println(trace_PURE_MSG, MQTT_SUB_BME_CMD);
    }
    else
    {
        // failure, not connected
        p_trace->println(trace_ERROR_MSG, 
                                "uninizialized MQTT client in bme sensor detected");
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
void Bme280Sensor::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true == this->isConnected_bol)
    {
        // received bme command
        if (String(build_topic(MQTT_SUB_BME_CMD)).equals(p_topic)) 
        {
            p_trace->println(trace_INFO_MSG, "BME Sensor mqtt callback");
            p_trace->println(trace_INFO_MSG, p_topic);
            p_trace->println(trace_INFO_MSG, p_payload);
            // test if the payload is equal to "ON" or "OFF"
            if(0 == p_payload.indexOf(String(MQTT_PAYLOAD_CMD_ON))) 
            {
                this->powerSaveMode_bol = false; 
            }
            else if(0 == p_payload.indexOf(String(MQTT_PAYLOAD_CMD_OFF)))
            {
                this->powerSaveMode_bol = true;
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
        p_trace->println(trace_ERROR_MSG, "connection failure in BME CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool Bme280Sensor::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret = false;

    if(this->prevTime_u32 + MQTT_REPORT_INTERVAL < millis() || this->prevTime_u32 == 0)
    {      
        // the sensor data publication is time interval based
        if(true == this->isConnected_bol)
        {
            p_trace->println(trace_INFO_MSG, "bme Sensor processes publish request");
            this->prevTime_u32 = millis();
            p_trace->println(trace_INFO_MSG, "reading sensor data from BME280");

            // check the internal ADC
            //out = (unsigned int) analogRead(A0);

            // checking the bme values
            this->TurnBmeOn();
            this->humidity_f32      = bme_p->readHumidity();
            this->temperature_f32   = bme_p->readTemperature();
            this->pressure_f32      = bme_p->readPressure() / 100.0F;
            this->altitude_f32      = bme_p->readAltitude(SEALEVELPRESSURE_HPA);
            this->TurnBmeOff();

            // apply correction factor to measurements
            this->humidity_f32      = this->humidity_f32 * HUMIDITY_CORR_FACTOR;
            this->temperature_f32   = this->temperature_f32 * TEMPERATURE_CORR_FACTOR;
            this->pressure_f32      = this->pressure_f32 * PRESSURE_CORR_FACTOR;
            this->altitude_f32      = this->altitude_f32 * ALTITUDE_CORR_FACTOR;
            
            // Check if any reads failed and exit
            if (    (isnan(this->humidity_f32)) 
                ||  (isnan(this->temperature_f32))
                ||  (isnan(this->pressure_f32))
                ||  (isnan(this->altitude_f32))
                ) 
            {
                p_trace->println(trace_ERROR_MSG, "Failed to read from BME sensor!");
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

                p_trace->print(trace_INFO_MSG, "<<mqtt>> publish pressure: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_PRESSURE);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_PRESSURE), 
                                        f2s(this->pressure_f32, 2), true);
                p_trace->println(trace_PURE_MSG, f2s(this->pressure_f32, 2)); 

                p_trace->print(trace_INFO_MSG, "<<mqtt>> publish altitude: ");
                p_trace->print(trace_PURE_MSG, MQTT_PUB_ALTITUDE);
                p_trace->print(trace_PURE_MSG, "  :  ");
                ret = client->publish(build_topic(MQTT_PUB_ALTITUDE), 
                                        f2s(this->altitude_f32, 2), true);
                p_trace->println(trace_PURE_MSG, f2s(this->altitude_f32, 2)); 
                publications_u16++;
            }
            if(MAX_PUBS_TILL_POWER_SAVE <= this->publications_u16)
            {
                this->publications_u16 = 0;
                if(true == this->powerSaveMode_bol)
                {
                    p_trace->println(trace_INFO_MSG, "GoTo Power Save Mode");
                    this->TurnBmeOff();
                    // power save time in seconds
                    ESP.deepSleep(POWER_SAVE_TIME); 
                    delay(100);
                }
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
char* Bme280Sensor::f2s(float f, int p)
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
 * @brief     This function turns on the Bme power pin and waits for a dedicated time.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Bme280Sensor::TurnBmeOn() 
{ 
  digitalWrite(BME_PWR, HIGH);
  delay(500);
  
  // start dht
  bme_p->begin(); 

  delay(500);
}

/**---------------------------------------------------------------------------------------
 * @brief     This function turns off the Bme power pin.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void Bme280Sensor::TurnBmeOff() 
{ 
  digitalWrite(BME_PWR, LOW); 
}

/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     topic       pointer to topic string
 * @return    combined topic as char pointer, it uses buffer_stca to store the topic
*//*-----------------------------------------------------------------------------------*/
char* Bme280Sensor::build_topic(const char *topic) 
{
  sprintf(buffer_ca, "std/%s%s", this->dev_p, topic);
  return buffer_ca;
}
