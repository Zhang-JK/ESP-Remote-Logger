#include <message.h>

DataMessageHandler rx;
DataMessageHandler tx;

DataDictionary::~DataDictionary()
{
    if (type == UINT8)
        delete (uint8_t *)value;
    else if (type == UINT16)
        delete (uint16_t *)value;
    else if (type == UINT32)
        delete (uint32_t *)value;
    else if (type == FLOAT)
        delete (float *)value;
}

DataMessage::DataMessage(uint8_t id, uint8_t number, uint8_t size, string name, string field)
{
    this->id = id;
    this->number = number;
    this->size = size;
    this->name = name;
    this->data = new DataDictionary[number];
    this->buffer = new uint8_t[size];
    char *token = strtok(&field[0], ",");
    for (int i = 0; i < number; i++)
    {
        this->data[i].key = string(token).substr(2, string(token).length() - 2);
        this->data[i].type = (DataType)token[0];
        switch (this->data[i].type)
        {
        case UINT8:
            this->data[i].value = new uint8_t;
            break;
        case UINT16:
            this->data[i].value = new uint16_t;
            break;
        case UINT32:
            this->data[i].value = new uint32_t;
            break;
        case FLOAT:
            this->data[i].value = new float;
            break;
        }

        token = strtok(NULL, ",");
    }
}

DataMessage::~DataMessage()
{
    delete[] data;
    delete[] buffer;
    delete[] fieldName;
}

void DataMessage::decode(uint8_t *data)
{
}

DataDictionary *DataMessage::getData()
{
    return nullptr;
}

DataDictionary *DataMessage::getData(string key)
{
    return nullptr;
}

string DataMessage::generateRegisterMsg() {
    string result = "";
    result = result + (char)REGISTER_HEADER + (char)id + (char)number + (char)size + name + "\;";

    for(int i=0; i<number; i++)
        result = result + (char)data[i].type + "-" + data[i].key + ",";

    result = result + (char)STOP_BYTE;
    return result;
}

DataMessage *DataMessageHandler::getMessageByID(uint8_t id)
{
    for (list<DataMessage *>::iterator it = messages.begin(); it != messages.end(); it++)
    {
        if ((*it)->id == id)
            return *it;
    }
    return nullptr;
}

void DataMessageHandler::registerMsg(string regData) 
{
    uint8_t id = regData[0];
    uint8_t number = regData[1];
    uint8_t size = regData[2];
    int ind = regData.find_first_of("\;");
    string name = regData.substr(3, ind - 3);
    string field = regData.substr(ind + 1, regData.length() - ind - 1);
    registerMsg(id, number, size, name, field);
}

void DataMessageHandler::registerMsg(uint8_t id, uint8_t number, uint8_t size, string name, string field)
{
    for (list<DataMessage *>::iterator it = messages.begin(); it != messages.end(); it++)
        if ((*it)->id == id) {
            Serial.println("Message already registered");
            return ;
        }

    messages.push_back(new DataMessage(id, number, size, name, field));
    numberOfMessages++;
    Serial.printf("Registered message id %02x named \"%s\" with %d fields: %s \n", id, name.c_str(), number, field.c_str());
}

void usartRxProcess(uint8_t header, string& data) {
    Serial.printf("Received Header %02X, message: %s \n", header, data.c_str());
    
    switch (header)
    {
    case TEXT_HEADER:
        break;
    case DATA_HEADER:
        break;
    case REGISTER_HEADER:
        rx.registerMsg(data);
        break;
    case RESPONSE_HEADER:
        break;
    
    default:
        return ;
    }
}

bool isReceiving = false;
uint8_t usartHeader = 0;
string usartData = "";
void usartReceive(HardwareSerial& serial) {
    while(serial.available()) {
        uint8_t d = serial.read();
        if (isReceiving) {
            if (d == STOP_BYTE) {
                usartRxProcess(usartHeader, usartData);
                isReceiving = false;
                usartHeader = 0; usartData = "";
            } 
            else {
                usartData += (char)d;
            }
        } else {
            if (d == TEXT_HEADER || d == DATA_HEADER || d == REGISTER_HEADER || d == RESPONSE_HEADER) {
                usartHeader = d;
                isReceiving = true;
                Serial.printf("Receiving Header %02X \n", d);
            }
        }
    }
}

