/*****************************************************************************************
* FILENAME :        TraceSerial.cpp
*
* DESCRIPTION :
*       serial debug support
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

/*****************************************************************************************
 * Include Interfaces
*****************************************************************************************/
#include <iostream>
#include <string>
using namespace std;

#include "Trace.h"
#include "TraceSerial.h"

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
* Public functions (unlimited visibility): 
*****************************************************************************************/

/**---------------------------------------------------------------------------------------
 * @brief     Default constructor   
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
TraceSerial::TraceSerial() : Trace()
{

}

/**---------------------------------------------------------------------------------------
 * @brief     Default constructor for channel 0
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     isActive_bol     trace active or not
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
TraceSerial::TraceSerial(bool isActive_bol) : Trace(isActive_bol)
{
    this->isActive_bol = isActive_bol;
    this->channel_u8 = 0;

}

/**---------------------------------------------------------------------------------------
 * @brief     Full constructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     isActive_bol     trace active or not
 * @param     channel_u16      channel identifier to allow later different trace channels
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
TraceSerial::TraceSerial(bool isActive_bol, uint8_t channel_u8) : Trace(isActive_bol, channel_u8)
{
    this->isActive_bol = isActive_bol;
    this->channel_u8 = channel_u8;
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
TraceSerial::~TraceSerial()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the trace interface selected with the constructor
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void TraceSerial::Initialize()
{
    // init the serial
    cout << "started...";
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for String parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void TraceSerial::print(uint8_t type_u8, string msg_str)
{
  Trace::prepareMsg(type_u8, msg_str);
  cout << buffer_str;
  
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for String parameter with new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_str         message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void TraceSerial::println(uint8_t type_u8, string msg_str)
{
  Trace::prepareMsg(type_u8, string(msg_str));
  cout << buffer_str << endl;
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for char pointer parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_pc          message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void TraceSerial::print(uint8_t type_u8, char *msg_pc)
{
  Trace::prepareMsg(type_u8, string(msg_pc));
  cout << buffer_str;
  
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for char pointer parameter with new line 
 *              at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     msg_pc          message
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void TraceSerial::println(uint8_t type_u8, char *msg_pc)
{
  Trace::prepareMsg(type_u8, string(msg_pc));
  cout << buffer_str << endl;
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print function for uint8_t parameter w/o new line at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     value_u8        data value
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void TraceSerial::print(uint8_t type_u8, uint8_t value_u8)
{
  Trace::prepareMsg(type_u8, std::to_string(value_u8));
  cout << buffer_str;
}

/**---------------------------------------------------------------------------------------
 * @brief     Trace print line function for uint8_t parameter with new line 
 *              at the end
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     type_u8         trace message type
 * @param     value_u8        data value
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void TraceSerial::println(uint8_t type_u8, uint8_t value_u8)
{
  Trace::prepareMsg(type_u8, std::to_string(value_u8));
  cout << buffer_str << endl;;
}

/**---------------------------------------------------------------------------------------
 * @brief     For asynchronous data channels, this function can be used to send 
 *            queued messages to the channel at a dedicated time.
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void TraceSerial::PushToChannel()
{
    // not needed in this implementation
}

/****************************************************************************************/
/* Private functions: */
/****************************************************************************************/

