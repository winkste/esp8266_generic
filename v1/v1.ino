/*****************************************************************************************
* FILENAME :        v1.c             
*
* DESCRIPTION :
*       Main module
*
* PUBLIC FUNCTIONS :
*
*
* NOTES :
*       This module supports the WIFI configuration and FW Update
*       based on the library:
*       https://github.com/tzapu/WiFiManager
*       ssid of config page: OPEN_ESP_CONFIG_devXX
*       ip address: 192.168.4.1
*       Also toggleing the button at start will startup the WIFI
*       configuration.
*       
*       The basicSwitch implements the MQTT sonoff basic switch
*       functionality to turn on/off the relay in the switch. Additional
*       the LED will be switched to indicate the status of the 
*       relay.
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
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*****************************************************************************************/

/*****************************************************************************************
 * Include Interfaces
*****************************************************************************************/
#include <ESP8266WiFi.h>
#include "WiFiManager.h" // local modified version          
#include <PubSubClient.h>
#include <EEPROM.h>
#include "gensettings.h"
#include "Trace.h"
#include "DeviceFactory.h"
#include "LinkedList.h"
#include "MqttDevice.h"

#include "version.h"

/*****************************************************************************************
 * Local constant defines
*****************************************************************************************/

/*****************************************************************************************
 * Local function like makros 
*****************************************************************************************/

/*****************************************************************************************
 * Local type definitions (enum, struct, union):
*****************************************************************************************/

/*****************************************************************************************
 * Global data definitions (unlimited visibility, to be avoided):
*****************************************************************************************/

/*****************************************************************************************
 * Local data definitions:
 * (always use static: limit visibility, const: read only, volatile: non-optimizable) 
*****************************************************************************************/
static char buffer_stca[60];
static char ssid_stca[60];

// buffer used to send/receive data with MQTT, can not be done with the 
// buffer_stca, as both are needed simultaniously 
static WiFiClient            wifiClient_sts;
static PubSubClient          client_sts(wifiClient_sts);
static mqttData_t            mqttData_sts;
static Trace                 trace_st(true);
static DeviceFactory         factory_st(&trace_st);
//static MqttDevice            *device_pst = NULL;
static LinkedList<MqttDevice*> *deviceList_pst = NULL;
static WiFiManager           wifiManager_sts;
// prepare wifimanager variables
static WiFiManagerParameter  wifiManagerParamMqttServerId_sts("mq_ip", "mqtt server ip", "", 15);
static WiFiManagerParameter  wifiManagerParamMqttServerPort_sts("mq_port", "mqtt server port", "1883", 5);
static WiFiManagerParameter  wifiManagerParamMqttCapability_sts("cap", "Capability Bit0 = n/a, Bit1 = n/a, Bit2 = n/a", "", 2);
static WiFiManagerParameter  wifiManagerParamMqttClientShort_sts("sid", "mqtt short id", "devXX", 6);
static WiFiManagerParameter  wifiManagerParamMqttServerLogin_sts("login", "mqtt login", "", 15);
static WiFiManagerParameter  wifiManagerParamMqttServerPw_sts("pw", "mqtt pw", "", 15);

static uint32_t             timerRepubAvoid_u32st = 0;
static uint32_t             timerLastPub_u32st = 0;
static boolean              publishInfo_bolst = false;
static boolean              startWifiConfig_bolst = false;

/*****************************************************************************************
* Global functions (unlimited visibility): 
*****************************************************************************************/

/**---------------------------------------------------------------------------------------
 * @brief     This function processes the publish requests and is called cyclic.   
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    This function returns 'true' if the function processing was successful.
 *             In all other cases it returns 'false'.
*//*-----------------------------------------------------------------------------------*/
boolean processPublishRequests(void)
{
  uint8_t idx_u8 = 0;
  String tPayload;
  boolean ret_bol = false;

  if(true == publishInfo_bolst)
  {
    trace_st.println(trace_PURE_MSG, "");
    trace_st.print(trace_INFO_MSG, "[mqtt] publish requested info: ");
    trace_st.print(trace_PURE_MSG, FW_IDENTIFIER);
    trace_st.println(trace_PURE_MSG, FW_VERSION);
    trace_st.println(trace_INFO_MSG, FW_DESCRIPTION);
    ret_bol = client_sts.publish(build_topic(MQTT_PUB_FW_IDENT), FW_IDENTIFIER, true);
    ret_bol &= client_sts.publish(build_topic(MQTT_PUB_FW_VERSION), FW_VERSION, true);
    ret_bol &= client_sts.publish(build_topic(MQTT_PUB_FW_DESC), FW_DESCRIPTION, true);
    if(ret_bol)
    {
      publishInfo_bolst = false;     
    }
  }
  else
  {
    idx_u8 = 0;
    while(idx_u8 < deviceList_pst->size())
    {
      deviceList_pst->get(idx_u8)->ProcessPublishRequests(&client_sts);
      idx_u8++;
    }
  }
 
  return(ret_bol);  
}

