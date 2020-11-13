/// CANSensor
#include <MCP2515_1.h>
const uint8_t n = 10;
uint8_t buff[n][14], nF = 0, nF25 = 0;
MCP2515 mcp(0x00, 8, 9);
#include "CANSensor.h"

//MAX 3856
#include <Adafruit_MAX31856.h>
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(10);
int a, n_max=10, m=10, delta, cont ;
double old_media = 0, new_media = 0 , leitura = 0;


//LCD
#include <LiquidCrystal.h>
const int rs = 8, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


typedef union
{
 float number;
 uint8_t bytes[4];
} FLOATUNION_t;

void setup() {
  Serial.begin(2000000);
  mcp.confINT(0x04, 0x0F, 0x00);
  
  attachInterrupt(digitalPinToInterrupt(2), sendCAN, FALLING);

  //MAX31865
   maxthermo.begin();
   maxthermo.setThermocoupleType(MAX31856_TCTYPE_K);

  Serial.print("Thermocouple type: ");
  switch (maxthermo.getThermocoupleType() ) {
    case MAX31856_TCTYPE_B: Serial.println("B Type"); break;
    case MAX31856_TCTYPE_E: Serial.println("E Type"); break;
    case MAX31856_TCTYPE_J: Serial.println("J Type"); break;
    case MAX31856_TCTYPE_K: Serial.println("K Type"); break;
    case MAX31856_TCTYPE_N: Serial.println("N Type"); break;
    case MAX31856_TCTYPE_R: Serial.println("R Type"); break;
    case MAX31856_TCTYPE_S: Serial.println("S Type"); break;
    case MAX31856_TCTYPE_T: Serial.println("T Type"); break;
    case MAX31856_VMODE_G8: Serial.println("Voltage x8 Gain mode"); break;
    case MAX31856_VMODE_G32: Serial.println("Voltage x8 Gain mode"); break;
    default: Serial.println("Unknown"); break;
  }   

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Kaki Max31856");
  
}

void loop() {  
  float k_temp = 0;
  //Vetor com os bytes a serem enviados, max 8
  uint8_t TX[8] ={0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00};
                  
  for(uint8_t j = 0; j < 10; j++){
    k_temp = k_temp + maxthermo.readThermocoupleTemperature(); 
  }
   k_temp =  k_temp/10;
   
  FLOATUNION_t data;
  data.number = k_temp;
  TX[0] = data.bytes[0];
  TX[1] = data.bytes[1];
  TX[2] = data.bytes[2];
  TX[3] = data.bytes[3];

  putinBuff(0x36, 0x10, 4, TX);

  lcd.setCursor(0,1);
  lcd.print("Temp.oC:");
  lcd.setCursor(9,1);
  lcd.print(data.number);
  
}
