/*****************************************************************************************
* FILENAME :        McpGpio.cpp
*
* DESCRIPTION :
*       Implementation of the MCP23017 as GpioDevice for standard interface.
*
* NOTES :
*
*
* Copyright (c) [2018] [Stephan Wink]
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
#include "McpGpio.h"
#include "Trace.h"

/****************************************************************************************/
/* Local constant defines */

/****************************************************************************************/
/* Local function like makros */

/****************************************************************************************/
/* Local type definitions (enum, struct, union) */

int McpGpio::mcp = 0;
bool McpGpio::mcpInitialized_bol = false;

/****************************************************************************************/
/* Public functions (unlimited visibility) */

/**---------------------------------------------------------------------------------------
 * @brief     Constructor for the McpGpio device
 * @author    winkste
 * @date      06. Feb. 2018
 * @param     p_trace     trace object for info and error messages
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
McpGpio::McpGpio(Trace *p_trace) : GpioDevice(p_trace)
{
    this->p_trace->println(trace_INFO_MSG, "<<mcpGpio>> Constructor of MCPDevice called");
    this->stat_u8 = 0;
    this->Initialize();
    this->PinMode();
}

/**---------------------------------------------------------------------------------------
 * @brief     Constructur for McpGpio
 * @author    winkste
 * @date      06. Feb. 2018
 * @param     p_trace   trace object for debug output
 * @param     pin_u8    corresponding pin
 * @return    a GpioDevice object
*//*-----------------------------------------------------------------------------------*/
McpGpio::McpGpio(Trace *p_trace, uint8_t pin_u8) : GpioDevice(p_trace, pin_u8)
{
    this->p_trace->println(trace_INFO_MSG, "<<mcpGpio>> Constructor of MCPDevice called");
    this->stat_u8 = 0;
    this->Initialize();
    this->PinMode();

}

/**---------------------------------------------------------------------------------------
 * @brief     Constructur for McpGpio
 * @author    winkste
 * @date      06. Feb. 2018
 * @param     p_trace   trace object for debug output
 * @param     pin_u8    corresponding pin
 * @param     dir_u8    direction of pin
 * @return    a GpioDevice object
*//*-----------------------------------------------------------------------------------*/
McpGpio::McpGpio(Trace *p_trace, uint8_t pin_u8, uint8_t dir_u8) : GpioDevice(p_trace, pin_u8, dir_u8)
{
    this->p_trace->println(trace_INFO_MSG, "<<mcpGpio>> Constructor of MCPDevice called");
    this->stat_u8 = 0;
    this->Initialize();
    this->PinMode();
}

/**---------------------------------------------------------------------------------------
 * @brief     Default destructor
 * @author    winkste
 * @date      06. Feb. 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
McpGpio::~McpGpio()
{
    // TODO Auto-generated destructor stub
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to set the direction of of the pin
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     dir_u8     direction of pin
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void McpGpio::PinMode(uint8_t dir_u8)
{
    this->p_trace->println(trace_INFO_MSG, "<<mcpgpio>> pinMode called");
    this->PrintPinStat();
    this->dir_u8 = dir_u8;
    this->PinMode();
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to set output pin to dedicated value
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     state_u8  state of the output
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void McpGpio::DigitalWrite(uint8_t state_u8)
{
    this->p_trace->println(trace_INFO_MSG, "<<mcpgpio>> digitalWrite of MCPDevice called");
    this->stat_u8 = state_u8;
    this->PrintPinStat();
}

/**---------------------------------------------------------------------------------------
 * @brief     Function call to set output pin to dedicated value
 * @author    winkste
 * @date      20 Okt. 2017
 * @param     pin_u8    pin selection
 * @return    state of the digital pin
*//*-----------------------------------------------------------------------------------*/
uint8_t McpGpio::DigitalRead()
{
    this->p_trace->println(trace_INFO_MSG, "<<mcpgpio>> digitalRead of MCPDevice called");
    this->PrintPinStat();
    return(this->stat_u8);
}

/****************************************************************************************/
/* Private functions: */
/**---------------------------------------------------------------------------------------
 * @brief     Function call to set output pin to dedicated value
 * @author    winkste
 * @date      20 Okt. 2017
 * @return    ---
*//*-----------------------------------------------------------------------------------*/
void McpGpio::PrintPinStat()
{
    this->p_trace->print(trace_INFO_MSG, "<<mcpgpio>> Pin Status: ");
    this->p_trace->print(trace_PURE_MSG, this->mcp);
    this->p_trace->print(trace_PURE_MSG, " / ");
    this->p_trace->print(trace_PURE_MSG, this->pin_u8);
    this->p_trace->print(trace_PURE_MSG, " / ");
    this->p_trace->print(trace_PURE_MSG, this->dir_u8);
    this->p_trace->print(trace_PURE_MSG, " / ");
    this->p_trace->println(trace_PURE_MSG, this->stat_u8);
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the MCP gpio device
 * @author    winkste
 * @date      06. Feb. 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void McpGpio::Initialize()
{
    if(false == this->mcpInitialized_bol)
    {
        this->mcp += 1;
        this->mcpInitialized_bol = true;
        this->p_trace->println(trace_INFO_MSG, "<<mcpgpio>> MCP initialized");
    }
    else
    {
        // ignore
    }
}

/**---------------------------------------------------------------------------------------
 * @brief     Initialization of the pin mode
 * @author    winkste
 * @date      06. Feb. 2018
 * @return    n/a
*//*-----------------------------------------------------------------------------------*/
void McpGpio::PinMode()
{
    this->p_trace->println(trace_INFO_MSG, "<<mcpgpio>> pin mode changed");
}

