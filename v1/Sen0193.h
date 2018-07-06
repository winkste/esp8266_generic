/*****************************************************************************************
* FILENAME :        Sen0193.h
*
* DESCRIPTION :
*       Class header for Sen0193 capacitive moisture sensor
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
#ifndef SEN0193_H_
#define SEN0193_H_

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

/****************************************************************************************/
/* Class definition: */
class Sen0193 : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */
        
        /********************************************************************************/
        /* Public function definitions: */
        Sen0193(Trace *p_trace);
        Sen0193(Trace *p_trace, GpioDevice *pwrPin_p);
        Sen0193(Trace *p_trace, GpioDevice *pwrPin_p, GpioDevice *lowMoistPin_p, 
                                GpioDevice *medMoistPin_p, GpioDevice *highMoistPin_p, 
                                uint8_t mode_u8, uint8_t moistureId_u8);

        // virtual functions, implementation in derived classes
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        virtual
        ~Sen0193();
    private:
        /********************************************************************************/
        /* Private data definitions */
        uint32_t    publishData_u32;
        char        buffer_ca[100];
        GpioDevice *lowMoistPin_p;
        GpioDevice *medMoistPin_p; 
        GpioDevice *highMoistPin_p;
        GpioDevice *pwrPin_p;
        float       moisture_f32; 
        uint8_t     level_u8;
        String      level_str;
        uint16_t    rawData_u16;
        uint32_t    prevTime_u32;
        uint32_t    reportCycleMSec_u32;
        uint8_t     moistureId_u8;
        uint8_t     mode_u8;
        String      status_str;
        const uint16_t zeroValueRawData_u16c    = 450;
        const uint16_t maxValueRawData_u16c     = 860;
        const uint16_t lowLevel_u16c            = 650;
        const uint16_t medLevel_u16c            = 500;
        const uint16_t highLevel_u16c           = 300;

        /********************************************************************************/
        /* Private function definitions: */
        char* build_topic(const char *topic);
        
    protected:
        /********************************************************************************/
        /* Protected data definitions */
        
        /********************************************************************************/
        /* Protected function definitions: */
        char *f2s(float f, int p);
        void PowerOn();
        void PowerOff();
        void ReadData();
        void ProcessMoisture(void);
        void SetMoistureLevelPins();


};
/****************************************************************************************/
#endif /* SEN0193_H_ */
