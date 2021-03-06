/*****************************************************************************************
* FILENAME :        EspGpio.h
*
* DESCRIPTION :
*       Class header for ESP Gpio class
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
#ifndef ESPGPIO_H_
#define ESPGPIO_H_

/****************************************************************************************/
/* Imported header files: */

#include "GpioDevice.h"
#include "Trace.h"

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class EspGpio : public GpioDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        EspGpio(Trace *p_trace);
        EspGpio(Trace *p_trace, uint8_t pin_u8);
        EspGpio(Trace *p_trace, uint8_t pin_u8, uint8_t dir_u8);

        void PinMode(uint8_t dir_u8);
        void DigitalWrite(uint8_t state_u8);
        uint8_t DigitalRead(void);
        virtual ~EspGpio();
    private:
        /********************************************************************************/
        /* Private data definitions */ 
        //static int mcp;
        //static bool mcpInitialized_bol;
        uint8_t stat_u8;

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

#endif /* ESPGPIO_H_ */
