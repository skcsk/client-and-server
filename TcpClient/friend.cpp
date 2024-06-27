#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include "privatechat.h"
#include "student_check_in.h"
Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE = new QTextEdit(this);
    m_pFriendListWidget = new QListWidget(this);

    m_pOnline =new Online;
    m_pOnline->hide();

    m_pInputMsgLE = new QLineEdit(this);
    m_pDelFriendPB = new QPushButton(tr("删除好友"), this);
    m_pFlushFriendPB = new QPushButton(tr("查看班级学生(老师)"), this);

    m_pSignInPB= new QPushButton(tr("签到"), this);
    m_pShowLogInPB = new QPushButton(tr("查看学生签到情况"), this);

    m_pShowOnlineUsrPB = new QPushButton(tr("显示在线用户"), this);
    m_pSearchUsrPB = new QPushButton(tr("查找用户"), this);
    m_pMsgSendPB =  new QPushButton(tr("群发送信息"),this);
    m_pPrivateChatPB =  new QPushButton(tr("私聊"),this);

#if 0
    QVBoxLayout *pRightLayout = new QVBoxLayout();  // 主要内容布局
    QVBoxLayout *pLeftPB = new QVBoxLayout();  // 按钮布局
    QHBoxLayout *pMain = new QHBoxLayout();  // 主布局

    // 将按钮添加到左侧布局
    pLeftPB->addWidget(m_pDelFriendPB);
    pLeftPB->addWidget(m_pFlushFriendPB);
    pLeftPB->addWidget(m_pShowOnlineUsrPB);
    pLeftPB->addWidget(m_pSerchUsrPB);
    pLeftPB->addWidget(m_PrivateChatPB);
    pLeftPB->addWidget(m_pMsgSendPB);

    // 将信息显示和好友列表添加到顶部水平布局
    QHBoxLayout *pTopHBL = new QHBoxLayout();
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);

    // 在右侧布局中添加顶部布局、输入框和 Online 控件
    pRightLayout->addLayout(pTopHBL);
    pRightLayout->addWidget(m_pInputMsgLE);


    // 将左侧布局和右侧布局添加到主布局
    pMain->addLayout(pLeftPB);
    pMain->addLayout(pRightLayout);
    pMain->addWidget(m_pOnline);

    setLayout(pMain);  // 设置主布局
#endif
    QVBoxLayout *pRightPBVBL =new QVBoxLayout;
    pRightPBVBL->addWidget(m_pSignInPB);
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pShowLogInPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    QVBoxLayout *pMain= new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    setLayout(pMain);


    connect(this->m_pShowOnlineUsrPB,&QPushButton::clicked,
            this,&Friend::showOnlien);
    connect(this->m_pSearchUsrPB,&QPushButton::clicked,
            this,&Friend::searchUsr );
    connect(this->m_pFlushFriendPB,&QPushButton::clicked,
            this,&Friend::flushFriend);
    connect(this->m_pDelFriendPB,&QPushButton::clicked,
            this,&Friend::delFriend);
    connect(this->m_pPrivateChatPB,&QPushButton::clicked,
            this,&Friend::privateChat);
    connect(this->m_pMsgSendPB,&QPushButton::clicked,
            this,&Friend::groupChat);
    connect(this->m_pShowLogInPB,&QPushButton::clicked,
            this,&Friend::showLoginstudent);
    connect(this->m_pSignInPB,&QPushButton::clicked,
            this,&Friend::singIn);


    this->flushFriend();

}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(pdu==nullptr){
        return;
    }
    m_pOnline->showUsr(pdu);
}


void Friend::showOnlien()
{
    if (m_pOnline->isHidden()) {
        m_pOnline->show();
        PDU * pdu = mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClienT::getInstance().getTcpSocket().write((char*)pdu,
                                                      pdu->uiPDULen);
        if(pdu!=nullptr){
            free(pdu);
            pdu=nullptr;
        }
    }
    else {
        m_pOnline->hide();
    }
}

void Friend::flushFriend()
{


    QString strName =TcpClienT::getInstance().loginName();
    //QMessageBox::information(this,"刷新","刷新成功");
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;

    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    if(pdu!=nullptr){
        free(pdu);
        pdu=nullptr;
    }

}

