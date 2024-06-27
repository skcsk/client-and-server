#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include "tcpclient.h"
#include  <QMessageBox>
PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
    ui->showMsg_te->setReadOnly(true);

}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::setChatName(QString strName)
{
    this->m_strChatName=strName;
    this->m_strLoginName=TcpClienT::getInstance().loginName();
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::updateMsg(const PDU *pdu)
{
    if(pdu==nullptr){
        return;
    }
    char caSendName[32]={'\0'};
    memcpy(caSendName,pdu->caData,32);
    QString strMsg =QString("%1 says: %2").arg(caSendName).arg((char*)pdu->caMsg);
    ui->showMsg_te->append(strMsg);
}

void PrivateChat::on_send_Msg_pb_clicked()
{

    QString strMsg =ui->input_Msg_le->text();
    qDebug()<<strMsg;
    if(!strMsg.isEmpty())
    {
        PDU *pdu=mkPDU(strMsg.size()+128);
        pdu->uiMsgType=ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        memcpy(pdu->caData,m_strLoginName.toStdString().c_str(),m_strLoginName.size());
        memcpy(pdu->caData+32,m_strChatName.toStdString().c_str(),m_strChatName.size());

        strcpy((char*)pdu->caMsg,strMsg.toStdString().c_str());
        pdu->caMsg[strMsg.size()] = '\0';
        TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        this->updateMsg(pdu);
        if(pdu!=nullptr){
            free(pdu);
            pdu=nullptr;
        }
        ui->input_Msg_le->clear();
    }
    else{
        QMessageBox::warning(this,"发送信息","发送信息不能为空");
    }
}

