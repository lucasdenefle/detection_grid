#include <CapacitiveSensor.h>

#define interrupt_pin 11
#define PRECISION 15
#define PRECISION_MOYENNE 3
long moyenne_array[PRECISION];
CapacitiveSensor   cs_4_2 = CapacitiveSensor(5,7);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired

// To add more sensors...
//CapacitiveSensor   cs_4_6 = CapacitiveSensor(4,6);        // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil
//CapacitiveSensor   cs_4_8 = CapacitiveSensor(4,8);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil

void setup()                    
{ 
   cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor... 
 Serial.begin(9600);


  init_array();


}

void loop()                    
{


shift_array();
moyenne_array[PRECISION-1]=cs_4_2.capacitiveSensor(30);

if(getreleve() > 2* getmoyenne())
{Serial.println("Touché");
 delay(1000);
 init_array();
}
//delay(1000);

}


//Fait la moyenne sur toutes l'array de relevés
long getmoyenne()
{
  int i=0;
  long moyenne=0;
  for(i=0;i<PRECISION-PRECISION_MOYENNE;i++)
  {
    moyenne+=moyenne_array[i];
    
  }
  moyenne=moyenne/(PRECISION-PRECISION_MOYENNE);
  Serial.print("MOYENNE : ");
  Serial.println(moyenne);
  return moyenne;
}


//Fait une moyenne des 5 derniers relevés
long getreleve()
{
  int i=0;
  long releve=0;
  for(i=PRECISION-PRECISION_MOYENNE;i<PRECISION;i++)
  {
    releve=moyenne_array[i]+releve;
  }
  releve=releve/5;
  Serial.print("RELEVE : ");
  Serial.println(releve);
  return releve;
}


//Décale l'array de moyennes 
void shift_array()
{
  int i=0;
  for(i=0;i<PRECISION-1;i++)
  {
    moyenne_array[i]=moyenne_array[i+1];
  }
  
}

void init_array()
{
   for (int i=0;i<PRECISION;i++)
  { 
  moyenne_array[i]=cs_4_2.capacitiveSensor(30);
  delay(5);
  }
}
