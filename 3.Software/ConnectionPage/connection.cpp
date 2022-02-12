//
// Created by lao_jk on 2022/2/12.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Connection.h" resolved

#include "connection.h"
#include "ui_Connection.h"


Connection::Connection(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::Connection) {
    ui->setupUi(this);
}

Connection::~Connection() {
    delete ui;
}

