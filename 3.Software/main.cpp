#include <QApplication>
#include <QPushButton>

#include "ConnectionPage/Connection.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Connection c;

    c.show();
    return QApplication::exec();
}
