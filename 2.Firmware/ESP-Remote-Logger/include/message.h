#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <network.h>
#include <iostream>
#include <list>
using namespace std;

#define TEXT_HEADER 0xA0
#define DATA_HEADER 0xA1
#define REGISTER_HEADER 0xA2
#define RESPONSE_HEADER 0xA3
#define STOP_BYTE 0xAF

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

enum DataType {
    UINT8 = 0x01,
    UINT16,
    UINT32,
    FLOAT
};

struct DataDictionary
{
    string key;
    DataType type;
    void * const value;

    DataDictionary(string key, DataType type, void * const value) : key(key), type(type), value(value) {}
    ~DataDictionary();
};

class DataMessage
{
public:
    uint8_t id;
    uint8_t number;
    uint8_t size;
    string name;
    bool updated = false;

    DataMessage(uint8_t id, uint8_t number, uint8_t size, string name, string field);
    ~DataMessage();
    
    void setBuffer(uint8_t *data);
    const uint8_t *getBuffer();
    void transmitMsg(HardwareSerial& serial);

    DataDictionary *getData(string key);
    DataDictionary *operator[](string key);

    string generateRegisterMsg();

private:
    // buffer is little endian!
    list<DataDictionary *> data;
    uint8_t *buffer;
};

class DataMessageHandler
{
public:
    int numberOfMessages = 0;
    list<DataMessage *> messages;
    uint8_t netBuffer[200];
    int netBufferLength = 0;

    const list<DataMessage *>& getMessageList() { return messages; }
    DataMessage *getMessageByID(uint8_t id);
    DataMessage *setMessageByID(uint8_t id, string field);
    void registerMsg(string regData);
    void registerMsg(uint8_t id, uint8_t number, uint8_t size, string name, string field);
    bool rxUpdate(uint8_t id, string data);
    bool txLoadMessage(uint8_t id, uint8_t *buffer);
    bool txTransmit(uint8_t id, HardwareSerial& serial);
    void txTransmitViaNet(ClientHandler& net);
};

void usartReceive(HardwareSerial& serial);

extern DataMessageHandler rx;
extern DataMessageHandler tx;

#endif