#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QString>
#include <QWidget>
#include <QFile>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig();

private:
    Ui::TcpServer *ui;

    QString m_strIP;//ip
    quint16 m_usPort;//端口号
};
#endif // TCPSERVER_H
