/**
 * This file is used for handling data from the server.
 * TCP is used to set up initial connection and heartbeat.
 * UDP is used to receive debug data.
 */

#ifndef LOGGERCLIENT_DATAHANDLER_H
#define LOGGERCLIENT_DATAHANDLER_H


#include <QString>
#include <QJsonArray>
#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDebug>

enum class DATA_TYPE {
    UINT8  = 0x01,
    UINT16 = 0x02,
    UINT32 = 0x03,
    FLOAT  = 0x04
};

class DataHandler : public QObject {
    Q_OBJECT
public:
    void init(QString serverIP, quint16 serverTCPPort, quint16 serverUDPPort, quint16 clientTCPPort, quint16 clientUDPPort);
    int connectToServer(QString name);
    int sendHeartbeat();
    const QJsonArray& getDataFormat();
    const QByteArray& getData();

signals:
    void dataReceived(const quint8& data);
    void dataFormatReceived(const QJsonArray& dataFormat);

private slots:
    void tcpReady();
    void udpReady();

private:
    QTcpSocket tcpSocket;
    QUdpSocket udpSocket;
    bool isInit = false;
    QString serverIP;
    quint16 serverTCPPort;
    quint16 serverUDPPort;
    quint16 clientTCPPort;
    quint16 clientUDPPort;
    QJsonArray dataFormat;
    QByteArray data;

    QJsonObject decodeDataFormat(const QString& frame);
};

extern DataHandler dataHandler;

#endif //LOGGERCLIENT_DATAHANDLER_H
