#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QWidget>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include "protocol.h"
#include "opewidget.h"
#include <QRegularExpression>
QT_BEGIN_NAMESPACE
namespace Ui { class TcpClienT; }
QT_END_NAMESPACE

class TcpClienT : public QWidget
{
    Q_OBJECT

public:
    TcpClienT(QWidget *parent = nullptr);
    ~TcpClienT();
    void loadConfig();
    static TcpClienT& getInstance();
    QTcpSocket &getTcpSocket();
    QString loginName();
    QString curPath();
    void setCurPath(QString strCurPath);


    bool downBool;

    QRegularExpression re;



public slots:


    /**
     * @brief 连接服务器成功槽函数
     */
    void showConnect();

    /**
     * @brief 接受服务器返回pdu槽函数
     */
    void recvMsg();
private slots:
 //   void on_send_pb_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_b_clicked();


private:
    Ui::TcpClienT *ui;
    QString m_strIP;//ip
    quint16 m_usPort;//端口号

    //连接服务器
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;
    QFile m_file;


    //当前所在文件夹
    QString m_usCurPath;
};
#endif // TCPCLIENT_H
