/*
 ESP8266 Blink by Simon Peter
 Blink the blue LED on the ESP-01 module
 This example code is in the public domain
 
 The blue LED on the ESP-01 module is connected to GPIO1 
 (which is also the TXD pin; so we cannot use Serial.print() at the same time)
 
 Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200);             // open serial port, set the baud rate as 115200 bps
}

// the loop function runs over and over again forever
void loop() 
{
  int val;
  float moist_f = 0.0f;
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because 
                                    // it is acive low on the ESP-01)
  val = analogRead(0);              //connect sensor to Analog 0  
  Serial.print("Raw Data Value: ");                
  Serial.println(val);              // print the value to serial port 
  Serial.print("Level of moisture: ");
  if(val < 300)
  {
    Serial.println("defect?");
  }
  else if(val < 500)
  {
    Serial.println("very wet"); 
  }          
  else if(val < 650)
  {
    Serial.println("wet"); 
  }                    
  else
  {
    Serial.println("dry");
  }
  Serial.print("Moisture in %: ");
  moist_f = 100.0 - (((val - 450.0)/(860.0f - 450.0f))*100.0f);
  Serial.println(moist_f);
  
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(1000);                      // Wait for two seconds (to demonstrate the active low LED)
}
