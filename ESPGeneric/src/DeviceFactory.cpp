/*****************************************************************************************
* FILENAME :        DeviceFactory.cpp
*
* DESCRIPTION :
*       Device generator class
*       This class has got the information of the different devices and generates
*       them on request. The user has to specify which type of device he needs.
*
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

#include "DeviceFactory.h"

#include "MqttDevice.h"
#include "GpioDevice.h"
#include "Trace.h"
#include "PubSubClient.h"
#include <LinkedList.h>

#include "DhtSensor.h"
#include "SingleRelay.h"
#include "SonoffBasic.h"
#include "Pir.h"
#include "McpGpio.h"
#include "EspGpio.h"
#include "Bme280Sensor.h"
#include "PowerSave.h"
#include "Sen0193.h"
#include "Temt6000.h"
#include "DimLight.h"
#include "NeoPix.h"
#include "GenSensor.h"

/****************************************************************************************/
/* Local constant defines */
#define CAPABILITY_0x40                 0x40u
#define CAPABILITY_0x20                 0x20u
#define CAPABILITY_TEST_DEVICE          0x15u
#define CAPABILITY_MULTI_SENSE_RELAY    0x14u
#define CAPABILITY_3D_PRINTER           0x13u
#define CAPABILITY_NEOPIXELS            0x12u
#define CAPABILITY_H801                 0x11u
#define CAPABILITY_DIM_LIGHT            0x10u
#define CAPABILITY_MULTI_SENSE          0x0Fu
#define CAPABILITY_MOISTURE_ONLY        0x0Eu
#define CAPABILITY_SONOFF_PIR           0x0Du
#define CAPABILITY_EIGHT_RELAY_ESP      0x0Cu
#define CAPABILITY_DOUBLE_DHT           0x0Bu
#define CAPABILITY_BME_SENSOR           0x0Au
#define CAPABILITY_FOUR_RELAY_MCP       0x09u
#define CAPABILITY_FOUR_RELAY           0x08u
#define CAPABILITY_PIR_RELAY            0x05u
#define CAPABILITY_DHT_SENSOR_BAT       0x04u
#define CAPABILITY_PIR                  0x03u
#define CAPABILITY_SONOFF_BASIC         0x02u
#define CAPABILITY_DHT_SENSOR           0x01u
#define CAPABILITY_SINGLE_RELAY         0x00u

#define WEMOS_PIN_D0                    16u // D0
#define WEMOS_PIN_D1                    5u  // D1
#define WEMOS_PIN_D2                    4u  // D2
#define WEMOS_PIN_D3                    0u  // D3
#define WEMOS_PIN_D4                    2u  // D4
#define WEMOS_PIN_D5                    14u // D5
#define WEMOS_PIN_D6                    12u // D6
#define WEMOS_PIN_D7                    13u // D7
#define WEMOS_PIN_D8                    15u // D8

#define PRT_3D_CTRL_REL_1               WEMOS_PIN_D1
#define PRT_3D_CTRL_MQTT_REL_1          "relay_one"
#define PRT_3D_CTRL_REL_2               WEMOS_PIN_D2
#define PRT_3D_CTRL_MQTT_REL_2          "relay_two"
#define PRT_3D_CTRL_DIM_1               WEMOS_PIN_D3
#define PRT_3D_CTRL_MQTT_DIM_1          "light_one"
#define PRT_3D_CTRL_DIM_2               WEMOS_PIN_D0
#define PRT_3D_CTRL_MQTT_DIM_2          "light_two"
#define PRT_3D_CTRL_DHT_DATA_1          WEMOS_PIN_D6
#define PRT_3D_CTRL_DHT_PWR_1           WEMOS_PIN_D5
#define PRT_3D_CTRL_DHT_DATA_2          WEMOS_PIN_D8
#define PRT_3D_CTRL_DHT_PWR_2           WEMOS_PIN_D7

#define NEOPIXELS_PIN                   WEMOS_PIN_D1
#define MQTT_NEOPIXELS                  "light_one"

