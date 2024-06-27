#ifndef FILERECEIVERTASK_H
#define FILERECEIVERTASK_H

#include <QRunnable>
#include <QTcpSocket>

class FileReceiverTask :public QObject, public QRunnable
{
    Q_OBJECT
public:
    // 构造函数，接受一个QTcpSocket对象
    explicit FileReceiverTask(QTcpSocket *socket,QString fileName,qint64 iTotal);
    void run() override;
signals:
    void stark();
private:
    QTcpSocket m_socket;
    QString m_fileName;
    qint64 m_iTotal;
    qint64 m_iReceved;
};

#endif // FILERECEIVERTASK_H
