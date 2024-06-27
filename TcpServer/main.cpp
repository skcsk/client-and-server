#include "tcpserver.h"
#include "opedb.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServer w;
    w.resize(300,200);
    w.show();
    return a.exec();
}
