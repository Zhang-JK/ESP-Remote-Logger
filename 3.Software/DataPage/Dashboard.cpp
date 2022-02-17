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

    connect(&dataHandler, SIGNAL(tcpUpdate(QJsonArray * )), this, SLOT(updateTCPData(QJsonArray * )));
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
    for (const QJsonValueRef &ref: *dataFormat) {
        QJsonObject obj = ref.toObject();
        QStandardItem *item = new QStandardItem(obj["name"].toString());
        dataItems.insert(obj.find("id").value().toInt(), item);

        QStandardItem *childItem = new QStandardItem("Update Time");
        item->appendRow(childItem);

        dataChildItems.insert(item, new QList<QStandardItem *>());
        QStandardItem *childItemValue = new QStandardItem("");
        dataChildItems[item]->append(childItemValue);
        item->setChild(0, 1, childItemValue);

        for (const QJsonValueRef &refChild: obj.find("field")->toArray()) {
            QJsonObject field = refChild.toObject();
            QStandardItem *childItem = new QStandardItem(field["name"].toString());
            item->appendRow(childItem);
            childItemValue = new QStandardItem("");
            dataChildItems[item]->append(childItemValue);
            item->setChild(childItem->row(), 1, childItemValue);
        }

        model->appendRow(item);
        model->setItem(model->indexFromItem(item).row(), 1,
                       new QStandardItem(QString("id: %1").arg(obj["id"].toInt())));
    }
}

void Dashboard::udpDecode(const QList<int> &updated) {
    for (int id: updated) {
        if (dataItems.contains(id)) {
            QStandardItem *item = dataItems[id];
            dataChildItems[item]->at(0)->setText(QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));
            QJsonObject obj = dataHandler.findInDataFormat(id)->toObject();

            int i = 1;
            for (const QJsonValueRef &ref: obj.find("field")->toArray()) {
                QJsonObject field = ref.toObject();
                QByteArray array = dataHandler.getData(id).data();
                int displacement = field.find("displacement").value().toInt();
                if (field.find("type")->toInt() == 1) {
                    qint8 value8 = array[displacement];
                    dataChildItems[item]->at(i++)->setText(QString("%1").arg(value8));
                } else if (field.find("type")->toInt() == 2) {
                    char data[2] = {array[displacement],
                                    array[displacement + 1]};
                    qint16 value16 = *(qint16 *) data;
                    dataChildItems[item]->at(i++)->setText(QString("%1").arg(value16));
                } else if (field.find("type")->toInt() == 3) {
                    char data[4] = {array[displacement],
                                    array[displacement + 1],
                                    array[displacement + 2],
                                    array[displacement + 3]};
                    qint32 value32 = *(qint32 *) data;
                    dataChildItems[item]->at(i++)->setText(QString("%1").arg(value32));
                } else if (field.find("type")->toInt() == 4) {
                    char data[4] = {array[displacement],
                                    array[displacement + 1],
                                    array[displacement + 2],
                                    array[displacement + 3]};
                    float value = *(float *) data;
                    dataChildItems[item]->at(i++)->setText(QString("%1").arg(value));
                }
            }
        }
    }
}

void Dashboard::receiveRate(int rate) {
    ui->refresh->display(rate);
}

