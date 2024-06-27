#include "tcpclient.h"
#include "ui_tcpclient.h"


#include "privatechat.h"
#include "student_check_in.h"
TcpClienT::TcpClienT(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClienT)
    ,re("^[^\\s]{1,21}$")
{
    downBool=false;
    ui->setupUi(this);
    loadConfig();
    //OpeWidget::getInstance().getBook();
    //连接服务器
    m_tcpSocket.connectToHost(QHostAddress(this->m_strIP),
                              this->m_usPort);

    //服务器连接信号槽
    connect(&m_tcpSocket,&QTcpSocket::connected,
            this,&TcpClienT::showConnect);
    //关联信号槽服务器发来信息触发recvMsg槽函数
    connect(&m_tcpSocket,&QTcpSocket::readyRead,
            this,&TcpClienT::recvMsg);


}

TcpClienT::~TcpClienT()
{
    delete ui;
}

void TcpClienT::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData= file.readAll();
        QString strData=baData.toStdString().c_str();
        file.close();
        strData.replace("\r\n"," ");
        QStringList strList=strData.split(" ");

        this->m_strIP=strList.at(0);
        this->m_usPort=strList.at(1).toUShort();
        qDebug()<<m_strIP<<m_usPort;
    }
}

TcpClienT &TcpClienT::getInstance()
{
    static TcpClienT instance;
    return instance;
}

QTcpSocket &TcpClienT::getTcpSocket()
{
    return this->m_tcpSocket;
}

QString TcpClienT::loginName()
{
    return m_strLoginName;
}

QString TcpClienT::curPath()
{
    return this->m_usCurPath;
}

void TcpClienT::setCurPath(QString strCurPath)
{
    this->m_usCurPath = strCurPath;
}

void TcpClienT::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}


