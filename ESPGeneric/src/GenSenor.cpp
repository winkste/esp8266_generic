/*****************************************************************************************
* FILENAME :        GenSensor.cpp
*
* DESCRIPTION :
*       Generic Sensor class
*
* PUBLIC FUNCTIONS :
*
* Copyright (c) [2020] [Stephan Wink]
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
* AUTHOR :    Stephan Wink        START DATE :    03.01.2020
*
*****************************************************************************************/

/****************************************************************************************/
/* Include Interfaces */
#include "GenSensor.h"

#include "MqttDevice.h"         
#include "Trace.h"
#include "Utils.h"

/****************************************************************************************/
/* Local constant defines */

#define MICROSEC_IN_SEC             1000000l // microseconds in seconds
#define MILLISEC_IN_SEC             1000l // milliseconds in seconds

#define MQTT_PUB_HEALTH_TIC         "/s/health/tic"   // health counter
#define MQTT_SUB_HEALTH_TIC         MQTT_PUB_HEALTH_TIC
#define MQTT_REPORT_INTERVAL        (30l * MILLISEC_IN_SEC) // 30 seconds between reports

#define SOFTWARE_WDOG_TIMEOUT       (5l * MILLISEC_IN_SEC) // 5 seconds before reset  
#define MAX_MQTT_ERRORS             3
#define BAD_COMMS_ERROR_RESET       3
/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for GenSensor
 * @author    winkste
 * @date      02 Jan. 2020
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
GenSensor::GenSensor(Trace *p_trace) : MqttDevice(p_trace)
{
    this->prevTime_u32 = 0;
    this->healthTic_u16 = 0;
    this->reportCycleMSec_u32 = MQTT_REPORT_INTERVAL;

    this->mqttObserverActive_bol = true;
    this->waitForFeedback_bol = false;

    this->feedbackHealthTic_u16 = 0U;
    this->mqttObserverActive_bol = false;
    this->mqttComErrorCounter_u16 = 0U;
    this->mqttGoodCommsCounter_u16 = 0U;
    this->waitForFeedback_bol = false;
    this->rebootThresholdOnComError_u16 = BAD_COMMS_ERROR_RESET;
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for GenSensor
 * @author    winkste
 * @date      02 Jan. 2020
 * @param     p_trace     trace object for info and error messages
 * @param     mqttObserverActive_bol    observer for mqtt communication on/off
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
GenSensor::GenSensor(Trace *p_trace, bool mqttObserverActive_bol) : GenSensor(p_trace)
{
    this->mqttObserverActive_bol = mqttObserverActive_bol;
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for GenSensor
 * @author    winkste
 * @date      02 Jan. 2020
 * @param     p_trace     trace object for info and error messages
 * @param     mqttObserverActive_bol    observer for mqtt communication on/off
 * @param     rebootThresholdOnComError_u16 maximum errors in a row until reboot
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
GenSensor::GenSensor(Trace *p_trace, bool mqttObserverActive_bol,
                        uint16_t rebootThresholdOnComError_u16)
                    :GenSensor(p_trace, mqttObserverActive_bol)
{
    this->rebootThresholdOnComError_u16 = rebootThresholdOnComError_u16;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      02 Jan. 2020
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
GenSensor::~GenSensor()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the Generic Sensor object
 * @author    winkste
 * @date      02 Jan. 2020
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void GenSensor::Initialize()
{
    // ensure DHT is powered down
    p_trace->println(trace_INFO_MSG, "<<genSensor>> initialize");
    this->healthTic_u16 = 0;  
    this->isInitialized_bol = true;

    this->feedbackHealthTic_u16 = 0U;
    this->mqttComErrorCounter_u16 = 0U;
    this->mqttGoodCommsCounter_u16 = 0U;
    this->waitForFeedback_bol = false;

    ESP.wdtEnable(SOFTWARE_WDOG_TIMEOUT);
    
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to initialize the MQTT interface for this sensor
 * @author    winkste
 * @date      02 Jan. 2020
 * @param     client_p  MQTT object for message transfer
 * @param     dev_p     string identifier of the MQTT device id
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void GenSensor::Reconnect(PubSubClient *client_p, const char *dev_p)
{
    if(NULL != client_p)
    {
        this->dev_p = dev_p;
        this->isConnected_bol = true;
        p_trace->println(trace_INFO_MSG, "<<genSensor>> connected");

        // ... and resubscribe
        // same helth message to see that we have a loop connection to the broker
        client_p->subscribe(build_topic(MQTT_SUB_HEALTH_TIC));  
        client_p->loop();
        p_trace->print(trace_INFO_MSG, "<<genSensor>> subscribed 1: ");
        p_trace->println(trace_PURE_MSG, build_topic(MQTT_SUB_HEALTH_TIC));
    }
    else
    {
        // failure, not connected
        p_trace->println(trace_ERROR_MSG, 
                                "<<genSensor>> uninizialized MQTT client detected");
        this->isConnected_bol = false;
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Callback function to process subscribed MQTT publication
 * @author    winkste
 * @date      02 Jan. 2020
 * @param     client     mqtt client object
 * @param     p_topic    received topic
 * @param     p_payload  attached payload message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void GenSensor::CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload)
{
    if(true == this->isConnected_bol)
    {
        // received own published health indicator
        if (String(build_topic(MQTT_SUB_HEALTH_TIC)).equals(p_topic)) 
        {
            this->waitForFeedback_bol = false;
            p_trace->print(trace_INFO_MSG, "<<genSensor>>mqtt callback: ");
            p_trace->print(trace_PURE_MSG, p_topic);
            p_trace->print(trace_PURE_MSG, " : ");
            p_trace->println(trace_PURE_MSG, p_payload);

            feedbackHealthTic_u16 = (uint16_t)p_payload.toInt();
            if((this->healthTic_u16 - 1U) == this->feedbackHealthTic_u16)
            {
                p_trace->println(trace_INFO_MSG, "<<genSensor>>heatbeat match");
                ObserveCommunication(GOOD_COMM);
            }
            else
            {
                p_trace->println(trace_WARN_MSG, "<<genSensor>>heatbeat missmatch");
                ObserveCommunication(FEEDBACK_ERROR);
            }
        }
    }
    else
    {
        p_trace->println(trace_ERROR_MSG, 
                            "<<genSensor>> connection failure in CallbackMqtt "); 
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Sending generated publications
 * @author    winkste
 * @date      02 Jan. 2020
 * @param     client     mqtt client object
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
bool GenSensor::ProcessPublishRequests(PubSubClient *client)
{
    String tPayload;
    boolean ret = false;

    // this is the loop function of every mqtt based sensor, here we are feeding the 
    // standard software watchdog

    ESP.wdtFeed();

    if(this->prevTime_u32 + this->reportCycleMSec_u32 < millis() || this->prevTime_u32 == 0)
    {     
        // the health tic publication is time interval based
        if(true == this->isConnected_bol)
        {
            if(true == this->waitForFeedback_bol)
            {
                // no feedback from last sent received, no communication?
                ObserveCommunication(NO_COMMUNICATION);
            }
            p_trace->println(trace_INFO_MSG, "<<genSensor>> processes publish request");
            this->prevTime_u32 = millis();

            p_trace->print(trace_INFO_MSG, "<<genSensor>> publish health tic: ");
            p_trace->print(trace_PURE_MSG, MQTT_PUB_HEALTH_TIC);
            p_trace->print(trace_PURE_MSG, "  :  ");
            ret = client->publish(build_topic(MQTT_PUB_HEALTH_TIC), 
                                    Utils::IntegerToDecString(this->healthTic_u16, 
                                                    &this->mqttPayload[0]), false);
            p_trace->println(trace_PURE_MSG, &this->mqttPayload[0]);
            this->healthTic_u16++;
            this->waitForFeedback_bol = true;
        } 
        else
        {
            p_trace->println(trace_ERROR_MSG, 
                "<<genSensor>> connection failure in ProcessPublishRequests "); 
        }
    }

    return ret;  
}

/****************************************************************************************/
/* Private functions: */

/**---------------------------------------------------------------------------------------
 * @brief     This function checks the communication status if activated
 * @author    winkste
 * @date      03 Jun. 2020
 * @param     comEvent_en       communication error event
*//*-----------------------------------------------------------------------------------*/
void GenSensor::ObserveCommunication(comEvent_t comEvent_en)
{
    if(this->mqttObserverActive_bol)
    {
        switch(comEvent_en)
        {
            case GOOD_COMM:
                this->mqttComErrorCounter_u16++;
                if(BAD_COMMS_ERROR_RESET < this->mqttGoodCommsCounter_u16)
                {
                    this->mqttComErrorCounter_u16 = 0U;
                    this->mqttGoodCommsCounter_u16 = 0U;

                    p_trace->println(trace_INFO_MSG, 
                    "<<genSensor>> communication counter resetted"); 
                }
            break;
            case FEEDBACK_ERROR:
            case NO_COMMUNICATION:
                p_trace->print(trace_ERROR_MSG, 
                    "<<genSensor>> mqtt observer detects error: ");
                p_trace->println(trace_PURE_MSG, (uint16_t)comEvent_en);

                this->mqttComErrorCounter_u16++;
                this->mqttGoodCommsCounter_u16 = 0U;
                if(MAX_MQTT_ERRORS < this->mqttComErrorCounter_u16)
                {
                    ForceWatchDogReset();
                }
            break;
            default:
            break;
        }
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     This function triggers the software watchdog to reset
 * @author    winkste
 * @date      03 Jun. 2020
*//*-----------------------------------------------------------------------------------*/
void GenSensor::ForceWatchDogReset(void)
{
    p_trace->println(trace_ERROR_MSG, 
    "<<genSensor>> jump to endless loop to force a watchdog reset");
    // maximum error counter reached, time for a reset
    for(;;);    
}


/****************************************************************************************/
/* Protected functions: */

/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      02 Jan. 2020
 * @param     topic       pointer to topic string
 * @return    combined topic as char pointer, it uses buffer_stca to store the topic
*//*-----------------------------------------------------------------------------------*/
char* GenSensor::build_topic(const char *topic) 
{
  sprintf(buffer_ca, "std/%s%s", this->dev_p, topic);

  return buffer_ca;
}
