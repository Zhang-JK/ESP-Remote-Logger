#ifndef CAN_H
#define CAN_H

#include <ESP32CAN.h>
#include <CAN_config.h>
#include <string.h>

#ifndef CAN_MESSAGE_LENGTH
#define CAN_MESSAGE_LENGTH 8
#endif

class CAN 
{
   private:
    CAN_frame_t rx_frame;
    CAN_frame_t tx_frame;

    uint8_t rxData[CAN_MESSAGE_LENGTH];
    // uint8_t *rxData;
    uint8_t txData[CAN_MESSAGE_LENGTH];
    int rxID;
    int txID;

   public:
    uint8_t *getRxData() { return rxData; }
    void setTxData(uint8_t *data) { memcpy(txData, data, CAN_MESSAGE_LENGTH); }
    int getRxID() { return rxID; }
    void setTxID(int id) { txID = id; }
    
    void init(CAN_speed_t speed, gpio_num_t rx, gpio_num_t tx);
    bool receive(int timeout);
    bool transmit();
};

#endif