void Friend::delFriend()
{
    // 获取当前选中的列表项

    QListWidgetItem *currentItem = this->m_pFriendListWidget->currentItem();
    QString strPerName;
    QString strName=TcpClienT::getInstance().loginName();//自己的名字
    if(currentItem==nullptr){
        QMessageBox::information(this,"删除好友","请选择要删除的好友");
        return;

    }
    strPerName=currentItem->text();
    if(strPerName.isEmpty()||strName.isEmpty()){
        return;
    }
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_DEL_FRIEND_REQUEST;
    memcpy(pdu->caData,strPerName.toStdString().c_str(),strPerName.size());
    memcpy(pdu->caData+32,strName.toStdString().c_str(),strName.size());
    TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

    if(pdu!=nullptr){
        free(pdu);
        pdu=nullptr;
    }
}

void Friend::privateChat()
{
    if(nullptr == m_pFriendListWidget->currentItem()){
        QMessageBox::information(this,"私聊","请选择私聊对象");
        return;
    }
    QString strChatName= m_pFriendListWidget->currentItem()->text();
    PrivateChat::getInstance().setChatName(strChatName);
    if(PrivateChat::getInstance().isHidden()){
        PrivateChat::getInstance().setWindowTitle(strChatName);
        PrivateChat::getInstance().show();
    }
}

void Friend::groupChat()
{

    QDateTime currentDateTime = QDateTime::currentDateTime();

    // 将日期和时间转换为字符串
    QString dateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");


    QString strMsg =m_pInputMsgLE->text();
    strMsg = "("+dateTimeString+")"+strMsg;
    if(!strMsg.isEmpty()){
        PDU *pdu=mkPDU(strMsg.size()+128);
        pdu->uiMsgType=ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
        QString strName=TcpClienT::getInstance().loginName();
        strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
        strncpy((char*)pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size()+128);
        TcpClienT::getInstance().getTcpSocket().write((char*)pdu,
                                                      pdu->uiPDULen);
        if(pdu!=nullptr){
            free(pdu);
            pdu=nullptr;
        }

        m_pShowMsgTE->append(QString("%1 says: %2").arg(TcpClienT::getInstance().loginName()).arg(strMsg));
        m_pInputMsgLE->clear();
    }
    else{
        QMessageBox::information(this,"群聊","发送信息不能为空");
    }
}

//查看学生签到情况槽函数
void Friend::showLoginstudent()
{
    QString strName =TcpClienT::getInstance().loginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_VIEW_CHECK_INS_REQUEST;

    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

    qDebug()<<pdu->caData<<"请求查看签到情况";

    if(Student_Check_In::getInstance().isHidden()){
        Student_Check_In::getInstance().show();
    }
    //Student_Check_In::getInstance().hide();
    qDebug()<<"查看签到情况";
}

//签到槽函
void Friend::singIn()
{
    QString strName =TcpClienT::getInstance().loginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_SIGNIN_REQUEST;

    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

    qDebug()<<pdu->caData<<"客户端请求签到";
    if(pdu!=nullptr){
        free(pdu);
        pdu=nullptr;
    }
}

QListWidget *Friend::pFriendListWidget() const
{
    return m_pFriendListWidget;
}

void Friend::searchUsr()
{

    bool ok;
     m_strSearchName=QInputDialog::getText(this,"搜索","用户名", QLineEdit::Normal, QString(), &ok);
    if(!ok){
        return;

     }
    if(!m_strSearchName.isEmpty()){
        //qDebug()<<name;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());

        TcpClienT::getInstance().getTcpSocket().write((char*)pdu,
                                                      pdu->uiPDULen);
        if(pdu!=nullptr){
            free(pdu);
            pdu=nullptr;
        }
    }
}

void Friend::updateFriendList(PDU *pdu)
{
    if(pdu==nullptr){
        return;
    }
    uint uiSize =pdu->uiMsgLen/32;
    char caName[33];
    m_pFriendListWidget->clear();
    for(uint i=0;i<uiSize;i++){
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        caName[32] = '\0';  // 确保字符串以空字符结尾
       m_pFriendListWidget->addItem(caName);

    }
}

void Friend::updateGroupMsg(PDU *pdu)
{
    QString strMsg =QString("%1 says: %2").arg(pdu->caData).arg((char*)pdu->caMsg);
    m_pShowMsgTE->append(strMsg);
}