void TcpClienT::recvMsg()
{

    //bool ret=OpeWidget::getInstance().getBook()==nullptr?false:OpeWidget::getInstance().getBook()->downLoad();
    bool ret=this->downBool;
    qDebug()<<ret;
    if(ret==false)

    //if(true)
    {
        //qDebug()<<m_tcpSocket.bytesAvailable();


        uint uiPDULen =0;
        m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen =uiPDULen - sizeof(PDU);
        PDU *pdu =mkPDU(uiMsgLen);
        pdu->uiPDULen=uiPDULen;
        m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));


        switch (pdu->uiMsgType)
        {
        case ENUM_MSG_TYPE_REGIST_RESPOND:{
            if(0==strcmp(pdu->caData,REGIST_OK)){
                QMessageBox::information(this,"注册",REGIST_OK);


            }
            else if(0==strcmp(pdu->caData,REGIST_FAILED)){
                QMessageBox::warning(this,"注册",REGIST_FAILED);
            }


            break;
        }
        case ENUM_MSG_TYPE_LOGIN_RESPOND:{
        if(0==strcmp(pdu->caData,LOGIN_ERROR_UNKNOWN)){
            QMessageBox::information(this,"登录",LOGIN_ERROR_UNKNOWN);
        }
        else if(0==strcmp(pdu->caData,LOGIN_FAILED_ALREADY_LOGGED_IN)){
            QMessageBox::warning(this,"登录",LOGIN_FAILED_ALREADY_LOGGED_IN);
        }
        else if(0==strcmp(pdu->caData,LOGIN_FAILED_USER_NOT_FOUND)){
            QMessageBox::information(this,"登录",LOGIN_FAILED_USER_NOT_FOUND);
        }
        else if(0==strcmp(pdu->caData,LOGIN_SUCCESS)){
            QMessageBox::information(this,"登录",LOGIN_SUCCESS);

            m_usCurPath=QString("./%1").arg(loginName());
            OpeWidget::getInstance().setWindowTitle(loginName());
            OpeWidget::getInstance().resize(500,270);
            OpeWidget::getInstance().show();

            this->hide();


        }
        break;
    }
        case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:{

        OpeWidget::getInstance().pFriend()->showAllOnlineUsr(pdu);
        break;
    }
        case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:{

        QMessageBox::information(this,
                                 QString("用户:%1").arg(OpeWidget::getInstance().pFriend()->m_strSearchName),
                                                           pdu->caData);
#if 0
        else if(0==strcmp(pdu->caData,pdu->caData)){
            QMessageBox::warning(this,
                                 QString("%1用户").arg(OpeWidget::getInstance().pFriend()->m_strSearchName),
                                        pdu->caData);
        }
        else if(0==strcmp(pdu->caData,SEARCH_USR_ONLINE)){
            QMessageBox::information(this,
                                     QString("%1用户").arg(OpeWidget::getInstance().pFriend()->m_strSearchName),
                                        pdu->caData);
        }
#endif
        break;
    }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{

        char caName[32]={'\0'};
        char caPerName[32]={'\0'};
        strncpy(caName,pdu->caData+32,32);
        strncpy(caPerName,pdu->caData,32);
        qDebug()<<caName<<caPerName;
        int ret = QMessageBox::question(
            this,
            tr("好友请求"),
        QString("%1 请求添加你为好友").arg(caName),
            QMessageBox::Yes | QMessageBox::No
            );
        PDU *respdu=mkPDU(0);
        memcpy(respdu->caData, caPerName, 32);
        memcpy(respdu->caData + 32, caName, 32);
        if(ret==QMessageBox::Yes){
            //同意添加好友
            qDebug()<<caName<<"发起好友请求";
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
        }
        else{
            //不同意
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
        }
        m_tcpSocket.write((char*)respdu,respdu->uiPDULen);

        if(respdu!=nullptr){
            free(respdu);
            respdu=nullptr;

        }
        break;
    }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
        QMessageBox::information(this,"添加好友",pdu->caData);
        break;
    }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{
        qDebug()<<"明敏是刷新啊，怎么调用这个叼毛了";
        OpeWidget::getInstance().pFriend()->updateFriendList(pdu);

        break;
    }
        case ENUM_MSG_TYPE_DEL_FRIEND_RESPOND:{
        if (strcmp(pdu->caData, DEL_FRIEND_SUCCESS) == 0){
        QMessageBox::information(this,"删除好友",pdu->caData);
        OpeWidget::getInstance().pFriend()->flushFriend();
    }
        else{
        QMessageBox::information(this,"删除好友",pdu->caData);
        }
        break;
    }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{

        char caSendName[32]={'\0'};
        memcpy(caSendName,pdu->caData,32);
        PrivateChat::getInstance().setChatName((QString)caSendName);
        qDebug()<<"交互";
        if(PrivateChat::getInstance().isHidden()){
        PrivateChat::getInstance().setWindowTitle(caSendName);
        PrivateChat::getInstance().show();

        }
        PrivateChat::getInstance().updateMsg(pdu);

        break;
    }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:{
        OpeWidget::getInstance().pFriend()->updateGroupMsg(pdu);
        break;
    }
        case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:{
        QMessageBox::information(this,"创建文件",pdu->caData);




        break;
    }
        case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:{
        OpeWidget::getInstance().getBook()->updateFileList(pdu);
        QString enterDir = OpeWidget::getInstance().getBook()->getEnterDir();
        if(!enterDir.isEmpty()){
        this->m_usCurPath = m_usCurPath +  + "/" + enterDir;
        qDebug()<<m_usCurPath;
        }
        OpeWidget::getInstance().getBook()->clearEnterDir();



        break;
    }
        case ENUM_MSG_TYPE_DEL_DIR_RESPOND:{
        QMessageBox::information(this,"删除文件",pdu->caData);

        break;
    }
        case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:{
        QMessageBox::information(this,"重命名文件",pdu->caData);


        break;
    }
        case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:{
        //进入目录失败
        OpeWidget::getInstance().getBook()->clearEnterDir();
        QMessageBox::information(this,"进入目录","that dir not exist");


        break;
    }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:{
        QMessageBox::information(this,"上传文件",pdu->caData);


        break;
    }
        case ENUM_MSG_TYPE_DEL_FILE_RESPOND:{
        QMessageBox::information(this,"删除文件",pdu->caData);

        break;
    }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:{
        qDebug()<<pdu->caData;
        char caFileName[32]={'\0'};

        sscanf(pdu->caData,"%s %lld",caFileName,&(OpeWidget::getInstance().getBook()->m_iTotal));
        if(strlen(caFileName)>0 && OpeWidget::getInstance().getBook()->m_iTotal>0)
        {
        this->downBool=true;//我知道你在找
        OpeWidget::getInstance().getBook()->setDownLoadStatus(true);//开始下载
        m_file.setFileName(OpeWidget::getInstance().getBook()->strSaveFilePath());
        qDebug()<<"strSaveFilePath:"<<OpeWidget::getInstance().getBook()->strSaveFilePath();
        if(!m_file.open(QIODevice::WriteOnly)){
            QMessageBox::warning(this,"下载文件","下载文件失败");
            qDebug()<<"失败了臭傻逼";
        }

        }
        OpeWidget::getInstance().getBook()->setDownLoadStatus(true);

        break;

        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE:{
        char  sendName[32]={0};

        char  curpath[256]={0};
        strncpy(sendName,pdu->caData,32);

        memcpy(curpath,pdu->caMsg,pdu->uiMsgLen+1);
        int lastIndex = QString(curpath).lastIndexOf('/');
        QString file = QString(curpath).mid(lastIndex + 1);

        int ret = QMessageBox::question(
            this,
            tr("接收文件"),
            QString("%1 向你转发 %2").arg(sendName).arg(file),
            QMessageBox::Yes | QMessageBox::No
            );
        if(ret==QMessageBox::Yes){
            qDebug()<<"success";
//            PDU *respdu = mkPDU(pdu->uiMsgLen);
//            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
//            strncpy(respdu->caData,this->loginName(),32);
//            memcpy((char*)respdu,(char*)pdu->caMsg,pdu->uiMsgLen);

//            getTcpSocket().write((char*)respdu,pdu->uiPDULen);
//            if(respdu!=nullptr){
//                free(respdu);
//                respdu=nullptr;
            pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            }
        else{
            qDebug()<<"failure";
        }

        break;

        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
        QMessageBox::information(this,"接收文件",pdu->caData);
        break;
        }
        case ENUM_MSG_TYPE_LOGOUT_RESPOND:{
        QMessageBox::information(this,"注销账号",pdu->caData);
        break;
        }
        case ENUM_MSG_TYPE_SIGNIN_RESPOND:{

            QMessageBox::information(this,"签到",pdu->caData);

            break;

        }
        case ENUM_MSG_TYPE_VIEW_CHECK_INS_RESPOND:{
            //OpeWidget::getInstance().pFriend()->updateFriendList(pdu);
            Student_Check_In::getInstance().updateList(pdu);
        }




        default:

        break;

        }



        if(pdu!=NULL){
        free(pdu);
        pdu=NULL;
        }
    }
    else
    {
        QByteArray buffer =  m_tcpSocket.readAll();
        m_file.write(buffer);
        Book *pBook = OpeWidget::getInstance().getBook();
        pBook->m_iRecved +=buffer.size();
        qDebug()<<"正在写入"<<double(pBook->m_iRecved/(double)pBook->m_iTotal);
        qDebug()<<"m_iRecved:"<<pBook->m_iRecved;
        qDebug()<<"m_iTotal:"<<pBook->m_iTotal;
        if(pBook->m_iTotal == pBook->m_iRecved){
            m_file.close();
            pBook->m_iRecved=0;
            pBook->m_iTotal=0;
            pBook->setDownLoadStatus(false);
            QMessageBox::information(this,"下载文件","下载文件成功");
            this->downBool=false;//我知道你在找
            qDebug()<<"下载文件成功";

        }else if(pBook->m_iTotal <pBook->m_iRecved){
            m_file.close();
            pBook->m_iRecved=0;
            pBook->m_iTotal=0;
            pBook->setDownLoadStatus(false);
            this->downBool=false;//我知道你在找
            QMessageBox::critical(this,"下载文件","下载文件失败");
            qDebug()<<"下载文件失败";
        }
    }
}



