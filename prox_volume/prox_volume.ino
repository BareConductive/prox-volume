
/*******************************************************************************

 Bare Conductive proximity controlled volume
 -------------------------------------------
 
 prox_volume.ino - touch triggered MP3 playback with proximity controlled volume
 
 E0 is connected to an 85mmx85mm square electrode and E1..E11 play TRACK001.MP3
 through to TRACK011.MP3 respectively. E0 controls the volume - a closer hand
 decreases volume, a more distant hand increases volume.
 
 Based on code by Jim Lindblom and plenty of inspiration from the Freescale 
 Semiconductor datasheets and application notes.
 
 Bare Conductive code written by Stefan Dzisiewski-Smith and Peter Krige.
 
 This work is licensed under a MIT license https://opensource.org/licenses/MIT
 
 Copyright (c) 2016, Bare Conductive
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

*******************************************************************************/

// compiler error handling
#include "Compiler_Errors.h"

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

// mapping and filter definitions
#define LOW_DIFF 0
#define HIGH_DIFF 50
#define filterWeight 0.3f // 0.0f to 1.0f - higher value = more smoothing
float lastProx = 0;

// the electrode to monitor for proximity
#define PROX_ELECTRODE 0

// mp3 includes
#include <SPI.h>
#include <SdFat.h>
#include <FreeStack.h> 
#include <SFEMP3Shield.h>

// mp3 variables
SFEMP3Shield MP3player;
byte result;
int lastPlayed = 0;
uint8_t volume = 0;

// sd card instantiation
SdFat sd;

// define LED_BUILTIN for older versions of Arduino
#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void setup(){  
  Serial.begin(57600);
  
  pinMode(LED_BUILTIN, OUTPUT);
   
  //while (!Serial) ; {} //uncomment when using the serial monitor 
  Serial.println("Bare Conductive Touch MP3 player");

  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  result = MP3player.begin();
  MP3player.setVolume(volume,volume);
 
  if(result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to start MP3 player");
   }

  // slow down some of the MPR121 baseline filtering to avoid 
  // filtering out slow hand movements
  MPR121.setRegister(MPR121_NHDF, 0x01); //noise half delta (falling)
  MPR121.setRegister(MPR121_FDLF, 0x3F); //filter delay limit (falling)   
   
}

void loop(){
  readTouchInputs();
}


void readTouchInputs(){
    
  MPR121.updateAll();

  // only make an action if we have one or fewer pins touched
  // ignore multiple touches

  for (int i=1; i < 12; i++){  // Check which electrodes were pressed
    if(MPR121.isNewTouch(i)){
    
        //pin i was just touched
        Serial.print("pin ");
        Serial.print(i);
        Serial.println(" was just touched");
        digitalWrite(LED_BUILTIN, HIGH);
        
        if(MP3player.isPlaying()){
          if(lastPlayed==i){
            // if we're already playing the requested track, stop it
            MP3player.stopTrack();
            Serial.print("stopping track ");
            Serial.println(i);
          } else {
            // if we're already playing a different track, stop that 
            // one and play the newly requested one
            MP3player.stopTrack();
            MP3player.playTrack(i);
            Serial.print("playing track ");
            Serial.println(i);
            
            // don't forget to update lastPlayed - without it we don't
            // have a history
            lastPlayed = i;
          }
        } else {
          // if we're playing nothing, play the requested track 
          // and update lastplayed
          MP3player.playTrack(i);
          Serial.print("playing track ");
          Serial.println(i);
          lastPlayed = i;
        }   
    }else{
      if(MPR121.isNewRelease(i)){
        Serial.print("pin ");
        Serial.print(i);
        Serial.println(" is no longer being touched");
        digitalWrite(LED_BUILTIN, LOW);
     } 
    }
  }

  // read the difference between the measured baseline and the measured continuous data
  int reading = MPR121.getBaselineData(PROX_ELECTRODE)-MPR121.getFilteredData(PROX_ELECTRODE);

  // print out the reading value for debug
  Serial.print("Proximity electrode: ");
  Serial.println(reading); 

  // constrain the reading between our low and high mapping values
  unsigned int prox = constrain(reading, LOW_DIFF, HIGH_DIFF);
  
  // implement a simple (IIR lowpass) smoothing filter
  lastProx = (filterWeight*lastProx) + ((1-filterWeight)*(float)prox);

  // map the LOW_DIFF..HIGH_DIFF range to 0..254 (max range for MP3player.setVolume())
  uint8_t thisOutput = (uint8_t)map(lastProx,LOW_DIFF,HIGH_DIFF,0,254);

  // if((uint8_t)lastProx!=prox){ // only update volume if the value has changed
    // output the mapped value to the LED
    MP3player.setVolume(thisOutput, thisOutput);
  // }
}
