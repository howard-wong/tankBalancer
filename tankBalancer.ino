#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
#define RELAY_0 12
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define HIGH_TEMP 26.5
#define LOW_TEMP 24.5

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // SET I2C Address

DeviceAddress t0 = { 0x28, 0xFF, 0x52, 0x6D, 0x61, 0x17, 0x04, 0xBE };
DeviceAddress t1 = { 0x28, 0xFF, 0xA8, 0x99, 0x53, 0x17, 0x04, 0x4B };
DeviceAddress t2 = { 0x28, 0xFF, 0xA8, 0x99, 0x53, 0x17, 0x04, 0x4C };

OneWire ds(ONE_WIRE_BUS);
DallasTemperature sensors(&ds);

struct CStat {
  bool relay[2]; 
  
} cstat;




void setup()
{
  
  Serial.begin(115200);
  Serial.println(F("FishTank Balancer v0.1"));

  lcd.begin(16,2);  // Initialinze LCD.
  lcd.backlight(); // makes Backligh ON. 
  lcd.clear();     // Clears LCD
  lcd.home();      // Sets cursor at Column 0 and Line 0  
  lcd.print(F("FishTank Balancer v0.1"));

  cstat.relay[0] = RELAY_OFF;
  
  pinMode(RELAY_0, OUTPUT);
  digitalWrite(RELAY_0, HIGH);

  //discoverOneWireDevices();

  sensors.begin(); 
  sensors.setResolution(t0, 12);
  sensors.setResolution(t1, 12);
  //sensors.setResolution(t2, 10);
}

void loop()
{
  sensors.requestTemperatures();
  //sensors.getTempCByIndex(0);
  float t0_temp, t1_temp;
  
  t0_temp = sensors.getTempC(t0);

  Serial.println();
  
  t1_temp = sensors.getTempC(t1);
  Serial.print("T0: ");
  Serial.print(t0_temp);
  Serial.print("     T1: ");
  Serial.print(t1_temp);

  Serial.println();  
  
  //if (t0_temp > 26.8)
 // digitalWrite(RELAY_0, LOW);
  
  
  lcd.clear();
  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("T0:");
  lcd.print(t0_temp);
  
  lcd.setCursor(0, 1);
  lcd.print("T1:");
  lcd.print(t1_temp); 

  lcd.setCursor(10, 0);
  lcd.print("R: "); 
  
  if (cstat.relay[0] == RELAY_OFF) {
    if (t0_temp > HIGH_TEMP)  {
       digitalWrite(RELAY_0, RELAY_ON);
       cstat.relay[0] = RELAY_ON;

    }     
  } else { // so it is on
    if (t0_temp < LOW_TEMP) {
       digitalWrite(RELAY_0, RELAY_OFF);
       cstat.relay[0] = RELAY_OFF;
    }    
  }
  
  if (cstat.relay[0] == RELAY_ON) {
    lcd.print("O");
  } else {
    lcd.print("X");
  }
  
  
  
  // for read resistance and write to control pwm
  // pinMode(controlPin, OUTPUT);   // sets the pin as output <---this line in setup()
  //val = analogRead(analogPin);   // read the input pin
  //analogWrite(controlPin, val);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  delay(1000);

}

void discoverOneWireDevices(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  
  Serial.print("Looking for 1-Wire devices...\n\r");
  while(ds.search(addr)) {
    Serial.print("\n\rFound \'1-Wire\' device with address:\n\r");
    for( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        Serial.print("CRC is not valid!\n");
        return;
    }
  }
  Serial.print("\n\r\n\rThat's it.\r\n");
  ds.reset_search();
  return;
}
