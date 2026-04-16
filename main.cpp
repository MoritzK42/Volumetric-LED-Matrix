/*
Bachelorprojekt 2025
volumetric led matrix disc

Copyright (c) 2026 Moritz Koslowski

Permission is granted to use, copy, and modify this source code
for private, non-commercial purposes only.

Any commercial use, distribution, sublicensing, or inclusion
in a commercial product or service is strictly prohibited
without prior written permission from the copyright holder.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
*/

#include <Arduino.h>
#include <SPI.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include "font5x7.h"
#include "loading2D.h"

#include "windrad_1.h"
#include "drehrad_8.h"
#include "SE_1.h"
#include "widesky_1.h"
#include "nightsky_1.h"
#include "rollingframe_18.h"
#include "rooollingframe_24.h"
#include "SEseperate_1.h"
#include "bird_7.h"
#include "fireanimation_9.h"

#include "calibrationCo.h"
#include "pokal.h"
#include "wireframecube.h"
#include "campfiremedium_8.h"

#define ROT_OFFSET_SLOWNESS 2       //slowness of object rotation -> 1 = max speed, 1/96th increment each revolution
#define ROT_RESULOTION 95           //subframe amount -1
#define ON_TIME_PERCENT 1           //percentage of time per subframe time slot an LED is switched on (1 = 1%)
#define DISPLAY_2D_DELAY 100        //delay between 2D frames in milliseconds
#define LOADING_ANIMATION_FRAMES 9  //amount of frames in the loading animation
#define MAX_TX_POWER 20             // units of 0.25 dBm, so = 5 dBm | max: 80 = 20 dBm - high TX power leads to MCU chrashing sometimes

#define AMOUNT_3D_IMAGES 6
const uint8_t (*frameptr[AMOUNT_3D_IMAGES])[8][1][ROT_RESULOTION + 1][32] = {&calibrationCo, &wireframecube, &pokal, &windrad_1, &widesky_1, &SEseperate_1};

#define AMOUNT_3D_ANIMATIONS 4
struct sAnimation
{
  const void *ptr;
  const int frameAmount;
  const int animationSlowness;

}typedef sAnimation;

//array struct of: {pointer to animation array, number of frames, animation slowness}
sAnimation animations[AMOUNT_3D_ANIMATIONS] = 
{
  {drehrad_8, 8, 1}, 
  {rooollingframe_24, 24, 2},
  {bird_7, 7, 2},
  {campfiremedium_8, 8, 2}
};


#define SDAT 6
#define SCLK 4
#define LAT 7
#define BLANK 3
#define PULSE 0
#define LED 1


uint8_t baseAddress[] = {0xf7, 0xd9, 0xf9, 0x86, 0xcf, 0x11};
uint8_t discAddress[] = {0x6e, 0x10, 0x32, 0x8d, 0xe8, 0xff};
esp_now_peer_info_t peerInfo;

uint64_t onUs = 0;
volatile unsigned long long lastTimeUs = 0;
volatile unsigned long long deltaT = 0;
volatile unsigned long long currentTimeUs = 0;
volatile bool pulseFlag = false;

byte timeSlots = ROT_RESULOTION;

int currentFrame = 0;
int frameCounter = 0;
int luxCompCounter = 0;
uint8_t nothing[32] = {};

//####### display3DMatrixV2 #########
uint64_t deltaT_64 = 0;
uint64_t resolution_64 = 0;
uint64_t slotTime = 0;
uint64_t onTime = 0;
//###################################

int animationFrameCounter = 0;
int animationFrameCounterDivider = 0;
bool animationFrameCounterDividerHelper = false;

bool rotateCW = false;
bool rotateCCW = false;
int rotOffsetCounter = 0;
int offsettedTimeSlots = 0;
int rotationOffsetCounterDivider = 0;
bool rotationOffsetCounterDividerHelper = false;

bool spinning = false;
volatile unsigned long rotations = 0;

int currentLoadingFrame = 0;
unsigned long loadingMillis = 0;

uint8_t Letter[32]  = {};

bool timeSlotWasZero = false;
int rotOffset = 0;
int rotOffsetHelper = 0;

int subsubStep = 0;
bool intTRIG = false;

void ISR_PULSE();
void displayFrameSPI8(uint8_t frame8Bit[32]);
void getMacAddress();
void setupESPNOWtransmit();
void setupESPNOWreceive();
void changeMACAddress();
void display3DMatrix();
void display2DAnimation();
void print2DLetters();
void actOnINt();
void displayLoading();

