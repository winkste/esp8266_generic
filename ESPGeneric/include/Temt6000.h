/*****************************************************************************************
* FILENAME :        Temt6000.h
*
* DESCRIPTION :
*       Class header for Temt6000 brightness sensor
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
#ifndef TEMT6000_H_
#define TEMT6000_H_

/****************************************************************************************/
/* Imported header files: */

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"
#include "GpioDevice.h"

#include <ESP8266WiFi.h>         
#include <PubSubClient.h>

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */
typedef enum temt6000State_tag
{
    TEMT6000_OFF              = 0,
    TEMT6000_MEAS_REQ,
    TEMT6000_POWER_STARTED,
    TEMT6000_SAMPLE_COMPLETED,
    TEMT6000_MEAS_COMPLETED,
    TEMT6000_MEAS_PUBLISHED,
    TEMT6000_UNKNOWN_STATE
}temt6000State_t;

/****************************************************************************************/
/* Class definition: */
class Temt6000 : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */
        
        /********************************************************************************/
        /* Public function definitions: */
        Temt6000(Trace *p_trace);
        Temt6000(Trace *p_trace, GpioDevice *pwrPin_p);
        Temt6000(Trace *p_trace, GpioDevice *pwrPin_p, GpioDevice *brightPin_p,
                                uint8_t brightId_u8);

        // virtual functions, implementation in derived classes
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        virtual
        ~Temt6000();
    private:
        /********************************************************************************/
        /* Private data definitions */
        uint32_t            publishData_u32;
        char                buffer_ca[100];
        GpioDevice          *brightPin_p;
        GpioDevice          *pwrPin_p;
        uint8_t             level_u8;
        const char          *level_chrp;
        uint8_t             lastLevel_u8;
        float               lastBrightness_f32;
        uint16_t            rawData_u16;
        float               brightness_f32;
        uint32_t            prevTime_u32;
        uint32_t            reportCycleMSec_u32;
        uint8_t             brightId_u8;
        const char          *MQTT_PUB_PAY_LEVEL_DARK    = "DARK";
        const char          *MQTT_PUB_PAY_LEVEL_BRIGHT  = "BRIGHT";

        temt6000State_t     state_en = TEMT6000_OFF;
        const uint32_t      STATE_LOOP_CYCLE = 500;
        uint32_t            lastReportTime_u32 = 0;

        uint32_t            avgData_32;
        uint16_t            avgCnt_u16;

        /********************************************************************************/
        /* Private function definitions: */
        char* build_topic(const char *topic);
        
    protected:
        /********************************************************************************/
        /* Protected data definitions */
        
        /********************************************************************************/
        /* Protected function definitions: */
        void CheckForMeasRequest(void);
        void PowerOn();
        void PowerOff();
        void ReadData();
        void ProcessBrightness(void);
        boolean PublishData(PubSubClient *client);
        boolean ProcessSensorStateMachine(PubSubClient *client);
};
/****************************************************************************************/
#endif /* TEMT6000_H_ */

