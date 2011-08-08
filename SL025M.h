#ifndef SL025M_H
#define SL025M_H

#include <WProgram.h>

#define BLOCK_SIZE 16

// Reader commands
#define GET_TAG 0x01
#define LOGIN 0x02
#define READ_BLOCK 0x03
#define RESET 0xFF
#define SEND_HEADER 0xBA

// Read status
#define OPERATION_OK 0x00
#define NO_TAG 0x01
#define READ_FAIL 0x04
#define WRITE_FAIL 0x05
#define UNABLE_READ_AFTER_WRITE 0x06
#define READ_AFTER_WRITE_FAIL 0x07
#define CHECKSUM_ERROR 0xF0
#define COMMAND_CODE_ERROR 0xF1

// Login key types
#define USE_KEY_A 0xAA
#define USE_KEY_B 0xBB

// Login results
#define LOGIN_SUCCEED 0x02
#define LOGIN_FAIL 0x03
#define LOAD_KEY_FAIL 0x0C

// Tag types
#define MIFARE_STANDARD_1K 0x01
#define MIFARE_PRO_CARD 0x02
#define MIFARE_ULTRALIGHT 0x03
#define MIFARE_STANDARD 0x04
#define MIFARE_PROX 0x05
#define MIFARE_DESFIRE 0x06

// Stores tag data
struct taginfo {
  unsigned long uid;  // UID with 4 bytes
  unsigned long uid2; // UID first 3 digits for UIDs with 4 bytes
  byte type;
  byte status;
};

// Stores a block data
struct datablock {
  byte status;
  byte data[BLOCK_SIZE];
};

// Stores a response from a reader
struct reader_response {
  byte length;
  byte status;
  byte command;
  byte data[300];
};

class SL025M {

private:
  byte checksum(byte *values, int length);
  void copyByteArray(byte *from, byte *to, int total);
  unsigned long getUID(byte *data, int total);
  void getResponse(reader_response *response);
  
public:
  void finalize();
  void readTagInfo(taginfo *tag);
  int loginToSector(byte sector, byte *key, byte type);
  datablock *readDatablock(int block);
  
};

#endif
