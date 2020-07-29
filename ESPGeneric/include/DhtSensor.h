/*****************************************************************************************
* FILENAME :        DhtSensor.h
*
* DESCRIPTION :
*       Class header for DHT Sensor
*
* NOTES :
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
*****************************************************************************************/
#ifndef DHTSENSOR_H_
#define DHTSENSOR_H_

/****************************************************************************************/
/* Imported header files: */
#include <ESP8266WiFi.h>         
#include <PubSubClient.h>
#include <DHT.h>

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"
#include "GpioDevice.h"

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */
typedef enum state_tag
{
    DHTSENSOR_OFF              = 0,
    DHTSENSOR_MEAS_REQ,
    DHTSENSOR_POWER_STARTED,
    DHTSENSOR_DRIVER_STARTED,
    DHTSENSOR_MEAS_COMPLETED,
    DHTSENSOR_MEAS_PUBLISHED,
    DHTSENSOR_UNKNOWN_STATE
}state_t;

/****************************************************************************************/
/* Class definition: */
class DhtSensor : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */
        
        /********************************************************************************/
        /* Public function definitions: */
        DhtSensor(Trace *p_trace);
        DhtSensor(Trace *p_trace, uint8_t dhtPin_u8, GpioDevice *pwrPin_p);
        DhtSensor(Trace *p_trace, uint8_t dhtPin_u8, GpioDevice *pwrPin_p, 
                      uint16_t reportCycleSec_u16);
        DhtSensor(Trace *p_trace, uint8_t dhtPin_u8, GpioDevice *pwrPin_p, 
                        uint16_t reportCycleSec_u16, uint8_t dhtId_u8);
        // virtual functions, implementation in derived classes
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        virtual
        ~DhtSensor();
    private:
        /********************************************************************************/
        /* Private data definitions */
        uint32_t        publishData_u32;
        char            buffer_ca[100];
        uint8_t         dhtPin_u8;
        GpioDevice      *pwrPin_p;
        DHT             *dht_p; 
        float           humidity_f32 = 0.0;
        float           temperature_f32 = 0.0; 
        uint32_t        prevTime_u32 = 0;
        uint32_t        reportCycleMSec_u32;
        uint8_t         dhtId_u8;

        state_t         state_en = DHTSENSOR_OFF;
        const uint32_t  STATE_LOOP_CYCLE = 2000;
        uint32_t        lastReportTime_u32 = 0;

        uint8_t         readRetries_u8 = 0U;
        const uint8_t   MAX_READ_RETRIES = 3U;

        /********************************************************************************/
        /* Private function definitions: */
        char* build_topic(const char *topic);
        
    protected:
        /********************************************************************************/
        /* Protected data definitions */
        
        /********************************************************************************/
        /* Protected function definitions: */
        char *f2s(float f, int p);
        void TurnDHTOn(void);
        void TurnDHTOff(void);
        void CheckForMeasRequest(void);
        void StartDhtSensorDriver(void);
        void ReadDataFromSensor(void);
        boolean PublishData(PubSubClient *client);
        boolean ProcessSensorStateMachine(PubSubClient *client);

};

#endif /* DHTSENSOR_H_ */
