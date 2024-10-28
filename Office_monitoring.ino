//Including Libraries
#include <Servo.h>
#include <Keypad.h>

Servo myservo; //Naming Servo

//Blynk
#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
char auth[] = "64f0a49d9ea74cf2a4bdab00a81bcc2b"; //Your Blynk AUTH code
char ssid[] = "Ghost";                            //wifi connection name for ESP8266
char pass[] = "ghost0210";                       //wifi connection password for ESP8266

#define EspSerial Serial1
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

#define Password_Lenght 7 // Give enough room for six chars + NULL char

char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
char Master[Password_Lenght] = "123456";
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

//Configuring Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

bool door = true;

byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); //initialize an instance of class NewKeypad

int pir1 = 11;//Assign PIR 1 to pin 11 For Right
int pir2 = 12;//Assign PIR 2 to pin 12 For Front
int pir3 = 13;//Assign PIR 3 to pin 13 For Left

int valpir1;
int valpir2;
int valpir3;

int commandpin = 50;
int command;

void setup()
{
  Serial.begin(9600);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

  pinMode(pir1, INPUT);
  pinMode(pir2, INPUT);
  pinMode(pir3, INPUT);
  
  pinMode(commandpin, INPUT);
  
  myservo.attach(10);
  myservo.write(0);
}

void loop()
{
  Blynk.run();
  command = digitalRead(commandpin);
  if (command == HIGH)
  {
    if (door == 1) 
    {
      valpir1 = digitalRead(pir1);
      valpir2 = digitalRead(pir2);
      valpir3 = digitalRead(pir3);

      if (valpir1 == HIGH)
      {
        digitalWrite(valpir2, LOW);
        digitalWrite(valpir3, LOW);
        myservo.write(0); //Right rotation angle
        Blynk.notify("Right");
      }
      else if (valpir2 == HIGH)
      {
        digitalWrite(valpir1, LOW);
        digitalWrite(valpir3, LOW);
        myservo.write(90); //Front rotation angle
        Blynk.notify("Front");
      }
      else if (valpir3 == HIGH)
      {
        digitalWrite(valpir1, LOW);
        digitalWrite(valpir2, LOW);
        myservo.write(170); //Left rotation angle
        Blynk.notify("Left");
      }
    }
    else
    {
      digitalWrite(valpir1, LOW);
      digitalWrite(valpir2, LOW);
      digitalWrite(valpir3, LOW);
    }

    if (door == 0)
    {
      customKey = customKeypad.getKey();
      if (customKey == '#')

      {
        Blynk.virtualWrite(V0, "DOOR LOCKED");
        door = 1;
      }
    }

    else Open();
    int pos = myservo.read();
    Serial.println(pos);
  }
}
void clearData()
{
  while (data_count != 0)
  { // This can be used for any array size,
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}

void Open()
{
  customKey = customKeypad.getKey();
  if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
  {
    Data[data_count] = customKey; // store char into data array
    data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
  }

  if (data_count == Password_Lenght - 1) // if the array index is equal to the number of expected chars, compare data to master
  {
    if (!strcmp(Data, Master)) // equal to (strcmp(Data, Master) == 0)
    {
      door = 0;
      Blynk.virtualWrite(V0, "DOOR OPEN");
    }
    else
    {
      Blynk.virtualWrite(V0, "Wrong PASS");
      Blynk.notify("Wrong PASS");
      door = 1;
    }
    clearData();
  }
}