// RGB FET
#define H801_PIN_RED                    15u //12
#define H801_PIN_GREEN                  13u //15
#define H801_PIN_BLUE                   12u //13
// W FET
#define H801_PIN_W1                     14u
#define H801_PIN_W2                     4u
// onbaord green LED D1
#define H801_PIN_LED                    5U
// onbaord red LED D2
#define H801_PIN_LED2                   1U
// beause of current consumption, set max dim to 800 digits
#define MAX_DIM_DIGITS                  800U

#define MQTT_H801_LIGHT_RED             "light_one"
#define MQTT_H801_LIGHT_GREEN           "light_two"
#define MQTT_H801_LIGHT_BLUE            "light_three"
#define MQTT_H801_LIGHT_W1              "light_four"
#define MQTT_H801_LIGHT_W2              "light_five"
#define MQTT_H801_LIGHT_LED             "light_six"
#define MQTT_H801_LIGHT_LED2            "light_seven"

#define RELAY_PIN_ONE                   WEMOS_PIN_D1
#define RELAY_PIN_TWO                   WEMOS_PIN_D2
#define RELAY_PIN_THREE                 WEMOS_PIN_D3
#define RELAY_PIN_FOUR                  WEMOS_PIN_D4

#define RELAY_MCP_PIN_ONE               0
#define RELAY_MCP_PIN_TWO               1
#define RELAY_MCP_PIN_THREE             2
#define RELAY_MCP_PIN_FOUR              3
#define RELAY_MCP_PIN_FIVE              4
#define RELAY_MCP_PIN_SIX               5
#define RELAY_MCP_PIN_SEVEN             6
#define RELAY_MCP_PIN_EIGHT             7
#define DHT_R8_OUT_DATA_PIN             WEMOS_PIN_D3
#define DHT_R8_OUT_PWR_PIN              WEMOS_PIN_D7
#define DHT_R8_IN_DATA_PIN              WEMOS_PIN_D8
#define DHT_R8_IN_PWR_PIN               WEMOS_PIN_D6
#define DHT_R8_REPORT_CYCLE_TIME        60u

#define RELAY_ESP_PIN_ONE               WEMOS_PIN_D2
#define RELAY_ESP_PIN_TWO               WEMOS_PIN_D1
#define RELAY_ESP_PIN_THREE             WEMOS_PIN_D0
#define RELAY_ESP_PIN_FOUR              WEMOS_PIN_D5
#define RELAY_ESP_PIN_FIVE              WEMOS_PIN_D3
#define RELAY_ESP_PIN_SIX               WEMOS_PIN_D6
#define RELAY_ESP_PIN_SEVEN             WEMOS_PIN_D7
#define RELAY_ESP_PIN_EIGHT             WEMOS_PIN_D8


#define DHT_POWER_ON_TIME               15u
#define DHT_DEEPSLEEP_TIME              60u

#define PIR_INPUT_PIN                   WEMOS_PIN_D3
#define PIR_LED_PIN                     WEMOS_PIN_D4

#define MS_PIR_INPUT_PIN                WEMOS_PIN_D5
#define MS_DHT_OUT_DATA_PIN             WEMOS_PIN_D3
#define MS_DHT_OUT_PWR_PIN              WEMOS_PIN_D7
#define MS_TEMT6000_OUT_PWR_PIN         WEMOS_PIN_D8
#define MS_DHT_REPORT_CYCLE_TIME        30u
#define MS_RELAY_PIN_ONE                WEMOS_PIN_D6
 

#define PIR_SONOFF_INPUT_PIN            WEMOS_PIN_D5

#define BME_PWR_PIN                     WEMOS_PIN_D3
#define BME_LED_PIN                     WEMOS_PIN_D4
#define BME_REPORT_CYCLE_TIME           5u
#define BME_POWER_ON_TIME               5u
#define BME_DEEPSLEEP_TIME              60u

#define DIM_LIGHT_1                     WEMOS_PIN_D4
#define MQTT_DIM_LIGHT_1                "light_one"


