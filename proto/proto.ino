#include <QueueArray.h>
#include <CapacitiveSensor.h>

//#define SIZE 8 
#define SIZE 2
#define SENSITIVITY 3
#define PRECISION 50
#define interrupt_pin 11
#define SERIAL_SIZE 20

// COMMENTAIRE INUTILE
struct deplacement
{
	char type; //depose: d pose: p
	int x;
	int y;
};


enum state_enum{
   LISTEN,
   SLEEP
};

long moyenne=0;
long releve=0;
int index=0;
char inChar;
char inData[SERIAL_SIZE] = "";

state_enum state = SLEEP;
 
 
CapacitiveSensor   cs_4_2 = CapacitiveSensor(5,7);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired

//int inp[SIZE] = { 16, 17,19,20,21,22,23,24 };
int inp[SIZE] = { 46, 42};
//int outp[SIZE] = { 25,26,27,28,29,30,31,32};
int outp[SIZE] = { 38,34};
//int tab[SIZE][SIZE] = {{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}};
int tab[SIZE][SIZE] = {{1,1},{1,1}};
int oldtab[SIZE][SIZE] = {{1,1},{1,1}};
//int oldtab[SIZE][SIZE]  = {{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}};
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

      pinMode(interrupt_pin,OUTPUT);
       digitalWrite(interrupt_pin,LOW);
          cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor... 
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
    //Serial.println(inputString);
    // clear the string:
   
         if(inputString=="sleep\n")
        {state=SLEEP;
        }
        if(inputString=="listen\n")
        {state=LISTEN;
        }
        
     inputString = "";
    stringComplete = false;
  }

  switch(state){
       case LISTEN: 
         listen();
         break;
       case SLEEP:
         sleepfunction();
         break;
       
  }
  
    releve=cs_4_2.capacitiveSensor(30);
   if(releve > 2*moyenne)
   {
    cap_press();
   }


  
}
void sleepfunction(){
 //  Serial.println("sleep");  
}

void listen()
{
   //Serial.println("listen");
		for (int j = 0; j<SIZE; j++)
		{
			for (int h = 0; h<SIZE; h++)
			{

				digitalWrite(outp[j], LOW);

                                
				if (digitalRead(inp[h]) == LOW)
				{
					tab[h][j] = 0;

				}
				else
				{
					tab[h][j] = 1;

				}

                                 
				digitalWrite(outp[j], HIGH);
                          }
                  }
                  
	
   if(comptab(oldtab, tab))
  {
 
  printtypecoup(oldtab, tab);
  copytab(oldtab, tab);
  }
  
         
         
}


//Depile la liste de coups dans serial
void poparray()
{
  
  deplacement a={'l',1,1};
  while (!array.isEmpty ())
  {
    a=array.pop();   
Serial.print (a.type);
Serial.print (a.x);
Serial.print(a.y);
Serial.print(" ");
}
    
   state=SLEEP;
   
                             
                               
}



void cap_press()

{
 // digitalWrite(interrupt_pin,LOW);
  //Serial.println("Touche");
  
  poparray();

}


void printtab(int tab[SIZE][SIZE])
//Affiche un tableau en argument
{
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
                                        temp.type='p';
                                      
				}
				else
				{       
      					//Serial.println("Piece levee en:");
                                        temp.type='d';

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
