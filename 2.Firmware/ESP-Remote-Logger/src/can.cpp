#include <can.h>

CAN_device_t CAN_cfg;

void CAN::init(CAN_speed_t speed, gpio_num_t rx, gpio_num_t tx) {
    CAN_cfg.speed = speed;
    CAN_cfg.rx_pin_id = rx;
    CAN_cfg.tx_pin_id = tx;
    CAN_cfg.rx_queue = xQueueCreate(1, sizeof(CAN_frame_t));
    ESP32Can.CANInit();
}

bool CAN::receive(int timeout) {
    if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, timeout * portTICK_PERIOD_MS) != pdTRUE)
        return false;
    rxID = rx_frame.MsgID;
    memcpy(rxData, rx_frame.data.u8, CAN_MESSAGE_LENGTH);
    return true;
}

bool CAN::transmit() {
    tx_frame.FIR.B.FF = CAN_frame_std;
    tx_frame.MsgID = txID;
    tx_frame.FIR.B.DLC = CAN_MESSAGE_LENGTH;
    memcpy(tx_frame.data.u8, txData, CAN_MESSAGE_LENGTH);
    return ESP32Can.CANWriteFrame(&tx_frame) == 0;
}