/**---------------------------------------------------------------------------------------
 * @brief     This callback function processes incoming MQTT messages and is called by   
 *              the PubSub client
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     p_topic     topic which was received
 * @param     p_payload   payload of the received MQTT message
 * @param     p_length    payload length
 * @return    This function returns 'true' if the function processing was successful.
 *             In all other cases it returns 'false'.
*//*-----------------------------------------------------------------------------------*/
void callback(char* p_topic, byte* p_payload, unsigned int p_length) 
{
  uint8_t idx_u8 = 0;
  String payload;

  // concat the payload into a string
  for (uint8_t i = 0; i < p_length; i++) 
  {
    payload.concat((char)p_payload[i]);
  }
  
  // print received topic and payload
  trace_st.print(trace_INFO_MSG, "[mqtt] message received: ");
  trace_st.print(trace_PURE_MSG, p_topic);
  trace_st.print(trace_PURE_MSG, "   ");
  trace_st.println(trace_PURE_MSG, payload);

  // execute generic support command
  if(String(build_topic(MQTT_SUB_COMMAND)).equals(p_topic)) 
  {
    // print firmware information
    if(0 == payload.indexOf(String(MQTT_PAYLOAD_CMD_INFO))) 
    {
      publishInfo_bolst = true;     
    }
    // goto wifimanager configuration 
    else if(0 == payload.indexOf(String(MQTT_PAYLOAD_CMD_SETUP))) 
    {
      startWifiConfig_bolst = true; 
    } 
    else
    {
      trace_st.print(trace_ERROR_MSG, "[mqtt] unexpected command: "); 
      trace_st.println(trace_PURE_MSG, payload);
    }
  }
  else if(String(MQTT_SUB_BCAST).equals(p_topic))
  {
    // print firmware information
    if(0 == payload.indexOf(String(MQTT_PAYLOAD_CMD_INFO))) 
    {
      publishInfo_bolst = true;     
    }
    else
    {
      trace_st.print(trace_ERROR_MSG, "[mqtt] unexpected command: "); 
      trace_st.println(trace_PURE_MSG, payload);
    }   
  }
  else
  {
    idx_u8 = 0;
    while(idx_u8 < deviceList_pst->size())
    {
      deviceList_pst->get(idx_u8)->CallbackMqtt(&client_sts, p_topic, payload);
      idx_u8++;
    }
  }  
}

