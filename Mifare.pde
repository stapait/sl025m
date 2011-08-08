#include <stdlib.h>

#include "SL025M.h"

#define BAUD_RATE 115200

SL025M mifare;
taginfo *tag;

// Key A to sector 2
byte keyA[6] = {
  0xD3, 0xA3, 0x4F, 0x11, 0xB8, 0xAA};  

// Key B to sector 2
byte keyB[6] = {
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};

void setup() {
  Serial.begin(BAUD_RATE);
}

void tagInfoTest() {
  tag = (taginfo *) malloc(sizeof(taginfo));
  mifare.readTagInfo(tag);

  switch (tag->type) {
  case MIFARE_STANDARD_1K:
    Serial.println("Standard 1K");
    break;

  case MIFARE_PRO_CARD:
    Serial.println("Pro");
    break;

  case MIFARE_ULTRALIGHT:
    Serial.println("Ultralight");
    break;

  case MIFARE_STANDARD:
    Serial.println("Standard");
    break;

  case MIFARE_PROX:
    Serial.println("Prox");
    break;

  case MIFARE_DESFIRE:
    Serial.println("Desfire");
    break;
  }

  if (tag->type > 0) {
    Serial.println(tag->uid);
  }

  delay(500);

  free(tag);
  tag = NULL;
}

void loginToSectorTest() {    
  if (mifare.loginToSector(2, keyA, USE_KEY_A) == LOGIN_SUCCEED)
    Serial.println("Login OK");
  else
    Serial.println("Login Failed");

  delay(500);  
}

void readDatablockTest() {
  int i;
  datablock *data;

  if (mifare.loginToSector(2, keyA, USE_KEY_A) == LOGIN_SUCCEED) {
    data = mifare.readDatablock(8);
    
    if (data->status == 0x00) {
      for (i = 0; i < 16; i++)
        Serial.println(data->data[i], HEX);
        
      delay(5000);      
    }    
  }
  
  delay(500);
}

void loop() {      
  tagInfoTest();
  //loginToSectorTest();
  //readDatablockTest();
}