void setup() 
{
  pinMode(LED, OUTPUT);
  pinMode(PULSE, INPUT_PULLUP); //active LOW
  attachInterrupt(digitalPinToInterrupt(PULSE), ISR_PULSE, RISING);
  
  pinMode(SDAT, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(LAT, OUTPUT);
  pinMode(BLANK, OUTPUT);

  changeMACAddress();
  setupESPNOWtransmit();
  SPI.begin();

  displayFrameSPI8(nothing);
 
  //confirm successful setup by blinking onboard LED
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);
}

void loop() 
{
  if(rotations > 2)
  {
    if(!spinning)
    {
      spinning = true;
      for(int i = 0; i < 5; i++)
      {
        uint8_t message = 1;
        esp_now_send(baseAddress, (uint8_t *) &message, sizeof(message));
        delay(2);
      }
      
      setupESPNOWreceive();
    }
    if(deltaT > 1E5) //disc not spinning, or spinning very slowly
    {
      displayLoading();
      //display2DAnimation();
    }
    else
    {
      display3DMatrix();
    }
  }
  else
  {
    spinning = false;
    uint8_t message = 0;
    esp_now_send(baseAddress, (uint8_t *) &message, sizeof(message));
    //display2DAnimation();
    displayLoading();
  }
}

//cycle time optimized function, can now deal with gradual brightness reduction
void display3DMatrix()
{
  const uint64_t now = (uint64_t)esp_timer_get_time();
  if(pulseFlag)
  {
    pulseFlag = false;
    timeSlots = ROT_RESULOTION; //reset timeSlots
    //calculate new slot-parameter (without using floats)
    deltaT_64 = deltaT;
    resolution_64 = ROT_RESULOTION;
    slotTime = (deltaT_64 / (resolution_64 ? resolution_64 : 1)); //failsafe, prevent division by zero
    onTime = (uint32_t)((slotTime * (uint64_t)ON_TIME_PERCENT) / 100ULL);
  }

  if(now - currentTimeUs >= slotTime * (ROT_RESULOTION - timeSlots))
  {
    if(timeSlots > 0)
    {
      timeSlots--;
      animationFrameCounterDividerHelper = true;
      rotationOffsetCounterDividerHelper = true;
    }

    //used to rotate the 3D Image/Animation in increments of 1 / ROT_RESULOTION 
    if(rotateCW)
    {
      offsettedTimeSlots = timeSlots + rotOffsetCounter;
      if(offsettedTimeSlots > ROT_RESULOTION)
      {
        offsettedTimeSlots -= (ROT_RESULOTION + 1);
      }
    }
    else if(rotateCCW)
    {
      offsettedTimeSlots = timeSlots - rotOffsetCounter;
      if(offsettedTimeSlots < 0)
      {
        offsettedTimeSlots += (ROT_RESULOTION + 1);
      }
    }
    else
    {
      offsettedTimeSlots = timeSlots;
      rotOffsetCounter = 0; //always set to zero to start the rotation without jumping around
    }

    onUs = (uint64_t)esp_timer_get_time();
  }

  if(now - onUs > slotTime * onTime)
  {
    displayFrameSPI8(nothing);
  }
  else
  {
    //change luxCompCounter to shift through the 8 brightness compensated frames
    if(++luxCompCounter >= 8) luxCompCounter = 0;
    
    uint8_t dma32[32];
    const uint8_t* src;
    
    if(currentFrame < AMOUNT_3D_IMAGES)
    {
      src = (*frameptr[currentFrame])[luxCompCounter][0][offsettedTimeSlots];
    }
    else
    {
      int idx = currentFrame - AMOUNT_3D_IMAGES;
      int frames = animations[idx].frameAmount;
      const uint8_t (*anim)[8][frames][ROT_RESULOTION+1][32] = (const uint8_t (*)[8][frames][ROT_RESULOTION+1][32]) animations[idx].ptr;
      src = anim[0][luxCompCounter][animationFrameCounter][offsettedTimeSlots];

    }
    //copy frame from flash to RAM for jitter free access and smooth SPI shifting
    memcpy(dma32, src, 32);
    displayFrameSPI8(dma32);
  }

  //count up animationFrameCounter to change the frames for animation
  //special logic to slow down frame changing depended on animations->animationSlowness
  if(animationFrameCounterDivider >= animations->animationSlowness)
  {
    animationFrameCounterDivider = 0;
    {
      if(animationFrameCounter < animations[currentFrame - AMOUNT_3D_IMAGES].frameAmount - 1)
      {
        animationFrameCounter++;
      }
      else
      {
        animationFrameCounter = 0;
      }
    }
  }
  
  if(timeSlots == 0 && animationFrameCounterDividerHelper)
  {
    animationFrameCounterDividerHelper = false;
    animationFrameCounterDivider++;
  }

  //count up rotOffsetCounter to be able to rotate the 3D Image
  if(rotationOffsetCounterDivider >= ROT_OFFSET_SLOWNESS)
  {
    rotationOffsetCounterDivider = 0;
    if(++rotOffsetCounter >= ROT_RESULOTION + 1) rotOffsetCounter = 0;
  }

  if(timeSlots == 0 && rotationOffsetCounterDividerHelper)
  {
    rotationOffsetCounterDividerHelper = false;
    rotationOffsetCounterDivider++;
  }
}



