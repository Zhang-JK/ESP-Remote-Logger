/**
 * Connection
 * Connect to a server
 */

// You may need to build the project (run Qt uic code generator) to get "ui_Connection.h" resolved

#include <QMessageBox>

#include "Connection.h"
#include "ui_Connection.h"

Connection::Connection(QWidget *parent) :
        QWidget(parent), ui(new Ui::Connection) {
    ui->setupUi(this);

    connect(ui->connect, SIGNAL(clicked()), this, SLOT(connectToServer()));
}

Connection::~Connection() {
    delete ui;
}

void Connection::connectToServer() {
    qDebug() << "Connecting to server";
    ui->connect->setEnabled(false); ui->connect->setText("Connecting");
    ui->cancel->setEnabled(false);
    QRegExp ipVal("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(!ipVal.exactMatch(ui->ip->text()))
    {
        QMessageBox::critical(this, tr("Syntax Error"), tr("IP Address is not valid"));
        return ;
    }
    if(!ui->port->text().contains(QRegExp("^\\d+$")) || (ui->port->text().toInt() < 0 || ui->port->text().toInt() > 65535))
    {
        QMessageBox::critical(this, tr("Syntax Error"), tr("Port is not valid"));
        return ;
    }

    dataHandler.init(ui->ip->text(), ui->port->text().toInt(), 23333, USER_TCP_PORT, USER_UDP_PORT);
    dataHandler.connectToServer("test");

    ui->connect->setText("Connected");
    QMessageBox::information(this, tr("Connected"), tr("Connected to server"), QMessageBox::Yes);
    this->close();
}

