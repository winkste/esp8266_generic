/*****************************************************************************************
* FILENAME :        DeviceFactory.h
*
* DESCRIPTION :
*       Class header for DeviceFactory
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
#ifndef DEVICEFACTORY_H_
#define DEVICEFACTORY_H_

/****************************************************************************************/
/* Imported header files: */

#include "MqttDevice.h"
#include "Trace.h"
#include "PubSubClient.h"
//#include "DhtSensor.h"
#include "LinkedList.h"

#include "SingleRelay.h"

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class DeviceFactory
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        DeviceFactory(Trace *p_trace);
        LinkedList<MqttDevice*> * GenerateDevice(uint8_t type_u8);
        virtual
        ~DeviceFactory();
    private:
        /********************************************************************************/
        /* Private data definitions */
        Trace * trace_p;
        /********************************************************************************/
        /* Private function definitions: */
    protected:
        /********************************************************************************/
        /* Protected data definitions */

        /********************************************************************************/
        /* Protected function definitions: */

};

#endif /* DEVICEFACTORY_H_ */
