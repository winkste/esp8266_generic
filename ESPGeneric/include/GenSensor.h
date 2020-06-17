/*****************************************************************************************
* FILENAME :        GenSensor.h
*
* DESCRIPTION :
*       Class header for Generic Sensor
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
#ifndef GENSENSOR_H_
#define GENSENSOR_H_

/****************************************************************************************/
/* Imported header files: */
#include "MqttDevice.h"
#include "Trace.h"

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

typedef enum 
{
    GOOD_COMM = 0,
    FEEDBACK_ERROR,
    NO_COMMUNICATION,
    SIZE_COMM_EVENTS
}comEvent_t;

/****************************************************************************************/
/* Class definition: */
class GenSensor : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */
        
        /********************************************************************************/
        /* Public function definitions: */
        GenSensor(Trace *p_trace);
        GenSensor(Trace *p_trace, bool mqttObserverActive_bol);
        GenSensor(Trace *p_trace, bool mqttObserverActive_bol,
                    uint16_t rebootThresholdOnComError_u16);
        
        // virtual functions, implementation in derived classes
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        virtual
        ~GenSensor();
    private:
        /********************************************************************************/
        /* Private data definitions */
        uint32_t    publishData_u32;
        char        buffer_ca[100];
        char        mqttPayload[20];
        uint32_t    prevTime_u32 = 0;
        uint32_t    reportCycleMSec_u32;

        uint16_t    healthTic_u16;

        uint16_t    feedbackHealthTic_u16;
        bool        mqttObserverActive_bol;
        uint16_t    mqttComErrorCounter_u16;
        uint16_t    mqttGoodCommsCounter_u16;
        bool        waitForFeedback_bol;
        uint16_t    rebootThresholdOnComError_u16;

        /********************************************************************************/
        /* Private function definitions: */
        char* build_topic(const char *topic);
        void ObserveCommunication(comEvent_t comEvent_en);
        void ForceWatchDogReset(void);
        
    protected:
        /********************************************************************************/
        /* Protected data definitions */
        
        /********************************************************************************/
        /* Protected function definitions: */
};

#endif /* GENSENSOR_H_ */
