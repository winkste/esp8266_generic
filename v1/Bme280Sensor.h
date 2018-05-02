/*****************************************************************************************
* FILENAME :        Bme280Sensor.h
*
* DESCRIPTION :
*       Class header for BME280 sensor 
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
#ifndef BME280SENSOR_H_
#define BME280SENSOR_H_

/****************************************************************************************/
/* Imported header files: */
#include <ESP8266WiFi.h>         
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

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

/****************************************************************************************/
/* Class definition: */
class Bme280Sensor : public MqttDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */
        Adafruit_BME280     *bme_p;
        float               humidity_f32        = 0.0;
        float               temperature_f32     = 0.0; 
        float               altitude_f32        = 40.00;
        float               pressure_f32        = 1000.00;
        uint32_t            prevTime_u32        = 0;
        uint16_t            publications_u16    = 0;

        /********************************************************************************/
        /* Public function definitions: */
        /*Bme280Sensor(Trace *p_trace);
        Bme280Sensor(Trace *p_trace, bool powerSaveMode_bol);*/
        Bme280Sensor(Trace *p_trace, bool powerSaveMode_bol, GpioDevice  *bmePwr_p, GpioDevice  *bmeStat_p);
        // virtual functions, implementation in derived classes
        bool ProcessPublishRequests(PubSubClient *client);
        void CallbackMqtt(PubSubClient *client, char* p_topic, String p_payload);
        void Initialize();
        void Reconnect(PubSubClient *client_p, const char *dev_p);
        virtual
        ~Bme280Sensor();
    private:
        /********************************************************************************/
        /* Private data definitions */
        uint32_t    publishData_u32;
        char        buffer_ca[100];
        bool        powerSaveMode_bol;
        GpioDevice  *bmePwr_p;
        GpioDevice  *bmeStat_p;

        /********************************************************************************/
        /* Private function definitions: */
        char* build_topic(const char *topic);
        
    protected:
        /********************************************************************************/
        /* Protected data definitions */
        
        /********************************************************************************/
        /* Protected function definitions: */
        char *f2s(float f, int p);
        void TurnBmeOn();
        void TurnBmeOff();
        void TurnStatusOn();
        void TurnStatusOff(); 
};

#endif /* BME280SENSOR_H_ */
