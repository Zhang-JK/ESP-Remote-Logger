/**
 * Connection
 * Connect to a server
 */

#ifndef LOGGERCLIENT_CONNECTION_H
#define LOGGERCLIENT_CONNECTION_H

#include <QWidget>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui { class Connection; }
QT_END_NAMESPACE

class Connection : public QWidget {
Q_OBJECT

public:
    explicit Connection(QWidget *parent = nullptr);

    ~Connection() override;

private:
    Ui::Connection *ui;

private slots:
    void connectToServer();
};


#endif //LOGGERCLIENT_CONNECTION_H
