/**
 * This file is used for handling data from the server.
 * TCP is used to set up initial connection and heartbeat.
 * UDP is used to receive debug data.
 */

#include <QJsonObject>
#include "DataHandler.h"

void DataHandler::init(QString serverIP, quint16 serverTCPPort, quint16 serverUDPPort, quint16 clientTCPPort,
                       quint16 clientUDPPort) {
    this->serverIP = serverIP;
    this->serverTCPPort = serverTCPPort;
    this->serverUDPPort = serverUDPPort;
    this->clientTCPPort = clientTCPPort;
    this->clientUDPPort = clientUDPPort;
    this->isInit = true;
    connect(&tcpSocket, &QTcpSocket::readyRead, this, &DataHandler::tcpReady);
    connect(&udpSocket, &QUdpSocket::readyRead, this, &DataHandler::udpReady);
}

int DataHandler::connectToServer(QString name) {
    if (!isInit) return -3;
    tcpSocket.connectToHost(serverIP, serverTCPPort);
    if(!tcpSocket.waitForConnected(5000))
    {
        qDebug() << "Connection failed!";
        return -2;
    }
    qDebug() << "Connect successfully!";
    QByteArray msg;
    msg.append("N:").append(name).append('\n');
    tcpSocket.write(msg);
    return tcpSocket.flush() ? 0 : -1;
}

int DataHandler::sendHeartbeat() {
    return 0;
}

const QJsonArray &DataHandler::getDataFormat() {
    return dataFormat;
}

const QByteArray& DataHandler::getData() {
    return data;
}

void DataHandler::tcpReady() {
    data = tcpSocket.readAll();
    qDebug() << "TCP Data: " << data;

    char splitter = 0xAF;
    QList<QByteArray> list = data.split(splitter);
    for (const QByteArray& str : list) {
        if(str.length() == 0) continue;
        if(str[0] != (char)0xA2) continue;
        dataFormat.append(decodeDataFormat(str));
    }

    qDebug() << "Data format: " << dataFormat << endl;
}

void DataHandler::udpReady() {

}

QJsonObject DataHandler::decodeDataFormat(const QString &frame) {
    QJsonObject obj;
    obj.insert("id", frame[1].toLatin1());
    obj.insert("length", frame[2].toLatin1());
    obj.insert("size", frame[3].toLatin1());
    obj.insert("name", frame.mid(4, frame.indexOf('\;') - 4));
    QJsonArray array;
    QStringList list = frame.mid(frame.indexOf('\;') + 1).split(',');
    for(QString str : list) {
        QJsonObject subObj;
        if(str[0].toLatin1() < 1 || str[0].toLatin1()  > 4) continue;
        subObj.insert("type", str[0].toLatin1());
        subObj.insert("name", str.mid(2, str.length() - 2));
        array.append(subObj);
    }
    if (array.size() != frame[2].toLatin1())
        qDebug() << "Data format error!" << frame << "\n";

    obj.insert("field", array);
    return obj;
}

DataHandler dataHandler;
