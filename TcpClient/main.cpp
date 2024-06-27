#include "tcpclient.h"

#include <QApplication>

#include "privatechat.h"//测试用
#include "book.h"//测试用
int main(int argc, char *argv[])
{
        QApplication a(argc, argv);
        TcpClienT::getInstance().resize(300,200);


        TcpClienT::getInstance().show();

//    PrivateChat w;
//    w.show();
//        ShareFile w;
//        w.show();
    return a.exec();
}
