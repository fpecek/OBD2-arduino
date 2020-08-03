//Define Serial Ports so I remember which is which
#include <SoftwareSerial.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

//Create an instance of the new soft serial library to control the serial LCD
//Note, digital pin 3 of the Arduino should be connected to Rx of the serial LCD.

// SoftwareSerial lcd(2,3);

/*-----( Declare Constants )-----*/
#define I2C_ADDR    0x3F  // Define I2C Address for the PCF8574T 
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

#define  LED_OFF  1
#define  LED_ON  0

LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

//Set up ring buffer
char rxData[20];
char rxIndex=0;

void setup() 
{
  lcd.begin (20,4);  // initialize the lcd 
   
  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
  lcd.backlight();  //Backlight ON if under program control
  Serial.begin(9600);

  ODB_init();
}

void loop() 
{
  //Delete any data that may be left over in the serial port.
  Serial.flush();
  
  //Move the serial cursor to the position where we want the RPM data.
  //lcd.print(" "); 
  //lcd.write(254);
  //lcd.write(128 + 69);
  //lcd.print(getRPM()); //Print the int returned from getRPM
  
  Serial.flush();
  
  //lcd.print(" ");
  //lcd.write(254);
  //lcd.write(128 + 6);
  //lcd.print(getTemp()); //Print the int returned from getTemp
  //lcd.print(" C");
  
  lcd.setCursor(0, 0);
  lcd.write(254);
  lcd.write(128 + 6);
  lcd.print(getSpeed()); //Print the int returned from getTemp
  lcd.print(" km/h");

  Serial.flush();

  //lcd.setCursor(0, 1);
  //lcd.print(" ");
  //lcd.write(254);
  //lcd.write(128 + 6);
  //lcd.print(getVoltage()); //Print the int returned from getTemp
  //lcd.print(" V");

  lcd.setCursor(0, 1);
  //lcd.print(" ");
  lcd.write(254);
  lcd.write(128 + 6);
  lcd.print("Oil temp: ");
  lcd.print(getEngineOilTemp()); //Print the int returned from getTemp
  lcd.print(" C");
  
}

void ODB_init(void)
{
  //Wait for a little while before sending the reset command to the OBD-II-UART
  delay(2000);
  
  //Reset the OBD-II-UART
  Serial.print("ATZ\r");
  //Wait for a bit before starting to send commands after the reset.
  delay(2000);
  OBD_read();

  delay(2000);
  Serial.print("ATE0\r");
  OBD_read();
  delay(2000);
  
  Serial.flush();  
}

int getRPM(void)
{
  //Query the OBD-II-UART for the Vehicle rpm
  Serial.flush();
  Serial.print("010C\r");
  OBD_read();

  return ((strtol(&rxData[6],0,16)*256)+strtol(&rxData[9],0,16))/4;
}

int getVoltage(void) {
  //Query the OBD-II-UART for the Vehicle voltage
  Serial.flush();
  Serial.print("ATRV\r");
  OBD_read();

  return strtol(&rxData[6],0,16);
}

int getSpeed(void)
{
  //Query the OBD-II-UART for the Vehicle speed
  Serial.flush();
  Serial.print("010D\r");
  OBD_read();

  return strtol(&rxData[6],0,16);
}

int getTemp(void)
{
  //Query the OBD-II-UART for the Engine Coolant Temp
  Serial.flush();
  Serial.print("0105\r");
  OBD_read();

  return strtol(&rxData[6],0,16)-40;
}

int getEngineOilTemp(void)
{
  //Query the OBD-II-UART for the Engine Oil Temp
  Serial.flush();
  Serial.print("010F\r");
  OBD_read();

  return strtol(&rxData[6],0,16)-40;
}


void OBD_read(void)
{
  char c;
  do{
    if(Serial.available() > 0)
    {
      c = Serial.read();
      if((c != '>') && (c != '\r') && (c != '\n')) //Keep these out of our buffer
      {
        rxData[rxIndex++] = c; //Add received value to the buffer
      }  
     }     
  }while(c != '>'); //The ELM327 ends its response with this char so when we get it we exit out.
  
  rxData[rxIndex++] = '\0';//Converts the array into a string
  rxIndex=0; //Set this to 0 so next time we call the read we get a "clean buffer"
}
