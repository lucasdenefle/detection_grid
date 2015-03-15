#include <CapacitiveSensor.h>

#define interrupt_pin 11


long moyenne=0;
CapacitiveSensor   cs_4_2 = CapacitiveSensor(5,7);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired

// To add more sensors...
//CapacitiveSensor   cs_4_6 = CapacitiveSensor(4,6);        // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil
//CapacitiveSensor   cs_4_8 = CapacitiveSensor(4,8);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil

void setup()                    
{ 
   cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor... 
 Serial.begin(9600);

 attachInterrupt(0, cap_press, RISING);
 pinMode(interrupt_pin,OUTPUT);
 digitalWrite(interrupt_pin,LOW);

 for (int i=0;i<10;i++)
{ 
  moyenne=cs_4_2.capacitiveSensor(30)+moyenne;
  delay(5);
}
Serial.println(moyenne);

}

void loop()                    
{
long releve=cs_4_2.capacitiveSensor(30);

if(releve > moyenne)
{digitalWrite(interrupt_pin,HIGH);
}

delay(100);

}


void cap_press()
{
  digitalWrite(interrupt_pin,LOW);
  Serial.println("Touche");
}
