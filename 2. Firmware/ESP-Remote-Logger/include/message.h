#ifndef MESSAGE_H
#define MESSAGE_H
#include <Arduino.h>
#include <iostream>
#include <list>
using namespace std;

#define TEXT_HEADER 0xA0
#define DATA_HEADER 0xA1
#define REGISTER_HEADER 0xA2
#define RESPONSE_HEADER 0xA3
#define STOP_BYTE 0xAF

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
    string *fieldName;
    list<DataDictionary *> data;

    DataMessage(uint8_t id, uint8_t number, uint8_t size, string name, string field);
    ~DataMessage();
    void decode(uint8_t *data);
    DataDictionary *getData();
    DataDictionary *getData(string key);
    DataDictionary *operator[](string key);

    string generateRegisterMsg();
    void transmitMsg();

private:
    uint8_t *buffer;
};

class DataMessageHandler
{
public:
    int numberOfMessages = 0;
    list<DataMessage *> messages;

    DataMessage *getMessageByID(uint8_t id);
    DataMessage *setMessageByID(uint8_t id, string field);
    void registerMsg(string regData);
    void registerMsg(uint8_t id, uint8_t number, uint8_t size, string name, string field);
    bool rxUpdate(uint8_t id, string data);
    bool txTransmit(uint8_t id);
};

void usartReceive(HardwareSerial& serial);

extern DataMessageHandler rx;
extern DataMessageHandler tx;

#endif