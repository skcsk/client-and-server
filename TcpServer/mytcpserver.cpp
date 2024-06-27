#include "mytcpserver.h"
#include <QDebug>
MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<"new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    this->m_tcpSocket.append(pTcpSocket);
    connect(pTcpSocket,&MyTcpSocket::offline,this,&MyTcpServer::deleteSocket);
}

void MyTcpServer::resend(const char *pername, PDU *pdu)
{
    if(pername==nullptr||pdu==nullptr){
        return;
    }
    //遍历 QList<MyTcpSocket*> m_tcpSocket;找到对应的名字
    for (MyTcpSocket* socket : m_tcpSocket) {
        if (socket->strName() == QString(pername)) {
            // 找到了对应的用户套接字，发送 PDU
            //qDebug()<<"发送添加好友请求";
            socket->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *mySocket)
{
    if (mySocket ==nullptr ) {
        return;
    }
    for (MyTcpSocket* socket : m_tcpSocket) {
        if (socket != nullptr) { // 确保指针不为空
            QString name = socket->strName(); // 调用 MyTcpSocket 的 strName() 方法
            qDebug() << "Socket name:" << name;
        }
    }


    m_tcpSocket.removeAll(mySocket);
    mySocket->deleteLater(); // 安全删除 socket


}