void TcpClienT::on_login_pb_clicked()//登录按钮
{

    QString strName =ui->name_le->text();
    QString strPwd =ui->pwd_le->text();
    if(strName.isEmpty()||strPwd.isEmpty()){
        QMessageBox::critical(this,"登录","登陆失败:用户名或密码为空");
        return;
    }
    //正则表达式判断输入是否合法

    if (!re.match(strName).hasMatch() ||
        !re.match(strPwd).hasMatch()) {
        QMessageBox::warning(this, "登录", "登录失败：用户名或密码不符合要求");
        return;
    }
    else{
        m_strLoginName = strName;
        PDU* pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);

        if(pdu!=NULL)
        {
            free(pdu);
            pdu=NULL;
        }
    }
}


void TcpClienT::on_regist_pb_clicked()//注册按钮
{
    QString strName =ui->name_le->text();
    QString strPwd =ui->pwd_le->text();
    //判断是否符合条件用正则表达式实现
    // 使用正则表达式验证用户名和密码
    // 不包含空格，长度1到21位
    if (!re.match(strName).hasMatch() ||
        !re.match(strPwd).hasMatch()) {
        QMessageBox::warning(this, "注册", "注册失败：用户名或密码不符合要求");
        return;
    }

    if(strName.isEmpty()||strPwd.isEmpty()){
        QMessageBox::critical(  this,"注册","注册失败：用户名或者密码为空");
        return;
    }
    else
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        if(pdu!=NULL){
            free(pdu);
            pdu=NULL;
        }

    }
}


void TcpClienT::on_cancel_b_clicked()//注销按钮
{
    QString strName =ui->name_le->text();
    QString strPwd =ui->pwd_le->text();
    if(strName.isEmpty()||strPwd.isEmpty()){
        QMessageBox::warning(this,"注销账号","请输入账号密码");
    }
    int ret=QMessageBox::question(this,"注销账号","你确定注销吗",QMessageBox::Yes,QMessageBox::No);
    if(ret==QMessageBox::No){
        return;
    }

    PDU *pdu=mkPDU(0);
    strncpy(pdu->caData,strName.toStdString().c_str(),32);

    strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
    pdu->uiMsgType = ENUM_MSG_TYPE_LOGOUT_REQUEST;
    m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
    if(pdu!=NULL){
        free(pdu);
        pdu=NULL;
    }

}


