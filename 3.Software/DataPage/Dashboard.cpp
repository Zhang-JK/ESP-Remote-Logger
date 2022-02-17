/**
 * This file is used for the Dashboard page.
 * It contains the data form CAN bus and development board.
 */

// You may need to build the project (run Qt uic code generator) to get "ui_Dashboard.h" resolved

#include <QJsonObject>
#include <QDateTime>
#include "Dashboard.h"
#include "ui_Dashboard.h"
#include "../NetDataHandler/DataHandler.h"

Dashboard::Dashboard(QWidget *parent) :
        QWidget(parent), ui(new Ui::Dashboard) {
    ui->setupUi(this);
    model = new QStandardItemModel(ui->dataTree);
    ui->dataTree->setModel(model);
    model->setHorizontalHeaderLabels(QStringList() << QStringLiteral("name") << QStringLiteral("value"));
    ui->dataTree->setColumnWidth(0, 150);

    connect(&dataHandler, SIGNAL(tcpUpdate(QJsonArray*)), this, SLOT(updateTCPData(QJsonArray*)));
    connect(ui->fullScreen, SIGNAL(clicked()), this, SLOT(fullScreen()));
    connect(&dataHandler, SIGNAL(updReceived(const QList<int>&)), this, SLOT(udpDecode(const QList<int>&)));
    connect(&dataHandler, SIGNAL(receiveRateUpdate(int)), this, SLOT(receiveRate(int)));
}

Dashboard::~Dashboard() {
    delete ui;
}

void Dashboard::fullScreen() {
    if (this->ui->fullScreen->text() == "Full Screen") {
        this->showFullScreen();
        this->ui->fullScreen->setText("Exit");
    } else {
        this->showNormal();
        this->ui->fullScreen->setText("Full Screen");
    }
}

void Dashboard::updateTCPData(QJsonArray *dataFormat) {
    model->clear();
    model->setHorizontalHeaderLabels(QStringList() << QStringLiteral("name") << QStringLiteral("value"));
    ui->dataTree->setColumnWidth(0, 150);
    for(const QJsonValueRef &ref : *dataFormat) {
        QJsonObject obj = ref.toObject();
        QStandardItem *item = new QStandardItem(obj["name"].toString());
        dataItems.insert(obj.find("id").value().toInt(), item);
        QStandardItem *childItem = new QStandardItem("Update Time");
        item->appendRow(childItem);
        for(const QJsonValueRef &refChild : obj.find("field")->toArray()) {
            QJsonObject field = refChild.toObject();
            QStandardItem *childItem = new QStandardItem(field["name"].toString());
            item->appendRow(childItem);
        }
        model->appendRow(item);
        model->setItem(model->indexFromItem(item).row(), 1, new QStandardItem(QString("id: %1").arg(obj["id"].toInt())));
    }
}

void Dashboard::udpDecode(const QList<int>& updated) {
    qDebug() << updated;
    for(int id : updated) {
        if(dataItems.contains(id)) {
            QStandardItem *item = dataItems[id];
            item->setChild(0, 1, new QStandardItem(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
            for(int i = 1; i <= item->rowCount(); i++) {
                item->setChild(0, 1, new QStandardItem(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
            }
        }
    }
}

void Dashboard::receiveRate(int rate) {
    ui->refresh->display(rate);
}

