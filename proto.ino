#include <QueueArray.h>
#include <CapacitiveSensor.h>

#define SIZE 8 
//#define SIZE 2
#define SENSITIVITY 3
#define PRECISION 50
#define interrupt_pin 11


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

state_enum state = LISTEN;
 String inputString = "";         // a string to hold incoming data
 
 
CapacitiveSensor   cs_4_2 = CapacitiveSensor(5,7);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired

//int inp[SIZE] = { 16, 17,19,20,21,22,23,24 };
int inp[SIZE] = { 46, 42};
//int outp[SIZE] = { 25,26,27,28,29,30,31,32};
int outp[SIZE] = { 38,34};
//int tab[SIZE][SIZE] = {{1,1},{1,1},{1,1},{1,1},{1,1},{1,1},{1,1},{1,1}};
int tab[SIZE][SIZE] = {{1,1},{1,1}};
int oldtab[SIZE][SIZE] = {{1,1},{1,1}};
//int oldtab[SIZE][SIZE] = {{1,1},{1,1},{1,1},{1,1},{1,1},{1,1},{1,1},{1,1}};
QueueArray<deplacement> array;


void setup()
{
       
       for (int i = 0; i<SIZE; i++)
	{
		pinMode(inp[i], INPUT_PULLUP);
		pinMode(outp[i], OUTPUT);
		digitalWrite(outp[i], HIGH);
	}

       Serial.println("Entrée setup");
      // attachInterrupt(0, cap_press, RISING);
       pinMode(interrupt_pin,OUTPUT);
       digitalWrite(interrupt_pin,LOW);
          cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor... 
	Serial.begin(9600);
        deplacement d={'c',1,1};
        array.push(d);
        array.push(d);
        array.push(d);
        
        
         for (int i=0;i<PRECISION;i++)
        { 
          moyenne=cs_4_2.capacitiveSensor(30)+moyenne;
          delay(5);
         }
         moyenne=moyenne/(PRECISION/3);
          Serial.println(moyenne);

}


void loop()
{
  
  switch(state){
       case LISTEN: 
         listen();
         break;
       case SLEEP:
         break;
       
  }
 // listen();
  
}

void serialEvent()
{
  while(Serial.available()){
     char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    if(inputString=="l")
        { state=LISTEN;
        inputString="";
        }
    if(inputString=="s")
         state=SLEEP;
         inputString="";
    }
}


void listen()
{
     Serial.println("Entrée listen");
	while (!comptab(oldtab, tab))
	{
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

                                releve=cs_4_2.capacitiveSensor(30);
                            //     Serial.println(releve);
                                  if(releve > 200)
                                  {//digitalWrite(interrupt_pin,HIGH);
                                  cap_press();
                                 }
                                 
                                delay(100);
                                 
        
			}


		}
	}
	printtypecoup(oldtab, tab);
	//printtab(tab);
	copytab(oldtab, tab);

        //Check si le senseur capacitif est activé
         
         
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
}
}



void cap_press()

{
 // digitalWrite(interrupt_pin,LOW);
  Serial.println("Touche");
  
  poparray();
  printtab(tab);
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
					Serial.println("Piece posee en ");
                                        temp.type='p';
                                      
				}
				else
				{       
      					Serial.println("Piece levee en:");
                                        temp.type='d';

				}
                                temp.x=h;
                                temp.y=j;
                                array.push(temp);

				Serial.print("x = ");
				Serial.println(h);
				Serial.print("y = ");
				Serial.println(j);
				Serial.println();
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
