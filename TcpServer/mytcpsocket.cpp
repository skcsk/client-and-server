#include "mytcpsocket.h"
#include <QDebug>
#include <QDir>
#include "mytcpserver.h"
#include <QDir>
#include <QFileInfoList>
#include <QThreadPool>
#include "threadpool.h"
MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    task=nullptr;
    m_pTimer = new QTimer;
    connect(this,&MyTcpSocket::readyRead,
            this,&MyTcpSocket::recvMsg);
    connect(this,&MyTcpSocket::disconnected,
            this,&MyTcpSocket::clientOffline);
    connect(m_pTimer,&QTimer::timeout,
            this,&MyTcpSocket::sendFileToClient);
    connect(this,&MyTcpSocket::receiveFile,this,
            &MyTcpSocket::starkReceiveFile);

    connect(task,&FileReceiverTask::stark,
            this,&MyTcpSocket::stark);


    this->m_bUploadt = false;
}

void MyTcpSocket::recvMsg()
{
    //if(!m_bUploadt)

   {
        //qDebug()<<this->bytesAvailable();
        uint uiPDULen =0;
        this->read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen =uiPDULen - sizeof(PDU);
        PDU *pdu =mkPDU(uiMsgLen);
        pdu->uiPDULen=uiPDULen;
        this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
        //qDebug()<<pdu->uiMsgType<<" "<<(char*)(pdu->caMsg);


        //定义返回pdu

        switch (pdu->uiMsgType)
        {
            //注册请求
        case ENUM_MSG_TYPE_REGIST_REQUEST:{
            PDU* respdu;
            char caName[32]={'\0'};
            char caPwd[32]={'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            //qDebug()<<caName<<" "<<caPwd<<"  "<<pdu->uiMsgType;
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;

            //调用数据库注册用户函数
            if(OpeDB::getInstance().handleRegist(caName,caPwd)){
                //返回真，注册成功
                QDir dir;

                qDebug()<<"create dir:"<< dir.mkdir(QString("./%1").arg(caName));

                strcpy(respdu->caData,REGIST_OK);

            }
            else{
                //返回假，注册失败
                strcpy(respdu->caData,REGIST_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);

            if(respdu!=NULL){
                free(respdu);
                respdu=NULL;
            }
            break;
        }
        //登录请求
        case ENUM_MSG_TYPE_LOGIN_REQUEST:{
            char caName[32]={'\0'};
            char caPwd[32]={'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);

            PDU* respdu=mkPDU(0);

            respdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
            int ret=OpeDB::getInstance().handleLogin(caName,caPwd);
            //返回-1登录错误
            //返回1用户不存在或账号错误
            //返回2用户已经登录
            //返回3正常登录成功
            if(ret==-1){
                strcpy(respdu->caData,LOGIN_ERROR_UNKNOWN) ;
            }
            else if(ret==1){
                strcpy(respdu->caData,LOGIN_FAILED_USER_NOT_FOUND) ;
            }
            else if(ret==2){
                strcpy(respdu->caData,LOGIN_FAILED_ALREADY_LOGGED_IN) ;
            }
            else if(ret==3){
                strcpy(respdu->caData,LOGIN_SUCCESS) ;
                m_strName=caName;
            }

            write((char*)respdu,respdu->uiPDULen);
            if(respdu!=NULL){
                free(respdu);
                respdu=NULL;
            }

            break;
        }
            //显示所有在线用户请求
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:{
            QStringList ret=OpeDB::getInstance().handleAllOnline();
            uint len=ret.size()*32;
            PDU *respdu =mkPDU(len);
            respdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for(int i=0;i<ret.size();i++){
                memcpy((char*)(respdu->caMsg)+i*32,
                       ret.at(i).toStdString().c_str(),
                       ret.at(i).size());
            }
            write((char*)respdu,respdu->uiPDULen);
            if(respdu!=NULL){
                free(respdu);
                respdu=NULL;
            }
            break;
        }
            //查找好友请求
        case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:{
            int ret =OpeDB::getInstance().handleSearchUsr(pdu->caData);
            PDU * respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_RESPOND;

            if(ret==1){
                strcpy(respdu->caData,SEARCH_USR_ONLINE);

            }
            else if(ret==2)
            {
                strcpy(respdu->caData,SEARCH_USR_OFFLINE);

            }else if(ret==0){
                strcpy(respdu->caData,SEARCH_USR_NO);
            }
            write((char*)respdu,respdu->uiPDULen);
            if(respdu!=NULL){
                free(respdu);
                respdu=NULL;
            }

            break;
        }
            //添加好友请求
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
            PDU* respdu=mkPDU(0);

            char caPerName[32]={'\0'};
            char caName[32]={'\0'};
            strncpy(caPerName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            int ret = OpeDB::getInstance().handleAddFriend(caPerName,caName);

            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            if(ret==-1){
                strcpy(respdu->caData,UNKNOW_ERROR);
                qDebug()<<"添加好友错误：未知错误";
            }
            else if(ret==1){
                strcpy(respdu->caData,ADD_FRIEND_NOEXIST);
                qDebug()<<"添加好友错误：对方不存在";
            }
            else if(ret==2){
                strcpy(respdu->caData,EXISTED_FRIEND);
                qDebug()<<"添加好友：对方已经是好友";
            }
            else if(ret==3){
                qDebug()<<"添加好友：对方存在并在线";
                qDebug()<<caPerName;
                MyTcpServer::getInstance().resend(caPerName,pdu);
                break;
            }
            else if(ret==4){
                qDebug()<<"添加好友：对方存在但不在线";
            }

            write((char*)respdu,respdu->uiPDULen);


            if(respdu!=nullptr){
                free(respdu);
                respdu=nullptr;
            }
            break;
        }
            //同意添加好友
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:{
            char caName[32]={'\0'};
            char perName[32]={'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(perName,pdu->caData+32,32);
            //执行数据库添加好友操作
            int ret=OpeDB::getInstance().handleAgreeAddFriend(caName,perName);
            if(ret){
                qDebug()<<"添加成功";

            }
            else{
                qDebug()<<"添加失败";
            }
            break;
        }
            //刷新好友请求
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:{
            QStringList ret= OpeDB::getInstance().handleFlushFriend(pdu->caData);
            uint uiMsgLen=ret.size()*32;
            PDU *respdu=mkPDU(uiMsgLen);
            respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for(int i=0;i<ret.size();i++){
                memcpy((char*)(respdu->caMsg)+i*32
                       ,ret.at(i).toStdString().c_str()
                       ,ret.at(i).size());
            }


            write((char*)respdu,respdu->uiPDULen);
            if(respdu!=nullptr){
                free(respdu);
                respdu=nullptr;
            }
            break;
        }
        case ENUM_MSG_TYPE_DEL_FRIEND_REQUEST:{
            //数据删除好友请求
            PDU* respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_DEL_FRIEND_RESPOND;
            char caPerName[32]={'\0'};
            char caName[32]={'\0'};
            strncpy(caPerName,pdu->caData,31);
            strncpy(caName,pdu->caData+32,31);
            bool ret = OpeDB::getInstance().handleDelFriend(caPerName,caName);
            if(ret){
                strcpy(respdu->caData,DEL_FRIEND_SUCCESS);
            }
            else{
                strcpy(respdu->caData,DEL_FRIEND_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);

            if(respdu!=nullptr){
                free(respdu);
                respdu=nullptr;
            }
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{
            char caPerName[32]={'\0'};
            memcpy(caPerName,pdu->caData+32,32);
            MyTcpServer::getInstance().resend(caPerName,pdu);
            qDebug()<<caPerName;
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:{
            qDebug()<<"有人发送群聊信息";
            char caName[32]={'\0'};
            memcpy(caName,pdu->caData,32);
            QStringList onlineFriend= OpeDB::getInstance().handleFlushFriend(caName);
            QString tmp;
            for(int i=0;i<onlineFriend.size();i++)
            {
                qDebug()<<"有人转发信息";
                tmp=onlineFriend.at(i);

                MyTcpServer::getInstance().resend(tmp.toStdString().c_str(),pdu);
            }



            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:{

            QDir dir;
            QString strCurPath = QString("%1").arg((char*)pdu->caMsg);
            bool ret = dir.exists(strCurPath);
            qDebug()<<strCurPath;
            PDU *respdu=nullptr;
            respdu=mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            if(ret)//当前目录存在
            {
                char caNewDir[32]={'\0'};
                memcpy(caNewDir,pdu->caData+32,32);
                QString strNewPath = strCurPath+"/"+caNewDir;
                qDebug()<<strNewPath;
                ret =dir.exists(strNewPath);
                qDebug()<<"----->"<<ret;
                if(ret)//创建的文件吗已存在
                {
                    strcpy(respdu->caData,FILE_NAME_EXIST);
                }
                else//创建的文件吗不存在
                {
                    if(dir.mkdir(strNewPath))//创建成功
                    {
                        strcpy(respdu->caData,CREATE_DIR_OK);
                    }
                    else//创建失败
                    {
                        qDebug() << "目录创建失败";
                        strcpy(respdu->caData, CREATE_DIR_FAILED);
                    }
                }
            }
            else//当前目录不存在
            {
                strcpy(respdu->caData,DIR_NO_EXIST);
            }
            write((char*)respdu,pdu->uiPDULen);

            if(respdu!=nullptr){
                free(pdu);
                pdu=nullptr;
            }



            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:{
            char *pCurPath =new char[pdu->uiMsgLen];
            memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);
            QDir dir(pCurPath);
            QFileInfoList fileInfoList = dir.entryInfoList();
            int iFileCount=fileInfoList.size();//文件个数
            FileInfo *pFileInfo=nullptr;
            PDU *respdu = mkPDU(sizeof(FileInfo)*iFileCount);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
            QString strFileName;//用来保存临时文件名
            int index=0;
            qDebug()<<iFileCount;
            for (const QFileInfo &fileInfo : fileInfoList) {
                // 处理每个文件或目录

                strFileName = fileInfo.fileName();
                pFileInfo = (FileInfo*)(respdu->caMsg)+index;
                memcpy(pFileInfo->name,strFileName.toStdString().c_str(),strFileName.size());
                qDebug()<<pFileInfo->name;


                if (fileInfo.isFile()) {
                    pFileInfo->iFileType = 1;//0表示常规文件
                    //qDebug() << "常规文件：: " << fileInfo.fileName()<<" size:"<<fileInfo.size();

                } else if (fileInfo.isDir()) {

                    pFileInfo->iFileType = 0;//1表示文件夹
                    //qDebug() << "文件夹：: " << fileInfo.fileName()<<" size:"<<fileInfo.size();
                }
                index++;
            }
            qDebug()<<"----------------------------------------------";
            FileInfo *p=nullptr;
            for(int i=0;i<iFileCount;i++)
            {
                p = (FileInfo*)(respdu->caMsg)+i;

                qDebug()<<p->name<<" "<<p->iFileType;
                pFileInfo++;
            }
            qDebug()<<"----------------------------------------------";
            write((char*)respdu,respdu->uiPDULen);

            if(respdu!=nullptr){
                free(respdu);
                respdu=nullptr;
            }


            delete [] pCurPath;
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_REQUEST:{
            char *strPath =new char[pdu->uiMsgLen+1];
            memcpy(strPath,pdu->caMsg,pdu->uiMsgLen);
            strPath[pdu->uiMsgLen] = '\0';
            qDebug()<<"delete dir :"<<strPath;
            QFileInfo fileInfo(strPath);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            //判断是不是文件夹
            if(fileInfo.isDir()){
                QDir dir;
                dir.setPath(strPath);
                if (dir.exists()) {
                    // 目录存在，尝试删除
                    bool ret = dir.removeRecursively();
                    if (ret) {
                        strcpy(respdu->caData, DEL_DIR_SUCCESS);
                        qDebug() << "Directory deleted successfully.";
                    } else {
                        strcpy(respdu->caData, DEL_DIR_FAILED);
                        qDebug() << "Failed to delete directory.";
                    }
                }
                //文件不存在
                else{
                    qDebug() << "Directory does not exist.";
                    strcpy(respdu->caData, DEL_DIR_FAILED_NOT_EXIST);
                }
            }
            else{
                strcpy(respdu->caData, DEL_DIR_FAILED_NOT_DIR);
                qDebug() << "Failed is not dir.";

            }
            write((char*)respdu,respdu->uiPDULen);
            delete[] strPath;
            if(respdu!=nullptr){
                free(respdu);
                respdu=nullptr;
            }

            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:{
            char *dataPtr = (char *)pdu->caMsg;


            // 获取新文件名
            QString strCurPath = QString(dataPtr);
            dataPtr+=strCurPath.size()+1;

            QString strOldName = QString(dataPtr);
            dataPtr+=strOldName.size()+1;

            QString strNewName = QString(dataPtr);

            QString strOldPath = strCurPath +"/"+strOldName;
            QString strNewPath = strCurPath +"/"+strNewName;

            PDU *respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_RESPOND;

            if (!QFile::exists(strOldPath)) {
                qDebug() << RENAME_FAILED_NOT_EXIST;
                // 发送响应消息给客户端，指示源文件或目录不存在
                strcpy(respdu->caData,RENAME_FAILED_NOT_EXIST);
            } else if (QFile::rename(strOldPath, strNewPath)) {
                qDebug() << RENAME_SUCCESS;
                strcpy(respdu->caData,RENAME_SUCCESS);
                // 发送响应消息给客户端，指示重命名成功
            } else {

                strcpy(respdu->caData,RENAME_FAILED);
                qDebug() << RENAME_FAILED;
                // 发送响应消息给客户端，指示重命名失败
            }



            write((char*)respdu,respdu->uiPDULen);
            if(respdu!=nullptr){
                free(respdu);
                respdu=nullptr;
            }

            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:{
            char caEnterName[64]={'\0'};
            strncpy(caEnterName,pdu->caData,64);
            char * pPath = (char*)pdu->caMsg;
            qDebug()<<caEnterName<<"  "<<pPath;
            QString strPath=QString(pPath) + '/' + QString(caEnterName);
            PDU *respdu=nullptr;
            //更改所在目录


            QFileInfo fileInfo(strPath);
            //判断是否是目录
            if(fileInfo.isDir()){
                QDir dir(strPath);
                QFileInfoList fileInfoList = dir.entryInfoList();
                int iFileCount=fileInfoList.size();//文件个数
                qDebug()<<iFileCount;
                FileInfo *pFileInfo=nullptr;
                respdu = mkPDU(sizeof(FileInfo)*iFileCount);
                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
                QString strFileName;//用来保存临时文件名
                int index=0;

                for (const QFileInfo &fileInfo : fileInfoList) {
                    // 处理每个文件或目录

                    strFileName = fileInfo.fileName();
                    pFileInfo = (FileInfo*)(respdu->caMsg)+index;
                    memcpy(pFileInfo->name,strFileName.toStdString().c_str(),strFileName.size());
                    qDebug()<<pFileInfo->name;


                    if (fileInfo.isFile()) {
                        pFileInfo->iFileType = 1;//0表示常规文件
                        qDebug() << "常规文件：: " << fileInfo.fileName()<<" size:"<<fileInfo.size();

                    } else if (fileInfo.isDir()) {

                        pFileInfo->iFileType = 0;//1表示文件夹
                        qDebug() << "文件夹：: " << fileInfo.fileName()<<" size:"<<fileInfo.size();
                    }
                    index++;
                }

                write((char*)respdu,respdu->uiPDULen);



            }
            else if(fileInfo.isFile()){
                qDebug()<<caEnterName<<"that dir not exist";
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                strcpy(pdu->caData,"that dir not exist");
                write((char*)respdu,respdu->uiPDULen);
            }



            if(respdu!=nullptr){
                free(respdu);
                respdu=nullptr;
            }


            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:{
            char caFileName[32]={'\0'};
            qint64 fileSize = 0;
            sscanf(pdu->caData,"%s %lld",caFileName,&fileSize);
            char *pPath =(char*)pdu->caMsg;
            QString strPath = QString(pPath) + '/' +QString(caFileName);
            qDebug()<<strPath;
            QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());





            m_file.setFileName(strPath);
            m_iTotal = fileSize;
            m_iReceved = 0;
            FileReceiverTask *task = new FileReceiverTask(socket,strPath,fileSize);
            //以只写方式打开文件，若文件不存在，则自动创建文件
//            if(m_file.open(QIODevice::WriteOnly)){
//                //m_bUploadt = true;
//                m_iTotal = fileSize;
//                m_iReceved = 0;
//                emit receiveFile();
//            }
            QThreadPool::globalInstance()->start(task);

            break;
        }
        case ENUM_MSG_TYPE_DEL_FILE_REQUEST:{
            char *strPath =new char[pdu->uiMsgLen+1];
            memcpy(strPath,pdu->caMsg,pdu->uiMsgLen);
            strPath[pdu->uiMsgLen] = '\0';
            qDebug()<<"delete file :"<<strPath;
            QFileInfo fileInfo(strPath);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
            //判断是不是文件
            if(fileInfo.isFile()){
                QFile file(strPath);

                if (file.exists()) {
                    // 文件存在，尝试删除
                    bool ret = file.remove(strPath);
                    if (ret) {
                        strcpy(respdu->caData, DEL_FILE_SUCCESS);
                        qDebug() << "file deleted successfully.";
                    } else {
                        strcpy(respdu->caData, DEL_FILE_FAILED);
                        qDebug() << "Failed to delete file.";
                    }
                }
                //文件不存在
                else{
                    qDebug() << "Directory does not exist.";
                    strcpy(respdu->caData, DEL_DIR_FAILED_NOT_EXIST);
                }
            }
            else{
                strcpy(respdu->caData, DEL_DIR_FAILED_NOT_DIR);
                qDebug() << "Failed is not file.";

            }
            write((char*)respdu,respdu->uiPDULen);
            delete[] strPath;
            if(respdu!=nullptr){
                free(respdu);
                respdu=nullptr;
            }

            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:{
            char caFileName[64]={'\0'};
            strcpy(caFileName,pdu->caData);
            char *pPath =(char*)pdu->caMsg;
            QString strPath = QString(pPath) + '/' +QString(caFileName);
            qDebug()<<strPath;
            QFileInfo fileInfo(strPath);
            qint64 fileSize = fileInfo.size();
            PDU *respdu = mkPDU(0);
            sprintf(respdu->caData,"%s %lld",caFileName,fileSize);
            respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            write((char*)respdu,respdu->uiPDULen);

            m_file.setFileName(strPath);
            m_file.open(QIODevice::ReadOnly);
            m_pTimer->start(1000);


            if(respdu!=nullptr){
                free(respdu);
                respdu = nullptr;
            }

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
#if 0
            qDebug()<<"pdu.sizeof"<<pdu->uiPDULen;

            int num =0;
//            char caSendName[32]={'\0'};
//            sscanf(pdu->caData,"%s%d",caSendName,&num);
                        memcpy(&num,pdu->caData,sizeof(int));//接受人数
                        char * caSendName=pdu->caData+sizeof(int);//接受发送者姓名

            int size = 32*num;

            PDU *respdu = mkPDU(pdu->uiMsgLen-size);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;

            memcpy((char*)respdu->caMsg,(char*)(pdu->caMsg)+size,pdu->uiMsgLen-size);


            for (int var = 0; var < num; ++var) {
                char caRecvName[32]={0};
                strncpy(caRecvName,(char*)(pdu->caMsg)+var*32,32);
                qDebug()<<"----"<<var<<"----"<<caRecvName;;
                MyTcpServer::getInstance().resend(caRecvName,respdu);
            }


            qDebug()<<"----------分隔符----------";
            for (int i = 0; i < num; i++) {
                char nameBuffer[33] = {'\0'}; // 多一个字节用于确保字符串结束符
                memcpy(nameBuffer, (char*)(pdu->caMsg + i * 32), 32);
                qDebug() << "Item " << i << ": " << nameBuffer;
            }


            // 打印路径
            char pathBuffer[128] = {'\0'}; // 假设路径长度不超过32个字符
            memcpy(pathBuffer, (char*)(pdu->caMsg) + num * 32, pdu->uiMsgLen-size); // 可能需要根据实际路径长度调整
            qDebug() << "Path: " << pathBuffer;




            qDebug()<<"----------分隔符----------";

            if(respdu!=nullptr){
                free(respdu);
                respdu = nullptr;
            }
            respdu =mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->caData,"share ok");



            if(respdu!=nullptr){
                free(respdu);
                respdu = nullptr;
            }
#endif

            char  caRecvName[32]={0};
            char  strName[32]={0};
            char  *curpath = new char[uiMsgLen+1];
            curpath[uiMsgLen]='\0';



            strncpy(strName,pdu->caData,32);
            strncpy(caRecvName, pdu->caData+32, 32);
            memcpy(curpath,pdu->caMsg,pdu->uiMsgLen);

            //判断文件是否存在

            if(!QFile::exists(curpath)){
                qDebug()<<"文件不存在";
                PDU *respdu=mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
                strcpy(respdu->caData,"文件不存在");

                write((char*)respdu,respdu->uiPDULen);

                if(respdu!=nullptr){
                    free(respdu);
                    respdu = nullptr;
                }

                return;



            }

//            qDebug()<<"分享者："<<QString(strName);
//            qDebug()<<"接受者："<<QString(caRecvName);
//            qDebug()<<"分享路径："<<QString(curpath);
            pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
            MyTcpServer::getInstance().resend(caRecvName,pdu);
            delete []curpath;
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
            //获取接收者名字

            //转发路径
            char  caRecvName[32]={0};

            char  *crupath= new char[pdu->uiMsgLen+1];
            crupath[pdu->uiMsgLen] = '\0';

            strncpy(caRecvName, pdu->caData+32, 32);
            memcpy(crupath,pdu->caMsg,pdu->uiMsgLen);
            qDebug()<<"接受者："<<QString(caRecvName);
            qDebug()<<"分享路径："<<QString(crupath);
            qDebug()<<"进行路径操作";

            int lastIndex = QString(crupath).lastIndexOf('/');
            QString file = QString(crupath).mid(lastIndex + 1);

            //拷贝路径
            QString onFile ="./"+(QString)caRecvName+'/'+file;
            qDebug()<<"源路径"<<QString(crupath);
            qDebug()<<"目标路径"<<onFile;

            PDU *respdu=mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            //判断源文件和目标文件是否已经存在
            if (!QFile::exists(QString(crupath))) {
                qDebug() << "源文件不存在: ";
                strcpy(respdu->caData,"源文件不存在");

            }
            else if (QFile::exists(onFile)) {
                qDebug() << "目标文件已存在，取消拷贝: " ;
                strcpy(respdu->caData,"目标文件已存在");

            }
            else if (QFile::copy(QString(crupath), onFile))
            {
                strcpy(respdu->caData,"接收文件成功");
                qDebug() << "文件拷贝成功。";
            }
            else
            {
                strcpy(respdu->caData,"接收文件失败");
                qDebug() << "文件拷贝失败。";
            }
            write((char*)respdu,respdu->uiPDULen);
            if(respdu!=nullptr){
                free(respdu);
                respdu = nullptr;
            }



            delete[] crupath;
            break;
        }
        case ENUM_MSG_TYPE_LOGOUT_REQUEST:{
            char name[32]={0};
            char pwd[32]={0};
            strncpy(name,pdu->caData,32);
            strncpy(pwd,pdu->caData+32,32);
            qDebug()<<name<<" "<<pwd;
            bool ret=OpeDB::getInstance().handleLogout(name,pwd);
            PDU*respdu=mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGOUT_RESPOND;
            if(ret){
                strcpy(respdu->caData,"注销成功");
            }else{
                strcpy(respdu->caData,"注销失败，请确认账号和密码");
            }
            write((char*)respdu,respdu->uiPDULen);
            if(respdu!=nullptr){
                free(respdu);
                respdu = nullptr;
            }

            break;
        }
        case ENUM_MSG_TYPE_SIGNIN_REQUEST:{


            //QString signInName = pdu->caData;
            qDebug()<<pdu->caData<<" 签到";
            int ret = OpeDB::getInstance().handleSigIn(pdu->caData);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SIGNIN_RESPOND;
            if(ret==-1)//失败
            {
                strcpy(respdu->caData, "签到失败！");
            }
            else if(ret==0){//今天已经签到过了
                strcpy(respdu->caData, "请勿重复签到！");


            }
            else if(ret==1){//签到成功
                strcpy(respdu->caData, "签到成功！");
            }

            write((char*)respdu,respdu->uiPDULen);
            break;

        }
        case ENUM_MSG_TYPE_VIEW_CHECK_INS_REQUEST:{

            qDebug()<<"调用数据库查看签到情况";
            QStringList ret= OpeDB::getInstance().handleView_Check_ins(pdu->caData);
            for(auto a:ret){
                qDebug()<<a<<" ";
            }
            uint uiMsgLen=ret.size()*64;
            PDU *respdu=mkPDU(uiMsgLen);
            respdu->uiMsgType=ENUM_MSG_TYPE_VIEW_CHECK_INS_RESPOND;
            for(int i=0;i<ret.size();i++){
                memcpy((char*)(respdu->caMsg)+i*64
                       ,ret.at(i).toStdString().c_str()
                       ,ret.at(i).size());
            }


            write((char*)respdu,respdu->uiPDULen);

            break;


            break;

        }




        default:
            break;

        }
        if(pdu!=nullptr){
            free(pdu);
            pdu=NULL;
        }

    }
#if 0
    else
    {
        PDU *respdu=nullptr;


        QByteArray buff = readAll();
        m_file.write(buff);
        m_iReceved +=buff.size();
        if(m_iTotal==m_iReceved){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            m_file.close();
            this->m_iTotal=0;
            this->m_iReceved=0;
            m_bUploadt = false;


            strcpy(respdu->caData,UPLOAD_FILE_OK);
            write((char*)respdu,respdu->uiPDULen);
        }
        else if(m_iTotal < m_iReceved){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strcpy(respdu->caData,UPLOAD_FILE_FAILURED);
            write((char*)respdu,respdu->uiPDULen);
            m_file.close();
            this->m_iTotal=0;
            this->m_iReceved=0;
            m_bUploadt = false;
        }


        if(respdu!=nullptr){
            free(respdu);
            respdu=nullptr;
        }

    }
#endif


}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(strName().toStdString().c_str());
    //发送下线信号
    emit offline(this);
}

void MyTcpSocket::sendFileToClient()
{
    m_pTimer->stop();
    char *pData = new char[4096];
    while(true){

        int ret = m_file.read(pData,4096);
        if(ret>0  && ret<=4096){
            write(pData,ret);
            qDebug()<<"send"<<ret;
        }else if(0==ret){
            m_file.close();
            break;
        }else if(ret<0){

            m_file.close();
            break;
            qDebug()<<"发送文件课程给客户端失败";
        }

    }
    delete[]pData;
}

QString MyTcpSocket::strName() const
{
    return m_strName;
}

bool MyTcpSocket::starkReceiveFile()
{
    return true;
}

void MyTcpSocket::stark()
{
    while (this->state() == QTcpSocket::ConnectedState){

        PDU *respdu=nullptr;


        QByteArray buff = readAll();
        m_file.write(buff);
        m_iReceved +=buff.size();
        if(m_iTotal==m_iReceved){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            m_file.close();
            this->m_iTotal=0;
            this->m_iReceved=0;
            m_bUploadt = false;


            strcpy(respdu->caData,UPLOAD_FILE_OK);
            write((char*)respdu,respdu->uiPDULen);
        }
        else if(m_iTotal < m_iReceved){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strcpy(respdu->caData,UPLOAD_FILE_FAILURED);
            write((char*)respdu,respdu->uiPDULen);
            m_file.close();
            this->m_iTotal=0;
            this->m_iReceved=0;
            m_bUploadt = false;
        }


        if(respdu!=nullptr){
            free(respdu);
            respdu=nullptr;
        }
    }
}
