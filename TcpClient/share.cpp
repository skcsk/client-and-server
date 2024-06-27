#include "share.h"
#include "ui_share.h"
#include "opewidget.h"
#include "tcpclient.h"
Share::Share(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Share)
{
    ui->setupUi(this);
}

Share::~Share()
{
    delete ui;
}

Share &Share::getInstance()
{
    static Share instance;
    return instance;
}

void Share::on_cancel_PB_clicked()
{
    this->hide();
}


void Share::on_shareOK_PB_clicked()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if(item ==nullptr){
        QMessageBox::warning(this,"分享文件","请选择分享对象");
        return;
    }
    QString strShareName = item->text();



    QString strName = TcpClienT::getInstance().loginName();//获取分享者名字
    QString strCurPath = TcpClienT::getInstance().curPath();//获取当前路径
    QString strShareFileName            //获取分享文件名
        =OpeWidget::getInstance().getBook()->strShareFileName;

    QString strPath = strCurPath + '/' + strShareFileName;//拼接路径






    //创建并发送pdu
    PDU *pdu = mkPDU(strPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    strncpy(pdu->caData,strName.toStdString().c_str(),32);
    strncpy(pdu->caData + 32, strShareName.toStdString().c_str(), 32);
    memcpy((char*)pdu->caMsg,strPath.toStdString().c_str(),strPath.size()+1);

    TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);


    char  a[32]={0};
    char  b[32]={0};
    char  c[128]={0};
    strncpy(a,pdu->caData,strName.size());
    strncpy(b, pdu->caData+32, 32);
    memcpy(c,pdu->caMsg,strPath.size()+1);
    qDebug()<<"分享者："<<QString(a);
        qDebug()<<"接受者："<<QString(b);
        qDebug()<<"分享路径："<<QString(c);

}


void Share::on_flushFriend_PB_clicked()
{
    ui->listWidget->clear();
    QListWidget *friendList = OpeWidget::getInstance().pFriend()->pFriendListWidget();
    for (int i = 0; i < friendList->count(); i++) {
        QListWidgetItem *item = friendList->item(i);

        // 创建一个新的列表项，其文本与好友列表中的当前项相同
        QListWidgetItem *newItem = new QListWidgetItem(item->text());

        // 将新创建的项添加到 Share 窗口中的 listWidget
        ui->listWidget->addItem(newItem);
    }
}

