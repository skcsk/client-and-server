#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include "opedb.h"
#include <QFile>
#include <QTimer>
#include "filereceivertask.h"
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString strName() const;
    bool starkReceiveFile();
    void stark();
signals:
    //下线信号
    void offline(MyTcpSocket * mySocket);

    //开辟线程接收文件信号
    void receiveFile();
public slots:
    //接受信息槽函数
    void recvMsg();

    /**
     * @brief 客户端下线槽函数
     */
    void clientOffline();


    void sendFileToClient();
private:
    //登录用户名字
    QString m_strName;

    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iReceved;
    bool m_bUploadt;
    QTimer *m_pTimer;

    FileReceiverTask *task;
};

#endif // MYTCPSOCKET_H
