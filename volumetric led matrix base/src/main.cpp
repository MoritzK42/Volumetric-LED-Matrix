/*
Bachelorprojekt 2025
volumetric LED matrix base

Copyright (c) 2026 Moritz Koslowski

Permission is granted to use, copy, and modify this source code
for private, non-commercial purposes only.

Any commercial use, distribution, sublicensing, or inclusion
in a commercial product or service is strictly prohibited
without prior written permission from the copyright holder.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
*/

#include <Arduino.h>
#include <ESP32Servo.h>
#include <Adafruit_NeoPixel.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include <WiFi.h>


#define LONG_PRESS_TIME 1000  //milliseconds until long press (alt function) is detected
#define DEBOUNCE_TIME 200     //milliseconds
#define DEFAULT_SPEED 27      //range 0 - 180 (180 way to high) 30 is bit low (flickering) but very low in vibration
#define TIME_TO_STANDBY 60000 //milliseconds
#define MAX_TX_POWER 20       //units of 0.25 dBm, so = 5 dBm | max: 80 = 20 dBm

#define BTN1 6
#define BTN2 7
#define BTN3 2
#define LED_ESC 5
#define WS2812 4
#define POT 4
#define LED_THT 0 
#define LED 1
#define ESC 3
#define EN_PWR 10

#define NUMPIXELS 16
Adafruit_NeoPixel pixels(NUMPIXELS, WS2812, NEO_GRB + NEO_KHZ800);

uint8_t baseAddress[] = {0xf7, 0xd9, 0xf9, 0x86, 0xcf, 0x11};
uint8_t discAddress[] = {0x6e, 0x10, 0x32, 0x8d, 0xe8, 0xff};
esp_now_peer_info_t peerInfo;

Servo myESC; 

bool powerState = false;
bool spinState = false;
bool spinning = false;
bool escOk = false;
unsigned long powerStartMillis = 0;
unsigned long lastMSGreceived = 0;
unsigned long timeToStandyMillis = 0;

unsigned long fadeMillis = 0;
byte fRed = 255;
byte fGreen = 0;
byte fBlue = 0;

volatile unsigned long BTN1Millis = 0;
volatile bool BTN1state = false;
volatile bool BTN1intAtt = false;

volatile unsigned long BTN2Millis = 0;
volatile bool BTN2state = false;
volatile bool BTN2intAtt = false;

volatile unsigned long BTN3Millis = 0;
volatile bool BTN3state = false;
volatile bool BTN3intAtt = false;

void setAllColor(byte red, byte green, byte blue);
void initESC();
void fadeRGB(int interval);
int avgValue(byte input, int cycles);
void INT_ATT();
void BTN1_INT();
void BTN2_INT();
void BTN3_INT();
void getMacAddress();
void setupESPNOWtransmit();
void setupESPNOWreceive();
void changeMACAddress();


void setup()
{
  pinMode(LED_ESC, OUTPUT);
  pinMode(LED_THT, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(ESC, OUTPUT);
  pinMode(EN_PWR, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BTN1), BTN1_INT, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN2), BTN2_INT, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN3), BTN3_INT, FALLING);

  pixels.begin(); //for WS2812 RGB LED ring

 
  changeMACAddress();
  setupESPNOWreceive();

  //confirm successful setup by blinking onboard LED
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);
}

void loop()
{
  INT_ATT();
  fadeRGB(10);

  if(BTN1state)
  {
    BTN1state = false;
    powerState ^= 1;

    if(powerState && !escOk)
    {
      digitalWrite(EN_PWR, HIGH);
      digitalWrite(LED_THT, HIGH);
      delay(500);
      digitalWrite(EN_PWR, LOW);
      delay(100);
      digitalWrite(EN_PWR, HIGH);
      delay(500);
      lastMSGreceived = millis();
      initESC();
      escOk = true;
    }
    else if (powerState && escOk)
    {
      myESC.write(140); //higher starting torque
      delay(200);
    }
    else
    {
      myESC.write(0);
    }
  }

  if(powerState)
  {
    if(millis() - powerStartMillis < 100)
    {
      myESC.write(120); //higher starting torque
    }
    else
    { 
      myESC.write(DEFAULT_SPEED);
    }
    timeToStandyMillis = millis();
  }
  else
  {
    myESC.write(0);
    powerStartMillis = millis();
  }

  //turn off ESC and wireless module after set time in order to limit unessecary heating of coils, and thus warping of 3D-printed holders.
  if(millis() - timeToStandyMillis > TIME_TO_STANDBY)
  {
    digitalWrite(EN_PWR, LOW);
    digitalWrite(LED_THT, LOW);
    spinning = false;
    escOk = false;
  }
  

  if(BTN2state)
  {
    if(digitalRead(BTN2))
    {
      BTN2state = false;
      uint8_t message = 1; //decrement trough frames/animations
      esp_now_send(discAddress, (uint8_t *) &message, sizeof(message));
    }
    else if(!digitalRead(BTN2) && (millis() - BTN2Millis > LONG_PRESS_TIME))
    {
      BTN2state = false;
      uint8_t message = 3;  //start/stop rotation clockwise
      esp_now_send(discAddress, (uint8_t *) &message, sizeof(message));
    }
    
  }

  if(BTN3state)
  {
    if(digitalRead(BTN3))
    {
      BTN3state = false;
      uint8_t message = 2;  //increment trough frames/animations
      esp_now_send(discAddress, (uint8_t *) &message, sizeof(message));
    }
    else if(!digitalRead(BTN3) && (millis() - BTN3Millis > LONG_PRESS_TIME))
    {
      BTN3state = false;
      uint8_t message = 4;  //start/stop rotation counter-clockwise
      esp_now_send(discAddress, (uint8_t *) &message, sizeof(message));
    }
  }
}



