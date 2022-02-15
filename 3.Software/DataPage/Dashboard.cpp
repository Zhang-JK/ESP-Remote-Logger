/**
 * This file is used for the Dashboard page.
 * It contains the data form CAN bus and development board.
 */

// You may need to build the project (run Qt uic code generator) to get "ui_Dashboard.h" resolved

#include "Dashboard.h"
#include "ui_Dashboard.h"


Dashboard::Dashboard(QWidget *parent) :
        QWidget(parent), ui(new Ui::Dashboard) {
    ui->setupUi(this);
}

Dashboard::~Dashboard() {
    delete ui;
}

