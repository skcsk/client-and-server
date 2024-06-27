#include "filereceivertask.h"
#include "protocol.h"
#include <QFile>
#include <QDebug>
FileReceiverTask::FileReceiverTask(QTcpSocket *socket,QString fileName,qint64 iTotal): m_socket(socket)
{
    m_fileName = fileName;
    m_iTotal = iTotal;
    m_iReceved = 0;
}

void FileReceiverTask::run()
{
    //PDU *respdu=nullptr;
    QFile file(m_fileName);
    if(!file.open(QIODevice::WriteOnly)){
        //m_bUploadt = true;
        //m_iTotal = fileSize;
        //m_iReceved = 0;
        //emit receiveFile();
    }

    emit stark();
    /*while (m_socket.state() == QTcpSocket::ConnectedState){

        QByteArray buff = m_socket.readAll();
        file.write(buff);
        m_iReceved +=buff.size();
        if(m_iTotal==m_iReceved){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            file.close();
            this->m_iTotal=0;
            this->m_iReceved=0;
            //m_bUploadt = false;


            //strcpy(respdu->caData,UPLOAD_FILE_OK);
            //write((char*)respdu,respdu->uiPDULen);
            break;
        }
        else if(m_iTotal < m_iReceved){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strcpy(respdu->caData,UPLOAD_FILE_FAILURED);
            //write((char*)respdu,respdu->uiPDULen);
            file.close();
            this->m_iTotal=0;
            this->m_iReceved=0;
            //m_bUploadt = false;
            break;
        }
        qDebug()<<"a接收文件 "<<this->m_iReceved/this->m_iTotal;

        if(respdu!=nullptr){
            free(respdu);
            respdu=nullptr;
        }
    }
   */
}