/**---------------------------------------------------------------------------------------
 * @brief     This function handles the connection to the MQTT broker. If connection can't
 *              be established after several attempts the WifiManager is called. If 
 *              connection is successfull, all needed subscriptions are done.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void reconnect() 
{
  // Loop until we're reconnected
  uint8_t idx_u8 = 0;
  uint8_t tries = 0;
  while (!client_sts.connected()) 
  {
    trace_st.println(trace_INFO_MSG, "<<mqtt>> Attempting connection...");
    // Attempt to connect
    trace_st.print(trace_INFO_MSG, "<<mqtt>> client id: ");
    trace_st.println(trace_PURE_MSG, mqttData_sts.dev_short);
    if(client_sts.connect(mqttData_sts.dev_short, mqttData_sts.login, mqttData_sts.pw)) 
    {
      trace_st.InitializeMqtt(&client_sts, mqttData_sts.dev_short);
      trace_st.println(trace_INFO_MSG, "<<mqtt>> connected");
      client_sts.loop();
      trace_st.print(trace_INFO_MSG,"<<mqtt>> subscribed generic: ");
      trace_st.println(trace_PURE_MSG, MQTT_SUB_COMMAND);
      client_sts.subscribe(build_topic(MQTT_SUB_COMMAND));  // request general command with payload
      client_sts.loop();
      trace_st.print(trace_INFO_MSG,"<<mqtt>> broadcast topic: ");
      trace_st.println(trace_PURE_MSG, MQTT_SUB_BCAST);
      client_sts.subscribe(MQTT_SUB_BCAST);  // request broadcast command with payload
      client_sts.loop();

      // reconnect all client device topics
      idx_u8 = 0;
      while(idx_u8 < deviceList_pst->size())
      {
        deviceList_pst->get(idx_u8)->Reconnect(&client_sts, mqttData_sts.dev_short);
        idx_u8++;
      }

      // send out generic commands
      trace_st.println(trace_INFO_MSG, "<<mqtt>> subscribing finished");
      trace_st.print(trace_INFO_MSG, "<<mqtt>> publish firmware partnumber: ");
      trace_st.print(trace_PURE_MSG, FW_IDENTIFIER);
      trace_st.println(trace_PURE_MSG, FW_VERSION);
      trace_st.print(trace_INFO_MSG, "<<mqtt>> publish firmware description: ");
      trace_st.println(trace_PURE_MSG, FW_DESCRIPTION);
      client_sts.publish(build_topic(MQTT_PUB_FW_IDENT), FW_IDENTIFIER, true);
      client_sts.publish(build_topic(MQTT_PUB_FW_VERSION), FW_VERSION, true);
      client_sts.publish(build_topic(MQTT_PUB_FW_DESC), FW_DESCRIPTION, true);
      trace_st.println(trace_INFO_MSG, "<<mqtt>> publishing finished");
    } 
    else 
    {
      trace_st.print(trace_ERROR_MSG, "failed, rc="); 
      trace_st.print(trace_PURE_MSG, String(client_sts.state())); 
      trace_st.println(trace_PURE_MSG, ", try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    tries++;
    if(tries >= CONNECT_RETRIES)
    {
      trace_st.println(trace_ERROR_MSG, "Can't connect, starting AP");
      wifiManager_sts.startConfigPortal(build_ssid(CONFIG_SSID)); // needs to be tested!
    }
  }
}

/**---------------------------------------------------------------------------------------
 * @brief     This callback function handles the wifimanager callback
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     myWiFiManager     pointer to the wifimanager
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void configModeCallback(WiFiManager *myWiFiManager) 
{
  wifiManager_sts.addParameter(&wifiManagerParamMqttServerId_sts);
  wifiManager_sts.addParameter(&wifiManagerParamMqttServerPort_sts);
  wifiManager_sts.addParameter(&wifiManagerParamMqttCapability_sts);
  wifiManager_sts.addParameter(&wifiManagerParamMqttClientShort_sts);
  wifiManager_sts.addParameter(&wifiManagerParamMqttServerLogin_sts);
  wifiManager_sts.addParameter(&wifiManagerParamMqttServerPw_sts);
  // prepare wifimanager variables
  wifiManager_sts.setAPStaticIPConfig(IPAddress(192,168,4,1), 
                                        IPAddress(192,168,4,255), 
                                        IPAddress(255,255,255,0));
  trace_st.println(trace_INFO_MSG, "entered config mode");
}

/**---------------------------------------------------------------------------------------
 * @brief     This callback function handles the wifimanager save callback
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void saveConfigCallback()
{ 
  sprintf(mqttData_sts.server_ip, "%s", wifiManagerParamMqttServerId_sts.getValue());
  sprintf(mqttData_sts.login, "%s", wifiManagerParamMqttServerLogin_sts.getValue());
  sprintf(mqttData_sts.pw, "%s", wifiManagerParamMqttServerPw_sts.getValue());
  sprintf(mqttData_sts.cap, "%s", wifiManagerParamMqttCapability_sts.getValue());
  sprintf(mqttData_sts.server_port, "%s", wifiManagerParamMqttServerPort_sts.getValue());
  sprintf(mqttData_sts.dev_short, "%s", wifiManagerParamMqttClientShort_sts.getValue());
  trace_st.println(trace_INFO_MSG, "=== Saving parameters: ===");
  trace_st.print(trace_INFO_MSG, "mqtt ip: ");        
  trace_st.println(trace_PURE_MSG, mqttData_sts.server_ip);
  trace_st.print(trace_INFO_MSG, "mqtt port: ");      
  trace_st.println(trace_PURE_MSG, mqttData_sts.server_port);
  trace_st.print(trace_INFO_MSG, "mqtt user: ");      
  trace_st.println(trace_PURE_MSG, mqttData_sts.login);
  trace_st.print(trace_INFO_MSG, "mqtt pw: ");        
  trace_st.println(trace_PURE_MSG, mqttData_sts.pw);
  trace_st.print(trace_INFO_MSG, "mqtt dev short: "); 
  trace_st.println(trace_PURE_MSG, mqttData_sts.dev_short);
  trace_st.print(trace_INFO_MSG, "capabilities: ");   
  trace_st.println(trace_PURE_MSG, mqttData_sts.cap);
  trace_st.println(trace_INFO_MSG, "=== End of parameters ==="); 
  char* temp=(char*) &mqttData_sts;
  for(int i=0; i<sizeof(mqttData_sts); i++){
    EEPROM.write(i,*temp);
    //Serial.print(*temp);
    temp++;
  }
  EEPROM.commit();

  wifiManagerParamMqttServerId_sts.setDefaultValue(&mqttData_sts.server_ip[0], 15);
  wifiManagerParamMqttServerPort_sts.setDefaultValue(&mqttData_sts.server_port[0], 5);
  wifiManagerParamMqttCapability_sts.setDefaultValue(&mqttData_sts.cap[0], 2);
  wifiManagerParamMqttClientShort_sts.setDefaultValue(&mqttData_sts.dev_short[0], 6);
  wifiManagerParamMqttServerLogin_sts.setDefaultValue(&mqttData_sts.login[0], 15);
  wifiManagerParamMqttServerPw_sts.setDefaultValue(&mqttData_sts.pw[0], 15);
  
  trace_st.println(trace_INFO_MSG, "configuration saved, restarting");
  delay(2000);  
  ESP.reset(); // we can't change from AP mode to client mode, thus: reboot
}


/**---------------------------------------------------------------------------------------
 * @brief     This function load the configuration from external eeprom
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     myWiFiManager     pointer to the wifimanager
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void loadConfig()
{
  // fill the mqtt element with all the data from eeprom
  char* temp=(char*) &mqttData_sts;
  
  for(int i=0; i<sizeof(mqttData_sts); i++)
  {
    *temp = EEPROM.read(i);
    temp++;
  }

  wifiManagerParamMqttServerId_sts.setDefaultValue(&mqttData_sts.server_ip[0], 15);
  wifiManagerParamMqttServerPort_sts.setDefaultValue(&mqttData_sts.server_port[0], 5);
  wifiManagerParamMqttCapability_sts.setDefaultValue(&mqttData_sts.cap[0], 2);
  wifiManagerParamMqttClientShort_sts.setDefaultValue(&mqttData_sts.dev_short[0], 6);
  wifiManagerParamMqttServerLogin_sts.setDefaultValue(&mqttData_sts.login[0], 15);
  wifiManagerParamMqttServerPw_sts.setDefaultValue(&mqttData_sts.pw[0], 15);
  
  trace_st.println(trace_INFO_MSG, "=== Loaded parameters: ===");
  trace_st.print(trace_INFO_MSG, "mqtt ip: ");        
  trace_st.println(trace_PURE_MSG, mqttData_sts.server_ip);
  trace_st.print(trace_INFO_MSG, "mqtt port: ");      
  trace_st.println(trace_PURE_MSG, mqttData_sts.server_port);
  trace_st.print(trace_INFO_MSG, "mqtt user: ");      
  trace_st.println(trace_PURE_MSG, mqttData_sts.login);
  trace_st.print(trace_INFO_MSG, "mqtt pw: ");        
  trace_st.println(trace_PURE_MSG, mqttData_sts.pw);
  trace_st.print(trace_INFO_MSG, "mqtt dev short: "); 
  trace_st.println(trace_PURE_MSG, mqttData_sts.dev_short);
  trace_st.print(trace_INFO_MSG, "capabilities: ");   
  trace_st.println(trace_PURE_MSG, mqttData_sts.cap);

  // capabilities
  deviceList_pst = factory_st.GenerateDevice(atoi(&mqttData_sts.cap[0]));
  //trace_st.println(trace_INFO_MSG, atoi(&mqttData_sts.cap[0]));
  // capabilities
  
  trace_st.println(trace_INFO_MSG, "=== End of parameters ===");
}

/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     topic       pointer to topic string
 * @return    combined topic as char pointer, it uses buffer_stca to store the topic
*//*-----------------------------------------------------------------------------------*/
char* build_topic(const char *topic) 
{
  sprintf(buffer_stca, "%s%s", mqttData_sts.dev_short, topic);
  return buffer_stca;
}

