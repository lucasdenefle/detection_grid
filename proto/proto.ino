#include <QueueArray.h>
#include <CapacitiveSensor.h>

#define SIZE 8 
//#define SIZE 2
#define SENSITIVITY 3
#define PRECISION 50
#define interrupt_pin 11
#define SERIAL_SIZE 20

// COMMENTAIRE INUTILE
struct deplacement
{
  char type; //up: u down: d
  int x;
  int y;
};


enum state_enum{
   LISTEN,
   SLEEP,
   RESET,
   WAITING,
   VALID,
   INVALID,
   UPDATE
};

long moyenne=0;
long releve=0;
int index=0;
char inChar;
char inData[SERIAL_SIZE] = "";

state_enum state = LISTEN;
 
 
CapacitiveSensor   cs_4_2 = CapacitiveSensor(5,7);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired

int inp[SIZE] = { 41,45,49,53,51,47,43,39 };
//int inp[SIZE] = { 46, 42};
int outp[SIZE] = { 35,31,27,23,25,29,33,37};
//int outp[SIZE] = { 38,34};
int tab[SIZE][SIZE] = {{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}};
//int tab[SIZE][SIZE] = {{1,1},{1,1}};
//int oldtab[SIZE][SIZE] = {{1,1},{1,1}};
int goodtab[SIZE][SIZE];
int oldtab[SIZE][SIZE]  = {{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}};
QueueArray<deplacement> array;



String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


void setup()
{
       
       for (int i = 0; i<SIZE; i++)
  {
    pinMode(inp[i], INPUT_PULLUP);
    pinMode(outp[i], OUTPUT);
    digitalWrite(outp[i], HIGH);
  }

     //  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor... 
  Serial.begin(9600);
         // reserve 200 bytes for the inputString:
      inputString.reserve(200);
        
      for (int i=0;i<PRECISION;i++)
        { 
          moyenne=cs_4_2.capacitiveSensor(30)+moyenne;
          delay(5);
         }
       moyenne=moyenne/(PRECISION/3);
      
}


void loop()
{
    if (stringComplete) {
   // Serial.println(inputString);
    // clear the string:
   
         if(inputString=="sleep\n")
        {state=SLEEP;
        }
        if(inputString=="listen\n")
        {state=LISTEN;
        }
        if(inputString=="valid\n" && state==WAITING)
        {state=VALID;
        }
        if(inputString=="invalid\n"  && state==WAITING)
        {state=INVALID;
        }
        if(inputString=="update\n")
        {state=UPDATE;
        }
        if(inputString=="waiting\n")
        {state=WAITING;
        }
        
        
     inputString = "";
     stringComplete = false;
  }

  switch(state){
       case UPDATE:
         //Scan la matrice et remplit goodtab
         //Serial.println("update");
         scantab(goodtab);
         break;
       case LISTEN: 
         //Serial.println("listen");
         listen();
         releve=cs_4_2.capacitiveSensor(30);
         if(releve > 2*moyenne){
         cap_press();}
         
         break;
       case SLEEP:
        //Serial.println("sleep");
         sleepfunction();
         break;
       case VALID:
         //valide le move 
         //Serial.println("valid");
         copytab(goodtab,tab);
         state=SLEEP;
         break;
       case INVALID:
       //  Serial.println("invalid");
         //attend que le tableau revienne à son dernier état correct
         if(!isValidTab()){
           state=LISTEN;
         }
         break;
       case WAITING:
         // Serial.println("update");
         //attend que le rasp communique "valid" ou "invalid"
         break;
         
       
  }
  
   


  
}
void sleepfunction(){
}

void listen()
{

    scantab(tab); 
    
   if(comptab(oldtab, tab))
  {
    printtab(tab);
    printtypecoup(oldtab, tab);
    copytab(oldtab, tab);
  }
  
  
         
         
}


void scantab(int tab[SIZE][SIZE])
{
  
      for (int j = 0; j<SIZE; j++)
    {
      for (int h = 0; h<SIZE; h++)
      {

        digitalWrite(outp[j], LOW);

                                
        if (digitalRead(inp[h]) == LOW)
        {
          tab[h][j] = 0;
         // Serial.println("XXXXXXXXXXXXXX");

        }
        else
        {
          tab[h][j] = 1;

        }

                                 
        digitalWrite(outp[j], HIGH);
                          }
                  }

}

//Depile la liste de coups dans serial
void poparray()
{
  
  deplacement a={'l',1,1};
  char x;
  while (!array.isEmpty ())
  {
    a=array.pop();   
Serial.print (a.type);
x=numbertoletter(a.x);
Serial.print (x);
Serial.print( a.y+1);
Serial.print(" ");
}
   state=WAITING;
    Serial.println();   

delay(100);    
                               
}

char numbertoletter(int a){
    return 'a'+a;
}

void cap_press()

{
  poparray();
}


void printtab(int tab[SIZE][SIZE])
//Affiche un tableau en argument
{
  Serial.println("printtab");
  for (int j = 0; j<SIZE; j++)
  {
    for (int h = 0; h<SIZE; h++)
    {
      Serial.print(tab[h][j]);
    }
    Serial.println();
  }

  Serial.println();
}



void printtypecoup(int tab1[SIZE][SIZE], int tab2[SIZE][SIZE])
//Ajoute un coup à la file
{
        deplacement temp;                 
       
  for (int j = 0; j<SIZE; j++)
  {
    for (int h = 0; h<SIZE; h++)
    {
      if (tab1[h][j] != tab2[h][j])
      {
        
                                if (tab1[h][j])
        {
          //Serial.println("Piece posee en ");
                                        temp.type='d';
                                      
        }
        else
        {       
                //Serial.println("Piece levee en:");
                                        temp.type='u';

        }
                                temp.x=h;
                                temp.y=j;
                                array.push(temp);
/*
        Serial.print("x = ");
        Serial.println(h);
        Serial.print("y = ");
        Serial.println(j);
        Serial.println();*/
      }

    }

  }


}

void copytab(int tab1[SIZE][SIZE], int tab2[SIZE][SIZE])
// Copie tab1 et tab2
{
  for (int j = 0; j<SIZE; j++)
  {
    for (int h = 0; h<SIZE; h++)
    {
      tab1[h][j] = tab2[h][j];


    }

  }
}


boolean comptab(int tab1[SIZE][SIZE], int tab2[SIZE][SIZE])
//retourne 1 en cas de non correspondance des tableaux
//0 en cas de correspondance
{
  for (int j = 0; j<SIZE; j++)
  {
    for (int h = 0; h<SIZE; h++)
    {
      if (tab1[h][j] != tab2[h][j])
      {
        return 1;
      }

    }

  }
  return 0;

}


boolean isValidTab()
{
  int temptab[SIZE][SIZE];
  scantab(temptab);
  if(comptab(temptab,goodtab)){
       //Tableaux différents
       return false;
   }
  else{ //Tableaux identiques
      return true;
    }
  
}



void serialEvent() {
  //Serial.println("serialEvent");
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
        stringComplete = true;
    }
  }
}
