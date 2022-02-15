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
#include <QTimer>

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
    int startHeartbeat();
    int startDataStream();
    const QJsonArray& getDataFormat();
    const QMap<int, QByteArray>& getData();

signals:
    void heartbeatFail();
    void updReceived(const QList<int>& data);
    void receiveRateUpdate(int rate);

private slots:
    void tcpReady();
    void udpReady();
    void heartbeat();

private:
    QTcpSocket tcpSocket;
    QUdpSocket udpSocket;
    bool isInit = false;
    QString serverIP;
    quint16 serverTCPPort;
    quint16 serverUDPPort;
    quint16 clientTCPPort;
    quint16 clientUDPPort;
    QTimer heartbeatTimer;
    QJsonArray dataFormat;
    QList<int> idUpdateList;
    QMap<int, QByteArray> buffer;
    int receiveCount = 0;

    QJsonObject decodeDataFormat(const QString& frame);
};

extern DataHandler dataHandler;

#endif //LOGGERCLIENT_DATAHANDLER_H