/**---------------------------------------------------------------------------------------
 * @brief     This function helps to build the complete topic including the 
 *              custom device.
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     topic       pointer to topic string
 * @return    combined topic as char pointer, it uses buffer_stca to store the topic
*//*-----------------------------------------------------------------------------------*/
char* build_ssid(const char *ssidName) 
{
  sprintf(ssid_stca, "%s%s", ssidName, mqttData_sts.dev_short);
  return ssid_stca;
}


/**---------------------------------------------------------------------------------------
 * @brief     This is the setup callback function
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void setupCallback() 
{
  uint8_t idx_u8 = 0;
  
  // init the serial
  //Serial.begin(115200);
  // first task: initialize Trace
  trace_st.Initialize();

  trace_st.println(trace_PURE_MSG,""); 
  trace_st.println(trace_INFO_MSG, "... starting");
  trace_st.print(trace_INFO_MSG, "Firmware Information:");  
  trace_st.print(trace_PURE_MSG, FW_IDENTIFIER); 
  trace_st.println(trace_PURE_MSG, FW_VERSION);
  trace_st.print(trace_INFO_MSG, "Firmware Description:");  
  trace_st.println(trace_PURE_MSG, FW_DESCRIPTION);
  EEPROM.begin(512); // can be up to 4096
 
  // start wifi manager
  wifiManager_sts.setAPCallback(configModeCallback);
  wifiManager_sts.setSaveConfigCallback(saveConfigCallback);
  wifiManager_sts.setConfigPortalTimeout(MAX_AP_TIME);
  WiFi.mode(WIFI_STA); // avoid station and ap at the same time

  trace_st.println(trace_INFO_MSG, "<<wifi>> connecting... ");
  if(!wifiManager_sts.autoConnect(build_ssid(CONFIG_SSID))){
    // possible situataion: Main power out, ESP went to config mode as the routers wifi wasn available on time .. 
    trace_st.println(trace_ERROR_MSG, "<<wifi>> failed to connect and hit timeout, restarting ...");
    delay(1000); // time for serial to print
    ESP.reset(); // reset loop if not only or configured after 5min .. 
  }

  // load parameters from eeprom
  loadConfig();

  // initialize devices
  idx_u8 = 0;
  while(idx_u8 < deviceList_pst->size())
  {
    deviceList_pst->get(idx_u8)->Initialize();
    idx_u8++;
  }
  
  trace_st.println(trace_INFO_MSG, "<<wifi>> connected");
  trace_st.print(trace_INFO_MSG, "<<wifi>>  IP address: "); 
  trace_st.println(trace_PURE_MSG, WiFi.localIP().toString());

  // init the MQTT connection
  client_sts.setServer(mqttData_sts.server_ip, atoi(mqttData_sts.server_port));
  client_sts.setCallback(callback);
}

/**---------------------------------------------------------------------------------------
 * @brief     This callback function handles the loop function
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void loopCallback() 
{

	if (!client_sts.connected()) 
	{
		reconnect();
 	}
 	client_sts.loop();

	//// check for publish requests, but keep an minimum time between two publifications
  if(millis()-timerLastPub_u32st > PUBLISH_TIME_OFFSET)
  {
    processPublishRequests();
    timerRepubAvoid_u32st = millis();
    timerLastPub_u32st = millis();

    trace_st.PushToChannel();
  }

  // check for a re-configuration trigger
	if((true == startWifiConfig_bolst) || MqttDevice::GetReconfigRequest())
	{
    startWifiConfig_bolst = false;
		trace_st.println(trace_INFO_MSG, "<<sys>> Rebooting to setup mode");
		delay(200);
		wifiManager_sts.startConfigPortal(build_ssid(CONFIG_SSID)); // needs to be tested!
		//ESP.reset(); // reboot and switch to setup mode right after that
	}
}

/**---------------------------------------------------------------------------------------
 * @brief     This is the standard setup function
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void setup() 
{
  setupCallback();
}

/**---------------------------------------------------------------------------------------
 * @brief     This is the standard loop funcktion
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void loop() 
{
  loopCallback();
}


