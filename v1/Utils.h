/*****************************************************************************************
* FILENAME :        Utils.h
*
* DESCRIPTION :
*       Class header for utility methods
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
#ifndef UTILS_H_
#define UTILS_H_

/****************************************************************************************/
/* Imported header files: */

#include <ESP8266WiFi.h>         
#include <PubSubClient.h>

/****************************************************************************************/
/* Global constant defines: */

/****************************************************************************************/
/* Global function like macro defines (to be avoided): */

/****************************************************************************************/
/* Global type definitions (enum, struct, union): */

/****************************************************************************************/
/* Class definition: */
class Utils
{
    public:
        /********************************************************************************/
        /* Public data definitions */

        /********************************************************************************/
        /* Public function definitions: */
        Utils();
        static char* FloatToString(float f, int p, char* pBuffer);
        static char* IntegerToDecString(uint32_t i, char* pBuffer);
        static char* BuildSendTopic(const char *dev_p, const char *channel_p, 
                                      const char *topic_p, char *buffer_p);
        static char* BuildSendTopic(const char *dev_p, 
                                      const char *topic_p, char *buffer_p);  
        static char* BuildReceiveTopic(const char *dev_p, const char *channel_p, 
                                      const char *topic_p, char *buffer_p); 
        virtual
        ~Utils();
    private:
        /********************************************************************************/
        /* Private data definitions */      
        
        /********************************************************************************/
        /* Private function definitions: */

    protected:
        /********************************************************************************/
        /* Protected data definitions */

        /********************************************************************************/
        /* Protected function definitions: */

};

#endif /* UTILS_H_ */

