#include <QueueArray.h>
#include <CapacitiveSensor.h>

#define SIZE 8
//#define SIZE 2
#define SENSITIVITY 3
#define PRECISION 30
#define PRECISION_MOYENNE 5
#define interrupt_pin 11
#define SERIAL_SIZE 20

// COMMENTAIRE INUTILE
struct deplacement
{
  char type; //up: u down: d
  int x;
  int y;
};


enum state_enum {
  LISTEN,
  SLEEP,
  RESET,
  WAITING,
  VALID,
  INVALID,
  UPDATE,
  TEST,
  INIT,
  TEST_LED,
  TEST_CAPT,
  TEST_RASP
};


long moyenne_array[PRECISION];
long releve = 0;
int index = 0;
char inChar;
char inData[SERIAL_SIZE] = "";
int player = 1;

state_enum state = SLEEP;


CapacitiveSensor   cs_4_2 = CapacitiveSensor(52, 48);       // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired

int inp[SIZE] = { 41, 45, 49, 53, 51, 47, 43, 39 };
int LED[2] = {46, 44};
//int inp[SIZE] = { 46, 42};
int outp[SIZE] = { 35, 31, 27, 23, 25, 29, 33, 37};
//int outp[SIZE] = { 38,34};
int tab[SIZE][SIZE] = {{1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}};
//int tab[SIZE][SIZE] = {{1,1},{1,1}};
//int oldtab[SIZE][SIZE] = {{1,1},{1,1}};
int goodtab[SIZE][SIZE];
int oldtab[SIZE][SIZE]  = {{1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}};
QueueArray<deplacement> array;



String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete


void setup()
{

  Serial.begin(9600);

  //Declaration OUTPUT et INPUT de la matrice
  for (int i = 0; i < SIZE; i++)
  {
    pinMode(inp[i], INPUT_PULLUP);
    pinMode(outp[i], OUTPUT);
    digitalWrite(outp[i], HIGH);
  }

  //Declaration PIN des LEDs
  for (int i = 0; i < 2; i++)
  {
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], LOW);
  }


  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor...
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  if (state != TEST) {
    init_array();
  }
}


void loop()
{
  if (stringComplete) {
    // Serial.println(inputString);
    // clear the string:

    if (inputString == "sleep$")
    { state = SLEEP;
    }
    if (inputString == "listen 1$")
    {
      player = 0;
      turn_off_all_leds();
      turn_on_led();
      state = LISTEN;
    }
    if (inputString == "listen 2$")
    {
      player = 1;
      turn_off_all_leds();
      turn_on_led();
      state = LISTEN;
    }
    if (inputString == "valid$")
    { state = VALID;
    }
    if (inputString == "invalid$")
    { state = INVALID;
    }
    if (inputString == "update$")
    { state = UPDATE;
    }
    if (inputString == "waiting$")
    { state = WAITING;
    }
    if (inputString == "test$")
    { state = TEST;
    }
    if (inputString == "init$")
    { state = INIT;
    }
    if (inputString == "test_led$")
    { state = TEST_LED;
     
    }
    if (inputString == "test_capt$")
    { state = TEST_CAPT;
    }
    
    if (inputString == "test_rasp$")
    { state = TEST_RASP;
    }


    inputString = "";
    stringComplete = false;
  }

  switch (state) {
  case UPDATE:
    //Scan la matrice et remplit goodtab
    //Serial.println("update");
    scantab(goodtab);
    break;

  case INIT:
    scantab(goodtab);
    scantab(oldtab);
    scantab(tab);
    //printtab(goodtab);
    state = SLEEP;
    break;
  case LISTEN:
    //Serial.println("listen");
    listen();
    //CAPTEUR TEST
    if (getreleve() > 2 * getmoyenne() && !array.isEmpty()) {
      //Serial.println("TOUCHE");
      pop_array();
    }
    break;
  case SLEEP:
    //Serial.println("sleep");

    break;
  case VALID:
    //valide le move
    //Serial.println("valid");
    scantab(goodtab);
    turn_off_all_leds();
    state = SLEEP;
    break;
  case INVALID:
    //  Serial.println("invalid");
    //attend que le tableau revienne à son dernier état correct


    if (!isValidTab()) {
      //    Serial.println("TAB VALIDE");
      delay(1000);
      state = LISTEN;
      scantab(oldtab);
      scantab(tab);
    }
    else { //blink
      blink_led();
    }
    break;
  case WAITING:
    // Serial.println("update");
    //attend que le rasp communique "valid" ou "invalid"
    break;
  case TEST:
    listen_test();
    //delay(500);

    break;
  case TEST_RASP:
  // Serial.println("Coucou");
   //delay(1000);
   break;
  case TEST_LED:
    blink_led();
    turn_off_all_leds();
    delay(1000);
    switch_player();
    break;
  case TEST_CAPT:
    if (getreleve() > 2 * getmoyenne()) {
      Serial.println("TOUCHE");
    }
    break;
  }

  //capteur calibration
  shift_array();
  moyenne_array[PRECISION - 1] = cs_4_2.capacitiveSensor(30);




}


