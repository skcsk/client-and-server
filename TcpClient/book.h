#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "protocol.h"
#include <QTimer>
class Book : public QWidget
{
    Q_OBJECT
public:

    void updateFileList(PDU *pdu);

    void clearEnterDir();//清除进入函数
    explicit Book(QWidget *parent = nullptr);
    QString getEnterDir();
    void setDownLoadStatus(bool status);

    qint64 m_iTotal;//总的文件大小
    qint64 m_iRecved;//已收到多少
    QString strShareFileName;
    bool downLoad() const;

    QString strSaveFilePath() const;

public slots:
    void createDir();//创建文件槽函数
    void flushFile();//刷新文件槽函数
    void DelDir();//删除文件夹槽函数
    void remaneFile();//重命名槽函数
    void enterDir(const QModelIndex &index);//进入文件槽函数
    void returnPer();//返回上一级
    void upLoadFile();//上传常规文件文件
    void upLoadFileData();//发送文件
    void delFile();//删除常规文件
    void downLoadFile();//下载文件槽函数

    void shareFile();


signals:

private:
    QString m_strEnterDir;//准备进入的目录
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDelDirPB;
    QPushButton *m_pRenamePB;
    QPushButton *m_pFlushPB;
    QPushButton *m_pUploadPB;
    QPushButton *m_pDownLoadPB;
    QPushButton *m_pDelFilePB;
    QPushButton *m_pShareFilePB;
    QString m_strUploadFilePath;
    QTimer *m_pTimer;

    QString m_strSaveFilePath;
public:
    bool m_downLoad;//是否处于下载状态
};

#endif // BOOK_H
