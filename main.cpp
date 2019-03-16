#include <QtGui/QApplication>
#include "tablewindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("bang");

    QApplication a(argc, argv);
    TableWindow w;
    w.show();

    return a.exec();
}
