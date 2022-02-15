#include <QApplication>
#include <QPushButton>
#include <QWidget>

#include "ConnectionPage/Connection.h"
#include "DataPage/Dashboard.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Connection c;
    Dashboard d;

    c.show();
    d.hide();
    Connection::connect(&c, SIGNAL(showDashboard()), &d, SLOT(show()));
    return QApplication::exec();
}
