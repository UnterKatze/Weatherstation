/*
   With this program your can build your own weatherstation, which can
   monitor the temperature, the humidity and the airpressure and sends it
   to a 16x2 LCD display and to Blynk App via Wifi so you can check it on your
   smartphone.

   You need:
   - NodeMCU v1.0
   - 16x2 LCD display
   - BME280 sensor
   - smartphone with Blynk App


        %%%%%Wiring%%%%%

   LCD RS pin to D8 of NodeMCU
   LCD Enable pin to D7 of NodeMCU
   LCD D4 pin to D5 of NodeMCU
   LCD D5 pin to D4 of NodeMCU
   LCD D6 pin to D3 of NodeMCU
   LCD D7 pin to D0 of NodeMCU
   LCD R/W pin to ground of NodeMCU
   LCD VSS pin to ground of NodeMCU
   LCD VDD pin to 3.3V of NodeMCU
   LCD VO pin to ground of NodeMCU
   LCD A pin to D6 of NodeMCU
   LCD K pin to ground of NodeMCU

   BME280 SCL pin to D1 of NodeMCU
   BME280 SDA pin to D2 of NodeMCU
*/

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BLYNK_PRINT Serial
#define SEALEVELPRESSURE_HPA (1013.25) // airpressure at Sealevel for your Location

char auth[] = ""; // Your Blynk Authentification Token
char ssid[] = "";  // Your Wifi Name
char pass[] = ""; // Your Wifi Password

const int rs = D8, en = D7, d4 = D5, d5 = D4, d6 = D3, d7 = D0;
unsigned long delayTime = 6000; // delay im ms for sending Data to Display and Blynk App
int Temp1;
float Temp2;
int Hum1;
float Hum2;
int Press1;
float Press2;


LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Adafruit_BME280 bme;
BlynkTimer timer;

void setup()
{
  Blynk.begin(auth, ssid, pass);  // start Blynk
  lcd.begin(16, 2); // start LCD
  digitalWrite(D6, HIGH); // turn LCD backlight on
  bool status;
  status = bme.begin();
  if (!status)  // check if sensor ist connected
  {
    lcd.setCursor(0, 0);
    lcd.print("No Sensor found");
    while (true) {};
  }
  lcd.setCursor(0, 0);
  lcd.print("Temp");

  lcd.setCursor(5, 0);
  lcd.print("Hum");

  lcd.setCursor(9, 0);
  lcd.print("Press");

  timer.setInterval(1000L, myTimerEvent); // online Time in ms
}

void loop()
{
  Blynk.run();  // start Blynk
  timer.run();  // start Timer
  printValues();  // function for printing values on display and Blynk App
  delay(delayTime);
  if (millis() >= 86400000)
  {
    ESP.restart();  // restart device every 24h
  }
}

void printValues()
{
  Temp2 = bme.readTemperature();  // read temperature from sensor
  Temp1 = Temp2;
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.setCursor(0, 1);
  if (Temp1 >= 0)
  {
    lcd.print("+");
  }
  lcd.print(Temp1); // print temperature to LCD
  lcd.print("C");
  Blynk.virtualWrite(V1, Temp2);  // print temperature to virtual-pin 1 on Blynk App

  Hum2 = bme.readHumidity();  // read humidity from sensor
  if (Hum2 >= 100)  // I had problems with humidity spikes over 100%, lol
  {
    Hum2 = 99.99;
  }
  Hum1 = Hum2;
  lcd.setCursor(5, 1);
  lcd.print("    ");
  lcd.setCursor(5, 1);
  lcd.print(Hum1);  // print humidity to LCD
  lcd.print("%");
  Blynk.virtualWrite(V2, Hum2); // print humidity to virtual-pin 2 on Blynk App

  Press2 = bme.readPressure() / 100.0F; // read airpressure from sensor
  Press1 = Press2;
  lcd.setCursor(9, 1);
  lcd.print("       ");
  lcd.setCursor(9, 1);
  lcd.print(Press1);  // print airpressure to LCD
  lcd.print("hPa");
  Blynk.virtualWrite(V3, Press2); // print airpressure to virtual-pin 3 on Blynk App
}

void myTimerEvent()
{
  Blynk.virtualWrite(V5, millis() / 60000); // send the online-time to virtual-pin 5 on Blynk App (in minutes)
}