void initESC()
{
  while(!spinning)
  {
    if(millis() - lastMSGreceived > 3000)
    {
      powerState = false;
      break;
    }
    digitalWrite(LED, HIGH);
    //delay(3200);
    myESC.attach(ESC, 900, 2100);

    
    for(int i = 80; i < 100; i++)
    {
      myESC.write(i);
      
      delay(5);
    }
    delay(1000);
    for(int i = 0; i < 10; i++)
    {
      myESC.write(i);
      
      delay(5);
    }
    myESC.write(0);
    delay(1000);
    
    digitalWrite(LED, LOW);
    //powerStartMillis = millis();
    //if(millis() - powerStartMillis < 500)

    myESC.write(140); //higher starting torque
    delay(500);
    myESC.write(DEFAULT_SPEED);
    delay(1000);
    if(!spinning)
    {
      digitalWrite(EN_PWR, LOW);
      digitalWrite(LED_THT, LOW);
      delay(500);
      digitalWrite(EN_PWR, HIGH);
      digitalWrite(LED_THT, HIGH);
      delay(500);
      digitalWrite(EN_PWR, LOW);
      delay(100);
      digitalWrite(EN_PWR, HIGH);
      delay(500);
    }
  }
  if(spinning)
  {
    setupESPNOWtransmit();
  }
}

void INT_ATT()
{
  if((millis() - BTN1Millis > DEBOUNCE_TIME) && !BTN1intAtt && digitalRead(BTN1))
  {
    attachInterrupt(digitalPinToInterrupt(BTN1), BTN1_INT, FALLING);
    BTN1intAtt = true;
  }

  if((millis() - BTN2Millis > DEBOUNCE_TIME) && !BTN2intAtt && digitalRead(BTN2))
  {
    attachInterrupt(digitalPinToInterrupt(BTN2), BTN2_INT, FALLING);
    BTN2intAtt = true;
  }

  if((millis() - BTN3Millis > DEBOUNCE_TIME) && !BTN3intAtt && digitalRead(BTN3))
  {
    attachInterrupt(digitalPinToInterrupt(BTN3), BTN3_INT, FALLING);
    BTN3intAtt = true;
  }
}

void BTN1_INT()
{
  BTN1Millis = millis();
  detachInterrupt(digitalPinToInterrupt(BTN1));
  BTN1intAtt = false;
  BTN1state = true;
}

void BTN2_INT()
{
  BTN2Millis = millis();
  detachInterrupt(digitalPinToInterrupt(BTN2));
  BTN2intAtt = false;
  BTN2state = true;
}

void BTN3_INT()
{
  BTN3Millis = millis();
  detachInterrupt(digitalPinToInterrupt(BTN3));
  BTN3intAtt = false;
  BTN3state = true;
}

int avgValue(byte input, int cycles)
{
  long buffer = 0;
  for(int i = 0; i < cycles; i++)
  {
    buffer += analogRead(input);
  }
  return buffer / cycles;
}


void setAllColor(byte red, byte green, byte blue)
{
  for(int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(red, green, blue));
  }
  pixels.show();
}

void fadeRGB(int interval)
{
  if(millis() - fadeMillis > interval)
  {
    fadeMillis = millis();
    if(fGreen < 255 && fBlue == 0)
    {
      fGreen++;
      fRed--;
    }
    else if(fGreen > 0 && fRed == 0 )
    {
      fGreen--;
      fBlue++;
    }
    else if(fGreen == 0 && fRed < 255)
    {
      fBlue--;
      fRed++;
    }
    setAllColor(fRed, fGreen, fBlue);
  }
}

void getMacAddress()
{
  Serial.begin(115200);
    delay(1000); // uncomment if your serial monitor is empty
    WiFi.mode(WIFI_STA); 
    while (!(WiFi.STA.started())) { // comment the while loop for ESP8266
        delay(10);
    }
    delay(1000);
    Serial.print("MAC-Address: ");
    String mac = WiFi.macAddress();
    Serial.println(mac);
    
    Serial.print("Formated: ");
    Serial.print("{");
    int index = 0;
    for(int i=0; i<6; i++){
        Serial.print("0x");
        Serial.print(mac.substring(index, index+2));
        if(i<5){
            Serial.print(", ");
        }
        index += 3;
    }
    Serial.println("}");
}

void messageReceived(const esp_now_recv_info *info, const uint8_t* incomingData, int len)
{
  lastMSGreceived = millis();
  if(incomingData[0] == 0)
  {
    spinning = false;
  }
  else if(incomingData[0] == 1)
  {
    spinning = true;
  }
}

void setupESPNOWtransmit()
{
  WiFi.mode(WIFI_STA);
  esp_now_init();
  memcpy(peerInfo.peer_addr, discAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
  esp_wifi_set_max_tx_power(MAX_TX_POWER);  //reduce max TX power, because of unstable power supply and problems with MCU resetting otherwise
}



void setupESPNOWreceive()
{
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(messageReceived);
}

void changeMACAddress()
{
  WiFi.mode(WIFI_STA);
    while (!(WiFi.STA.started())) 
    {
      delay(100);
    }

    esp_wifi_set_mac(WIFI_IF_STA, baseAddress);
}