void displayLoading()
{

  if(millis() - loadingMillis > DISPLAY_2D_DELAY)
  {
    loadingMillis = millis();
    if(currentLoadingFrame < LOADING_ANIMATION_FRAMES - 1)
    {
      currentLoadingFrame++;
    }
    else
    {
      currentLoadingFrame = 0;
    }
  }
  displayFrameSPI8(loading2D[0][currentLoadingFrame]);
  delay(0); 
  displayFrameSPI8(nothing);
  delay(10); //bad coding
}

void print2DLetters()
{
  bool buffer[256] = {};
  for(int i = 0; i < 5; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      buffer[8 * i + j] = font5x7[65 + i] & (0x01 << j);
    }
  }
  //convBoolTo8Bit(buffer, Letter);
}

void display2DAnimation()
{
  displayFrameSPI8(Letter);
}


//triggered at every rotation, used to calculate period duration
void IRAM_ATTR ISR_PULSE()
{
  rotations = rotations + 1;
  currentTimeUs = esp_timer_get_time(); //returns the time since program start in microseconds
  gpio_set_level((gpio_num_t)LED, HIGH); //faster than digitalWrite... only for debug
  deltaT = currentTimeUs - lastTimeUs;
  lastTimeUs = currentTimeUs;
  //timeSlots = ROT_RESULOTION; //reset timeSlots
  pulseFlag = true;
  gpio_set_level((gpio_num_t)LED, LOW);
}

void actOnINt()
{
  intTRIG = false;
}


void displayFrameSPI8(uint8_t frame8Bit[32])//bool inputFrame[256])
{
  SPI.beginTransaction(SPISettings(25E6, MSBFIRST, SPI_MODE3)); //SPI_MODE0 or SPI_MODE3 work | 35E6 = 35 MHz SPI freq, max of LED driver

  gpio_set_level((gpio_num_t)LAT, LOW); //faster than digitalWrite...
  SPI.writeBytes(frame8Bit, 32); 
  gpio_set_level((gpio_num_t)LAT, HIGH);

  SPI.endTransaction();
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
  //decrement trough frames/animations
  if(incomingData[0] == 1)
  {
    if(currentFrame > 0)
    {
      currentFrame--;
    }
    else
    {
      currentFrame = (AMOUNT_3D_IMAGES + AMOUNT_3D_ANIMATIONS) - 1;
    }
  }

  //increment trough frames/animations
  if(incomingData[0] == 2)
  {
    if(currentFrame < ((AMOUNT_3D_IMAGES + AMOUNT_3D_ANIMATIONS) - 1))
    {
      currentFrame++;
    }
    else
    {
      currentFrame = 0;
    }
  }

  //start/stop rotation clockwise
  if(incomingData[0] == 3)
  {
    rotateCW ^= 1;          //turn on/off 3D rotation clockwise
    rotateCCW = false;  //turn off 3D rotation counter clockwise
  }

  //start/stop rotation counter-clockwise
  if(incomingData[0] == 4)
  {
    rotateCCW ^= 1;         //turn on/off 3D rotation counter-clockwise
    rotateCW = false;  //turn off 3D rotation clockwise
  }
}

void setupESPNOWtransmit()
{
  WiFi.mode(WIFI_STA);
  esp_now_init();
  memcpy(peerInfo.peer_addr, baseAddress, 6);
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

    esp_wifi_set_mac(WIFI_IF_STA, discAddress);
}


