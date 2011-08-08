#include "SL025M.h"
#include <stdlib.h>

// Finalize serial port
void SL025M::finalize() {
  Serial.end();
}

/* Calculate checksum by making XOR of all values */
byte SL025M::checksum(byte *values, int length) {
  byte result = values[0];

  for (int i = 1; i < length; i++)
    result ^= values[i];

  return result;
}

/* Copy 'total' int values from one array to another */
void SL025M::copyByteArray(byte *from, byte *to, int total) {
  for (int i = 0; i < total; i++) {
    to[i] = from[i];
  }
}

/* Mount TAG UID according to size (card Type) */
unsigned long SL025M::getUID(byte *data, int total) {
  unsigned long result = 0;
  unsigned long values[7];

  for (int i = 0; i < total; i++) {
    values[i] = data[i];
  }
  
  if (total == 4) {
    result = (values[0] << 24) | (values[1] << 16) | (values[2] << 8) | values[3];
  }
  else if (total == 3) {
    result = (values[0] << 16) | (values[1] << 8) | values[2];
  }

  return result;
}

/* Get reader response */
void SL025M::getResponse(reader_response *response) {
  byte header, length, command, status, checksum;

  while (Serial.available() == 0) ;
  header = Serial.read();

  while (Serial.available() == 0) ;
  length = Serial.read();

  while (Serial.available() == 0) ;
  command = Serial.read(); 

  while (Serial.available() == 0) ;
  status = Serial.read();

  // Retrieve data
  if (status == OPERATION_OK) {
    for (int i = 0; i < length - 3; i++) {      
      while (Serial.available() == 0) ;
      response->data[i] = Serial.read();
    }    
  }

  while (Serial.available() == 0) ;
  checksum = Serial.read();

  response->length = length;
  response->command = command;
  response->status = status;
}

/* Retrieve TAG information */
void SL025M::readTagInfo(taginfo *tag) {
  byte *frame;
  reader_response *response;

  // Mount frame
  frame = (byte *) malloc(4 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 0x02;
  frame[2] = GET_TAG;
  frame[3] = checksum(frame, 3);

  // Send frame to reader  
  Serial.write(frame, 4);
  free(frame);
  frame = NULL;

  // Get response from reader
  response = (reader_response *) malloc(sizeof(reader_response));
  getResponse(response);

  // Mount taginfo result
  tag->status = response->status;

  // Get UID and Type from response
  if (tag->status == OPERATION_OK) {     
    if (response->length == 8) { 
      tag->uid = getUID(response->data, 4); // 4 bytes UID
      tag->type = response->data[4];
    }
    else if (response->length == 11) { // 7 bytes UID
      tag->uid = getUID(response->data, 4);      // UID last 4 digits
      tag->uid2 = getUID(&response->data[4], 3); // UID first 3 digits
      tag->type = response->data[7];
    }
  }
  else {
    tag->type = 0;  
  }

  free(response);
  response = NULL;
}

/* Login to a specific sector */
int SL025M::loginToSector(byte sector, byte *key, byte type) {
  byte *frame;
  reader_response *response;
  int status;

  frame = (byte *) malloc(12 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 10;   // Length 10
  frame[2] = LOGIN;
  frame[3] = sector;
  frame[4] = type;
  frame[5] = key[0];
  frame[6] = key[1];
  frame[7] = key[2];
  frame[8] = key[3];
  frame[9] = key[4];
  frame[10] = key[5];
  frame[11] = checksum(frame, 11);

  // Send frame to reader  
  Serial.write(frame, 12);
  free(frame);
  frame = NULL;

  // Get response from reader
  response = (reader_response *) malloc(sizeof(reader_response));
  getResponse(response);

  status = response->status;

  free(response);
  response = NULL;
  return status;
}

/* Retrieve data of a block from a previously logged in sector */
datablock* SL025M::readDatablock(int block) {
  byte *frame;
  reader_response *response;
  datablock *result;

  frame = (byte *) malloc(5 * sizeof(byte));
  frame[0] = SEND_HEADER;
  frame[1] = 3;
  frame[2] = READ_BLOCK;
  frame[3] = block;
  frame[4] = checksum(frame, 4);

  // Send frame to reader  
  Serial.write(frame, 5);
  free(frame);
  frame = NULL;

  // Get response from reader
  response = (reader_response *) malloc(sizeof(reader_response));
  getResponse(response);

  result = (datablock *) malloc(sizeof(datablock));
  result->status = response->status;

  // Retrieve data from block
  if (response->status == OPERATION_OK)
    copyByteArray(response->data, result->data, 16);

  free(response);
  return result;
}


