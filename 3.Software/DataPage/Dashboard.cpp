/**
 * This file is used for the Dashboard page.
 * It contains the data form CAN bus and development board.
 */

// You may need to build the project (run Qt uic code generator) to get "ui_Dashboard.h" resolved

#include "Dashboard.h"
#include "ui_Dashboard.h"
#include "../NetDataHandler/DataHandler.h"

Dashboard::Dashboard(QWidget *parent) :
        QWidget(parent), ui(new Ui::Dashboard) {
    ui->setupUi(this);
    model = new QStandardItemModel(ui->dataTree);
    ui->dataTree->setModel(model);
    model->setHorizontalHeaderLabels(QStringList() << QStringLiteral("name") << QStringLiteral("value"));
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

void Dashboard::udpDecode(const QList<int>& updated) {
    qDebug() << updated;
}

void Dashboard::receiveRate(int rate) {
    ui->refresh->display(rate);
}