void listen()
{
  scantab(tab);
  if (comptab(oldtab, tab))
  { printtypecoup(oldtab, tab);
    copytab(oldtab, tab);
    delay(500);
  }
}


void listen_test()
{

  scantab(tab);
  if (comptab(oldtab, tab))
  {
    printtab(tab);
    printtypecoup(oldtab, tab);
    copytab(oldtab, tab);
  }
}

void scantab(int tab[SIZE][SIZE])
{

  for (int j = 0; j < SIZE; j++)
  { for (int h = 0; h < SIZE; h++)
    {
      digitalWrite(outp[j], LOW);
      if (digitalRead(inp[h]) == LOW)
      { tab[h][j] = 0;
      }
      else
      { tab[h][j] = 1;
      }
      digitalWrite(outp[j], HIGH);
    }
  }

}




void printtab(int tab[SIZE][SIZE])
//Affiche un tableau en argument
{
  Serial.println("printtab");
  for (int j = 0; j < SIZE; j++)
  {
    for (int h = 0; h < SIZE; h++)
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

  for (int j = 0; j < SIZE; j++)
  {
    for (int h = 0; h < SIZE; h++)
    {
      if (tab1[h][j] != tab2[h][j])
      {

        if (tab1[h][j])
        {
          //Serial.println("AJOUT Piece posee en ");
          temp.type = 'd';
        }
        else
        {
          //Serial.println("AJOUT Piece levee en:");
          temp.type = 'u';

        }
        temp.x = h;
        temp.y = j;
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
  for (int j = 0; j < SIZE; j++)
  {
    for (int h = 0; h < SIZE; h++)
    {
      tab1[h][j] = tab2[h][j];


    }

  }
}


boolean comptab(int tab1[SIZE][SIZE], int tab2[SIZE][SIZE])
//retourne 1 en cas de non correspondance des tableaux
//0 en cas de correspondance
{
  for (int j = 0; j < SIZE; j++)
  {
    for (int h = 0; h < SIZE; h++)
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
// printtab(temptab);
  if (comptab(temptab, goodtab)) {
    //Tableaux différents
    return true;
  }
  else { //Tableaux identiques
    return false;
  }

  delay(500);

}


//SERIAL
/****************************************************/
//S'execute entre chaque tour de loop
void serialEvent() {
  //Serial.println("serialEvent");
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '$') {
      stringComplete = true;
    }
  }
}

//Depile la liste de coups dans serial
void pop_array()
{

  deplacement a = {'l', 1, 1};
  char x;
  while (!array.isEmpty ())
  {
    a = array.pop();
    Serial.print (a.type);
    x = numbertoletter(a.x);
    Serial.print (x);
    Serial.print( 8 - (a.y));
    Serial.print(" ");
  }
  state = WAITING;
  Serial.println();

  delay(100);

}

char numbertoletter(int a) {
  return 'a' + (7 - a);
}

/****************************************************/



//CAPTEUR CAPACITIF

/****************************************************/
//Fait la moyenne sur toutes l'array de relevés
long getmoyenne()
{
  int i = 0;
  long moyenne = 0;
  for (i = 0; i < PRECISION - PRECISION_MOYENNE; i++)
  {
    moyenne += moyenne_array[i];

  }
  moyenne = moyenne / (PRECISION - PRECISION_MOYENNE);
  //Serial.print("MOYENNE : ");
  //Serial.println(moyenne);
  return moyenne;
}


//Fait une moyenne des 5 derniers relevés
long getreleve()
{
  int i = 0;
  long releve = 0;
  for (i = PRECISION - PRECISION_MOYENNE; i < PRECISION; i++)
  {
    releve = moyenne_array[i] + releve;
  }
  releve = releve / 5;
  //Serial.print("RELEVE : ");
  //Serial.println(releve);
  return releve;
}



//Décale l'array de moyennes
void shift_array()
{
  int i = 0;
  for (i = 0; i < PRECISION - 1; i++)
  {
    moyenne_array[i] = moyenne_array[i + 1];
  }
}

//Nettoie l'array
void init_array()
{
  for (int i = 0; i < PRECISION; i++)
  {
    moyenne_array[i] = cs_4_2.capacitiveSensor(30);
    delay(5);
  }
}


/****************************************************/




//LEDs

/****************************************************/
void turn_off_all_leds()
{
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(LED[i], LOW);
  }
}

void turn_on_led()
{
  digitalWrite(LED[player], HIGH);
}

void turn_off_led()
{
  digitalWrite(LED[player], LOW);
}

void blink_led()
{

  digitalWrite(LED[player], LOW);
  delay(500);
  digitalWrite(LED[player], HIGH);
  delay(500);

}

void switch_player()
{
  if (player == 1)
  {
    player = 0;
  }
  else {player = 1;}
}
/****************************************************/
