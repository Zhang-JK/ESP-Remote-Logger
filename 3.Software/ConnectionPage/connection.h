//
// Created by lao_jk on 2022/2/12.
//

#ifndef LOGGERCLIENT_CONNECTION_H
#define LOGGERCLIENT_CONNECTION_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class Connection; }
QT_END_NAMESPACE

class Connection : public QMainWindow {
Q_OBJECT

public:
    explicit Connection(QWidget *parent = nullptr);

    ~Connection() override;

private:
    Ui::Connection *ui;
};


#endif //LOGGERCLIENT_CONNECTION_H
