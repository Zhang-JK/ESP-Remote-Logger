/**
 * This file is used for handling data from the server.
 * TCP is used to set up initial connection and heartbeat.
 * UDP is used to receive debug data.
 */

#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkDatagram>
#include <QDateTime>
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

int DataHandler::startDataStream() {
    if (!isInit) return -1;
    udpSocket.bind(clientUDPPort);
    qDebug() << "Data Stream Started!";
    return 0;
}

int DataHandler::startHeartbeat() {
    heartbeatTimer.stop();
    heartbeatTimer.setInterval(1000);
    connect(&heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));
    heartbeatTimer.start();
    qDebug() << "Heartbeat Started!";
}

const QJsonArray &DataHandler::getDataFormat() {
    return dataFormat;
}

const QMap<int, QByteArray> &DataHandler::getData() {
    return buffer;
}

void DataHandler::tcpReady() {
    QByteArray data = tcpSocket.readAll();
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
    QByteArray data;
    idUpdateList.clear();
    while (udpSocket.hasPendingDatagrams()) {
        data += udpSocket.receiveDatagram().data();
    }

    char splitter = 0xAF;
    QList<QByteArray> list = data.split(splitter);
    for(const QByteArray& str : list) {
        if(str.length() == 0) continue;
        if(str[0] != (char)0xA1) continue;
        buffer[str[1]] = str.mid(3);
        if (!idUpdateList.contains(str[1])) idUpdateList.append(str[1]);
    }

    for (int i=0; i<dataFormat.count(); i++) {
        if(!idUpdateList.contains(dataFormat[i].toObject()["id"].toInt())) continue;
//        QJsonValueRef ref = dataFormat[i].toObject().find("timestamp").value();
//        if (ref.isUndefined()) continue;
//        ref = QDateTime::currentDateTime().toString("mm:ss");
        dataFormat[i].toObject().find("timestamp").value() = QDateTime::currentDateTime().toString("mm:ss");
    }
    receiveCount++;
    emit updReceived(idUpdateList);
}

void DataHandler::heartbeat() {
    emit receiveRateUpdate(receiveCount);
    receiveCount = 0;

    if (tcpSocket.state() != QAbstractSocket::ConnectedState) {
        tcpSocket.connectToHost(serverIP, serverTCPPort);
        if(!tcpSocket.waitForConnected(500)) {
            emit heartbeatFail();
            qDebug() << "Heartbeat connection failed!";
            return;
        }
    }

    QByteArray msg = "H:test\n";
    tcpSocket.write(msg);
}

QJsonObject DataHandler::decodeDataFormat(const QString &frame) {
    QJsonObject obj;
    obj.insert("id", frame[1].toLatin1());
    obj.insert("length", frame[2].toLatin1());
    obj.insert("size", frame[3].toLatin1());
    obj.insert("name", frame.mid(4, frame.indexOf('\;') - 4));
    obj.insert("timestamp", QDateTime::currentDateTime().toString("mm:ss"));
    buffer.insert(obj["id"].toInt(), QByteArray());

    QJsonArray array;
    QStringList list = frame.mid(frame.indexOf('\;') + 1).split(',');
    int displacement = 0;
    for(QString str : list) {
        QJsonObject subObj;
        if(str[0].toLatin1() < 1 || str[0].toLatin1()  > 4) continue;
        subObj.insert("type", str[0].toLatin1());
        subObj.insert("name", str.mid(2, str.length() - 2));
        subObj.insert("displacement", displacement);
        if(str[0].toLatin1() == 1)
            displacement += 1;
        else if(str[0].toLatin1() == 2)
            displacement += 2;
        else
            displacement += 4;
        array.append(subObj);
    }
    if (array.size() != frame[2].toLatin1() || displacement != frame[3].toLatin1())
        qDebug() << "Data format error!" << frame << "\n";

    obj.insert("field", array);
    return obj;
}

DataHandler dataHandler;
