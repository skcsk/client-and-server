#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"
Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if(pdu==nullptr){
        return;
    }
    uint uiSize =pdu->uiMsgLen/32;
    char caTmp[33];
    for(uint i=0;i<uiSize;i++){
        memcpy(caTmp,(char*)(pdu->caMsg)+i*32,32);
        caTmp[32] = '\0';  // 确保字符串以空字符结尾
        //    ui->listWidget->addItem(caTmp);
        if((QString)caTmp==TcpClienT::getInstance().loginName()){
            continue;
        }

        ui->listWidget->addItem(caTmp);

    }

}
void Online::hideEvent(QHideEvent *event){
    // 在窗口隐藏前执行的代码
    ui->listWidget->clear();  // 例如，清空列表

    QWidget::hideEvent(event); // 调用基类的 hideEvent 以保持正常的隐藏行为
}

//添加好友槽函数
void Online::on_addFriend_pb_clicked()
{
    // 获取当前选中的列表项
    QListWidgetItem *currentItem = ui->listWidget->currentItem();

    // 确保有选中的项
    if (currentItem != nullptr) {
        QString strPerUsrName = currentItem->text();
        QString strLoginName=TcpClienT::getInstance().loginName();
        qDebug() << "Selected name for adding friend:" << strPerUsrName;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
        memcpy(pdu->caData,strPerUsrName.toStdString().c_str(),strPerUsrName.size());
        pdu->caData[31] = '\0';  // 确保字符串结束
        memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),strLoginName.size());
        pdu->caData[63] = '\0';  // 确保字符串结束
        TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        if(pdu!=nullptr)
        {
            free(pdu);
            pdu=nullptr;
        }
        // 数据库添加好友
        //代写
        // OpeDB::getInstance().addFriend(selectedName);
    } else {
        qDebug() << "No item selected for adding friend.";
    }
}

