/*****************************************************************************************
* FILENAME :        GpioDevice.h
*
* DESCRIPTION :
*       Abstract class for GPIOS
*
* NOTES :
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
*****************************************************************************************/
#ifndef GPIODEVICE_H_
#define GPIODEVICE_H_

/****************************************************************************************/
/* Imported header files: */
#include "Trace.h"

/****************************************************************************************/
/* Global constant defines: */
#define gpioDevice_LOW     (0u)
#define gpioDevice_HIGH    (1u)
#define gpioDevice_INPUT   (0u)
#define gpioDevice_OUTPUT  (1u)

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class GpioDevice
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        GpioDevice(Trace *p_trace);
        GpioDevice(Trace *p_trace, uint8_t pin_u8);
        GpioDevice(Trace *p_trace, uint8_t pin_u8, uint8_t dir_u8);
        virtual ~GpioDevice();
        //virtual void Initialize();
        virtual void PinMode(uint8_t dir_u8) = 0;
        virtual void DigitalWrite(uint8_t state_u8) = 0;
        virtual uint8_t DigitalRead(void) = 0;
        virtual void AnalogWrite(uint16_t value_u16) = 0;
        virtual uint16_t AnalogRead(void) = 0;
        
    protected:
        /********************************************************************************/
        /* Protected data definitions */
        Trace       *p_trace;
        uint8_t     pin_u8;
        uint8_t     dir_u8;
        uint16_t    value_u16;

        /********************************************************************************/
        /* Protected function definitions: */
        
};

#endif /* GPIODEVICE_H_ */
