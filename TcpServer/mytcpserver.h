#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <list>
#include "mytcpsocket.h"
class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();
    static MyTcpServer& getInstance();
    void incomingConnection(qintptr socketDescriptor);


    void resend(const char* pername,PDU *pdu);
signals:


public slots:
    //在socketlist删除下线socket
    void deleteSocket(MyTcpSocket *mySocket);
private:
    QList<MyTcpSocket*> m_tcpSocket;
};

#endif // MYTCPSERVER_H