#define MQTT_CHAN_ONE                   "relay_one"
#define MQTT_CHAN_TWO                   "relay_two"
#define MQTT_CHAN_THREE                 "relay_three"
#define MQTT_CHAN_FOUR                  "relay_four"
#define MQTT_CHAN_FIVE                  "relay_five"
#define MQTT_CHAN_SIX                   "relay_six"
#define MQTT_CHAN_SEVEN                 "relay_seven"
#define MQTT_CHAN_EIGHT                 "relay_eight"


/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructorfor the device fectory
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DeviceFactory::DeviceFactory(Trace * p_trace)
{
    this->trace_p = p_trace;
}

/**---------------------------------------------------------------------------------------
 * @brief     Method to check the trace channel according to the capabilities
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     cap_u8    capabilities
 * @return    a MqttDevice if successful, else NULL
*//*-----------------------------------------------------------------------------------*/
void DeviceFactory::SelectTraceChannel(uint8_t chan_u8)
{
    trace_p->SelectTraceChannel(chan_u8);
}

/**---------------------------------------------------------------------------------------
 * @brief     Method to generate devices based on the specified type
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8     type of device
 * @return    a MqttDevice if successful, else NULL
*//*-----------------------------------------------------------------------------------*/
LinkedList<MqttDevice*> * DeviceFactory::GenerateDevice(uint8_t cap_u8)
{
    LinkedList<MqttDevice*> *deviceList_p = new LinkedList<MqttDevice*>();
    
    MqttDevice * device_p = NULL;
    SonoffBasic *sonoffDevice_p = NULL;
    Pir *pirDevice_p = NULL;
    GpioDevice *gpio_p = NULL;
    GpioDevice *gpio2_p = NULL;

    switch(cap_u8)
    {
        case CAPABILITY_SINGLE_RELAY:
            gpio_p   = new EspGpio(trace_p, RELAY_PIN_ONE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_ONE, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_DHT_SENSOR:
            device_p = new DhtSensor(trace_p);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated dht device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_SONOFF_BASIC:
            sonoffDevice_p = new SonoffBasic(trace_p);
            sonoffDevice_p->SetSelf(sonoffDevice_p);
            device_p = sonoffDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated sonoff basic device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_PIR:
            //pirDevice_p = new Pir(trace_p);
            pirDevice_p = new Pir(trace_p, PIR_INPUT_PIN, PIR_LED_PIN);
            pirDevice_p->SetSelf(pirDevice_p);
            device_p = pirDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated pir device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_DHT_SENSOR_BAT:
            device_p = new DhtSensor(trace_p);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated dht device");
            deviceList_p->add(device_p);
            device_p = new PowerSave(trace_p, true, DHT_POWER_ON_TIME, DHT_DEEPSLEEP_TIME);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated power save device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_FOUR_RELAY:
            gpio_p   = new EspGpio(trace_p, RELAY_PIN_ONE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_ONE, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device one");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_PIN_TWO, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_TWO, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device two");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_PIN_THREE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_THREE, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device three");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_PIN_FOUR, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_FOUR, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device four");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_PIR_RELAY:
            gpio_p   = new EspGpio(trace_p, RELAY_PIN_ONE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_ONE, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device");
            deviceList_p->add(device_p);
            pirDevice_p = new Pir(trace_p, PIR_INPUT_PIN, PIR_LED_PIN);
            pirDevice_p->SetSelf(pirDevice_p);
            device_p = pirDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated pir device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_FOUR_RELAY_MCP:
            gpio_p   = new McpGpio(trace_p, RELAY_MCP_PIN_ONE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_ONE, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device one");
            deviceList_p->add(device_p);
            gpio_p   = new McpGpio(trace_p, RELAY_MCP_PIN_TWO, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_TWO, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device two");
            deviceList_p->add(device_p);
            gpio_p   = new McpGpio(trace_p, RELAY_MCP_PIN_THREE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_THREE, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device three");
            deviceList_p->add(device_p);
            gpio_p   = new McpGpio(trace_p, RELAY_MCP_PIN_FOUR, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_FOUR, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device four");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_DOUBLE_DHT:
            gpio_p   = new EspGpio(trace_p, DHT_R8_OUT_PWR_PIN, OUTPUT);
            device_p = new DhtSensor(trace_p, DHT_R8_OUT_DATA_PIN, gpio_p, DHT_R8_REPORT_CYCLE_TIME);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated outside dht device");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, DHT_R8_IN_PWR_PIN, OUTPUT);
            device_p = new DhtSensor(trace_p, DHT_R8_IN_DATA_PIN, gpio_p, DHT_R8_REPORT_CYCLE_TIME, 1);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated inside dht device");
            deviceList_p->add(device_p);
            device_p = new Sen0193(trace_p);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated moisture sensor 0193 device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_EIGHT_RELAY_ESP:
            gpio_p   = new EspGpio(trace_p, RELAY_ESP_PIN_ONE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_ONE, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device one");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_ESP_PIN_TWO, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_TWO, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device two");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_ESP_PIN_THREE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_THREE, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device three");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_ESP_PIN_FOUR, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_FOUR, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device four");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_ESP_PIN_FIVE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_FIVE, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device five");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_ESP_PIN_SIX, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_SIX, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device six");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_ESP_PIN_SEVEN, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_SEVEN, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device seven");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, RELAY_ESP_PIN_EIGHT, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_EIGHT, false);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device eight");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_BME_SENSOR:
            gpio_p   = new EspGpio(trace_p, BME_PWR_PIN, OUTPUT);
            gpio2_p   = new EspGpio(trace_p, BME_LED_PIN, OUTPUT);
            device_p = new Bme280Sensor(trace_p, gpio_p, gpio2_p, BME_REPORT_CYCLE_TIME);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated bme device");
            deviceList_p->add(device_p);
            device_p = new PowerSave(trace_p, true, BME_POWER_ON_TIME, BME_DEEPSLEEP_TIME);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated power save device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_SONOFF_PIR:
            sonoffDevice_p = new SonoffBasic(trace_p);
            sonoffDevice_p->SetSelf(sonoffDevice_p);
            device_p = sonoffDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated sonoff basic device");
            deviceList_p->add(device_p);
            pirDevice_p = new Pir(trace_p, PIR_SONOFF_INPUT_PIN);
            pirDevice_p->SetSelf(pirDevice_p);
            device_p = pirDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated pir device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_MOISTURE_ONLY:
            device_p = new Sen0193(trace_p);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated moisture sensor 0193 device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_MULTI_SENSE:
            device_p = new GenSensor(trace_p);
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, MS_DHT_OUT_PWR_PIN, OUTPUT);
            device_p = new DhtSensor(trace_p, MS_DHT_OUT_DATA_PIN, gpio_p, MS_DHT_REPORT_CYCLE_TIME);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated dht device");
            deviceList_p->add(device_p);
            pirDevice_p = new Pir(trace_p, MS_PIR_INPUT_PIN);
            pirDevice_p->SetSelf(pirDevice_p);
            device_p = pirDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated pir device");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, MS_TEMT6000_OUT_PWR_PIN, OUTPUT);
            device_p = new Temt6000(trace_p, gpio_p);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated temt6000 device");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, NEOPIXELS_PIN, OUTPUT);
            device_p = new NeoPix(trace_p, gpio_p, MQTT_NEOPIXELS);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated neopixels object");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_MULTI_SENSE_RELAY:
            device_p = new GenSensor(trace_p);
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, MS_DHT_OUT_PWR_PIN, OUTPUT);
            device_p = new DhtSensor(trace_p, MS_DHT_OUT_DATA_PIN, gpio_p, MS_DHT_REPORT_CYCLE_TIME);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated dht device");
            deviceList_p->add(device_p);
            pirDevice_p = new Pir(trace_p, MS_PIR_INPUT_PIN);
            pirDevice_p->SetSelf(pirDevice_p);
            device_p = pirDevice_p;
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated pir device");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, MS_TEMT6000_OUT_PWR_PIN, OUTPUT);
            device_p = new Temt6000(trace_p, gpio_p);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated temt6000 device");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, NEOPIXELS_PIN, OUTPUT);
            device_p = new NeoPix(trace_p, gpio_p, MQTT_NEOPIXELS);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated neopixels object");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, MS_RELAY_PIN_ONE, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, MQTT_CHAN_ONE, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_DIM_LIGHT:
            gpio_p   = new EspGpio(trace_p, DIM_LIGHT_1, OUTPUT);
            device_p = new DimLight(trace_p, gpio_p, MQTT_DIM_LIGHT_1);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated dim light one");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_H801:
            gpio_p   = new EspGpio(trace_p, H801_PIN_RED, OUTPUT);
            device_p = new DimLight(trace_p, gpio_p, MQTT_H801_LIGHT_RED, MAX_DIM_DIGITS);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated h801 red chan");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, H801_PIN_GREEN, OUTPUT);
            device_p = new DimLight(trace_p, gpio_p, MQTT_H801_LIGHT_GREEN, MAX_DIM_DIGITS);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated h801 green chan");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, H801_PIN_BLUE, OUTPUT);
            device_p = new DimLight(trace_p, gpio_p, MQTT_H801_LIGHT_BLUE, MAX_DIM_DIGITS);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated h801 blue chan");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, H801_PIN_W1, OUTPUT);
            device_p = new DimLight(trace_p, gpio_p, MQTT_H801_LIGHT_W1);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated h801 W1 chan");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, H801_PIN_W2, OUTPUT);
            device_p = new DimLight(trace_p, gpio_p, MQTT_H801_LIGHT_W2);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated h801 W2 chan");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, H801_PIN_LED, OUTPUT);
            device_p = new DimLight(trace_p, gpio_p, MQTT_H801_LIGHT_LED);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated h801 led");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, H801_PIN_LED2, OUTPUT);
            device_p = new DimLight(trace_p, gpio_p, MQTT_H801_LIGHT_LED2);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated h801 led2");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_NEOPIXELS:
            gpio_p   = new EspGpio(trace_p, NEOPIXELS_PIN, OUTPUT);
            device_p = new NeoPix(trace_p, gpio_p, MQTT_NEOPIXELS);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated neopixels object");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_3D_PRINTER:
            gpio_p   = new EspGpio(trace_p, PRT_3D_CTRL_DIM_1);
            device_p = new DimLight(trace_p, gpio_p, PRT_3D_CTRL_MQTT_DIM_1);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated h801 red chan");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, PRT_3D_CTRL_DIM_2);
            device_p = new DimLight(trace_p, gpio_p, PRT_3D_CTRL_MQTT_DIM_2);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated h801 red chan");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, PRT_3D_CTRL_REL_1, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, PRT_3D_CTRL_MQTT_REL_1, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device one");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, PRT_3D_CTRL_REL_2, OUTPUT);
            device_p = new SingleRelay(trace_p, gpio_p, PRT_3D_CTRL_MQTT_REL_2, true);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated single relay device one");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, PRT_3D_CTRL_DHT_PWR_1, OUTPUT);
            device_p = new DhtSensor(trace_p, PRT_3D_CTRL_DHT_DATA_1, gpio_p, MS_DHT_REPORT_CYCLE_TIME);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated dht device");
            deviceList_p->add(device_p);
            gpio_p   = new EspGpio(trace_p, PRT_3D_CTRL_DHT_PWR_2, OUTPUT);
            device_p = new DhtSensor(trace_p, PRT_3D_CTRL_DHT_DATA_2, gpio_p, MS_DHT_REPORT_CYCLE_TIME, 1);
            trace_p->println(trace_INFO_MSG, "<<devMgr>> generated dht device");
            deviceList_p->add(device_p);
            break;
        case CAPABILITY_TEST_DEVICE:
        default:
            device_p = new GenSensor(trace_p, true);
            deviceList_p->add(device_p);
            break;
    }

    return(deviceList_p);
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
DeviceFactory::~DeviceFactory()
{
    // TODO Auto-generated destructor stub
}

/****************************************************************************************/
/* Private functions: */

