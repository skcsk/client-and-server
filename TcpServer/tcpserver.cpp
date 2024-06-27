#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"

#include <QHostAddress>


TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();


    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData= file.readAll();
        QString strData=baData.toStdString().c_str();
        file.close();
        strData.replace("\r\n"," ");
        QStringList strList=strData.split(" ");

        this->m_strIP=strList.at(0);
        this->m_usPort=strList.at(1).toUShort();
        qDebug()<<m_strIP<<m_usPort;
    }
}

