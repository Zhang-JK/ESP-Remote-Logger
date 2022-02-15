/**
 * This file is used for the Dashboard page.
 * It contains the data form CAN bus and development board.
 */

#ifndef LOGGERCLIENT_DASHBOARD_H
#define LOGGERCLIENT_DASHBOARD_H

#include <QWidget>
#include <QStandardItemModel>


QT_BEGIN_NAMESPACE
namespace Ui { class Dashboard; }
QT_END_NAMESPACE

class Dashboard : public QWidget {
Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

    ~Dashboard() override;

private slots:
    void fullScreen();
    void udpDecode(const QList<int>& data);
    void receiveRate(int updated);

private:
    Ui::Dashboard *ui;
    QStandardItemModel *model;
};


#endif //LOGGERCLIENT_DASHBOARD_H
