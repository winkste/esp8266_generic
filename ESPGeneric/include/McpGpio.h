/*****************************************************************************************
* FILENAME :        McpGpio.h
*
* DESCRIPTION :
*       Class header for MCP Gpio class
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
#ifndef MCPGPIO_H_
#define MCPGPIO_H_

/****************************************************************************************/
/* Imported header files: */

#include "GpioDevice.h"
#include "Trace.h"

#include "Adafruit_MCP23017.h"

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class McpGpio : public GpioDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        McpGpio(Trace *p_trace);
        McpGpio(Trace *p_trace, uint8_t pin_u8);
        McpGpio(Trace *p_trace, uint8_t pin_u8, uint8_t dir_u8);
        McpGpio(Trace *p_trace, uint8_t pin_u8, uint8_t dir_u8, GpioDevice *nReset_p);

        void PinMode(uint8_t dir_u8);
        void DigitalWrite(uint8_t state_u8);
        uint8_t DigitalRead(void);
        void AnalogWrite(uint16_t value_u16);
        uint16_t AnalogRead(void);

        virtual ~McpGpio();
    private:
        /********************************************************************************/
        /* Private data definitions */ 
        static Adafruit_MCP23017 mcp;
        static uint8_t mcpAddr_u8;
        static bool mcpInitialized_bol;
        uint8_t stat_u8;
        uint16_t    value_u16;
        GpioDevice *nReset_p;

        /********************************************************************************/
        /* Private function definitions: */
        void PrintPinStat();
        void PinMode();
        void Initialize();

        /********************************************************************************/
        /* Protected data definitions */

        /********************************************************************************/
        /* Protected function definitions: */

};

#endif /* MCPGPIO_H_ */
