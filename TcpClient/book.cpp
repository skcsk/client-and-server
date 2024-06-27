#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QFileDialog>
#include "share.h"

void Book::updateFileList(PDU *pdu)
{

    if(pdu==nullptr){
        return;
    }
    FileInfo *pFileInfo =nullptr;
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    m_pBookListW->clear();
    qDebug()<<iCount;//文件数量
    qDebug()<<"--------------------------------------------------------";

    for(int i=2;i<iCount;i++){

        pFileInfo = (FileInfo*)(pdu->caMsg)+i;
        QListWidgetItem *pItem = new    QListWidgetItem;
       qDebug()<<(QString)pFileInfo->name<<pFileInfo->iFileType;
        if(pFileInfo->iFileType == 0 )
        {
            pItem->setIcon(QIcon(QPixmap(":/map/dir.jpg")));
        }
        else if(pFileInfo->iFileType == 1 ) {
            pItem->setIcon(QIcon(QPixmap(":/map/reg.jpg")));
        }
        pItem->setText((QString)pFileInfo->name);
        this->m_pBookListW->addItem(pItem);

    }
    qDebug()<<"--------------------------------------------------------";
}

void Book::clearEnterDir()
{
    this->m_strEnterDir.clear();
}

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    Share::getInstance().setWindowTitle("分享文件");
    m_pTimer = new QTimer;
    m_downLoad = false;
    m_strEnterDir.clear();
    m_pBookListW = new QListWidget;
    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenamePB = new QPushButton("重命名文件夹");
    m_pFlushPB = new QPushButton("刷新文件");

    QVBoxLayout *pDirVBL =new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushPB);

    m_pUploadPB = new QPushButton("上传文件");
    m_pDownLoadPB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("发布文件");

    QVBoxLayout *pFileVBL =new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);

    QHBoxLayout *pMain =new  QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);
    setLayout(pMain);

    connect(m_pCreateDirPB,&QPushButton::clicked,
            this,&Book::createDir);
    connect(m_pFlushPB,&QPushButton::clicked,
            this,&Book::flushFile);
    connect(m_pDelDirPB,&QPushButton::clicked,
            this,&Book::DelDir);
    connect(m_pRenamePB,&QPushButton::clicked,
            this,&Book::remaneFile);
    connect(m_pBookListW,&QListWidget::doubleClicked,
            this,&Book::enterDir);
    connect(m_pReturnPB,&QPushButton::clicked,
            this,&Book::returnPer);
    connect(m_pUploadPB,&QPushButton::clicked,
            this,&Book::upLoadFile);
    connect(m_pTimer,&QTimer::timeout,
            this,&Book::upLoadFileData);
    connect(m_pDelFilePB,&QPushButton::clicked,
            this,&Book::delFile);
    connect(m_pDownLoadPB,&QPushButton::clicked,
            this,&Book::downLoadFile);
    connect(m_pShareFilePB,&QPushButton::clicked,
            this,&Book::shareFile);

}

QString Book::getEnterDir()
{
    return this->m_strEnterDir;
}

void Book::setDownLoadStatus(bool status)
{
    this->m_downLoad = status;
}



void Book::createDir()
{
    bool ok;
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","新文件夹名字",
                                              QLineEdit::Normal, QString(), &ok);
    if(!ok){
        return;
    }


    if(strNewDir.isEmpty()){
        QMessageBox::warning(this,"新建文件夹","新文件夹名字不能为空");
        return;
    }
    if(strNewDir.size()>32){
        QMessageBox::warning(this,"新建文件夹","新文件夹名字过长");
        return;
    }
    QString strName=TcpClienT::getInstance().loginName();
    QString strCurPath=TcpClienT::getInstance().curPath();
    PDU *pdu=mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
    strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

    if(pdu!=nullptr){
        free(pdu);
        pdu=nullptr;
    }
}

void Book::flushFile()
{

    QString strCurPath =TcpClienT::getInstance().curPath();//获取当前所在目录
    PDU *pdu=mkPDU(strCurPath.size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

    if(pdu!=nullptr){
        free(pdu);
        pdu=nullptr;
    }

}

void Book::DelDir()
{
    QString strCurPath =TcpClienT::getInstance().curPath();//获取当前所在目录
    QListWidgetItem *pItem = nullptr;
    pItem  = this->m_pBookListW->currentItem();
    if(pItem == nullptr){
        QMessageBox::information(this,"删除文件夹","请选择要删除文件夹");
        return;
    }
    // 获取选中的文件夹名
    QString strDirName = pItem->text();

    // 弹出确认对话框
    int ret = QMessageBox::question(this, "删除文件夹",
                                    "您确定要删除文件夹 " + strDirName + " 吗？",
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        // 用户确认删除
        qDebug() << "用户确认删除文件夹:" << strDirName;
        // 发送删除文件加pdu
        QString delDir = strCurPath + "/" + strDirName;
        PDU * pdu = mkPDU(delDir.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;

        memcpy(pdu->caMsg, delDir.toStdString().c_str(), delDir.size() + 1);
        TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//发送数据
        qDebug()<<"dir : "<<delDir<<" delete!";
        if(pdu!=nullptr){
            free(pdu);
            pdu=nullptr;
        }
    } else {
        // 用户取消删除
        qDebug() << "用户取消删除文件夹:" << strDirName;
    }




}

void Book::remaneFile()
{

    QListWidgetItem *pItem = nullptr;
    pItem  = this->m_pBookListW->currentItem();
    if(pItem == nullptr){
        QMessageBox::information(this,"重命名文件","请选择要重命名的文件");
        return;
    }
    QString strOldName = pItem->text();

    bool ok;
    QString strNewName = QInputDialog::getText(this,"重命名文件","重命名",
                                              QLineEdit::Normal, QString(), &ok);
    if(!ok) {
        return;
    }
    if(strNewName.isEmpty()){
        QMessageBox::warning(this,"重命名文件","输入为空");
        return;
    }
    QString strCurPath =TcpClienT::getInstance().curPath();//获取当前所在目录


    uint msgLen = strCurPath.size()+strOldName.size()+strNewName.size()+3; // +1 for separator
//    qDebug()<<"当前目录"<<strCurPath;
//    qDebug()<<"重命名名字"<<strOldName;
//    qDebug()<<"新名字"<<strNewName;
    PDU *pdu = mkPDU(msgLen);
    pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
    char * dataPtr =(char *) pdu->caMsg;
    strncpy(dataPtr,strCurPath.toStdString().c_str(),strCurPath.size());
    dataPtr+= strCurPath.size();
    *dataPtr++='\0';
    strncpy(dataPtr,strOldName.toStdString().c_str(),strOldName.size());

    dataPtr+= strOldName.size();
    *dataPtr++='\0';
    strncpy(dataPtr,strNewName.toStdString().c_str(),strNewName.size());
    dataPtr+= strNewName.size();
    *dataPtr++='\0';
    qDebug()<<(char*)pdu->caMsg;
    qDebug()<<(char*)pdu->caMsg+strCurPath.size()+1;
    qDebug()<<(char*)pdu->caMsg+strCurPath.size()+2+strOldName.size();
    TcpClienT::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
    dataPtr = nullptr;
    if (pdu != nullptr) {
        free(pdu);
        pdu = nullptr;
    }






}

void Book::enterDir(const QModelIndex &index)
{

    QString strDirName = index.data().toString();
    qDebug()<<strDirName;
    QString strCurPath =TcpClienT::getInstance().curPath();//获得当前所在目录
    m_strEnterDir = strDirName;
    PDU *pdu =mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy(pdu->caData,strDirName.toStdString().c_str(),strDirName.size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());



    TcpClienT::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
    if (pdu != nullptr) {
        free(pdu);
        pdu = nullptr;
    }





}

void Book::returnPer()
{
    m_strEnterDir.clear();
    //获得当前路径
    QString strCurPath =TcpClienT::getInstance().curPath();
    QString strRootPath = "./" + TcpClienT::getInstance().loginName();
    if(strCurPath==strRootPath){
        QMessageBox::warning(this,"返回失败","已经在源目录");
        return;
    }
    else{

        int index= strCurPath.lastIndexOf('/');
        strCurPath.remove(index,strCurPath.size()-index);
        qDebug()<<"return --->"<<strCurPath;
        TcpClienT::getInstance().setCurPath(strCurPath);
        this->flushFile();
    }
}

void Book::upLoadFile()
{

    m_strUploadFilePath = QFileDialog::getOpenFileName();

    if(m_strUploadFilePath.isEmpty()){
        //QMessageBox::warning(this,"上传文件","不能为空");
        return;
    }
    int index = m_strUploadFilePath.lastIndexOf("/");
    QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
    qDebug()<<strFileName;
    QFile file(m_strUploadFilePath);

    qint64 fileSize = file.size();
    QString strCurPath =TcpClienT::getInstance().curPath();//获取当前所在路径
    PDU *pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy((char*)pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
    sprintf(pdu->caData,"%s %lld",strFileName.toStdString().c_str(),fileSize);
    TcpClienT::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
    if (pdu != nullptr) {
        free(pdu);
        pdu = nullptr;
    }
    m_pTimer->start(1000);



}

void Book::upLoadFileData()
{
    m_pTimer->stop();
    QFile file(this->m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"上传文件","打开文件失败");
        return;
    }
    char *pBuffer = new char[4096];
    qint64 ret =0;
    size_t sum=0;
    while(true){
        ret = file.read(pBuffer,4096);
        if(ret>0  &&  ret<=4096){
            TcpClienT::getInstance().getTcpSocket().write(pBuffer,ret);
            sum+=ret;
            qDebug()<<"上传文件"<<sum;
        }
        else if(ret==0){
            //表示读到末尾了，结束循环
            break;
        }
        else{
            //QMessageBox::warning(this,"上传文件","读取文件失败");

        }
    }
    file.close();
    delete[] pBuffer;
}

void Book::delFile()
{
    QString strCurPath =TcpClienT::getInstance().curPath();//获取当前所在目录
    QListWidgetItem *pItem = nullptr;
    pItem  = this->m_pBookListW->currentItem();
    if(pItem == nullptr){
        QMessageBox::information(this,"删除文件","请选择要删除文件");
        return;
    }
    // 获取选中的文件名
    QString strDirName = pItem->text();

    // 弹出确认对话框
    int ret = QMessageBox::question(this, "删除文件",
                                    "您确定要删除文件 " + strDirName + " 吗？",
                                                QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        // 用户确认删除
        qDebug() << "用户确认删除文件:" << strDirName;
        // 发送删除文件加pdu
        QString delDir = strCurPath + "/" + strDirName;
        PDU * pdu = mkPDU(delDir.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;

        memcpy(pdu->caMsg, delDir.toStdString().c_str(), delDir.size() + 1);
        TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//发送数据
        qDebug()<<"dir : "<<delDir<<" delete!";
        if(pdu!=nullptr){
            free(pdu);
            pdu=nullptr;
        }
    } else {
        // 用户取消删除
        qDebug() << "用户取消删除文件夹:" << strDirName;
    }

}

void Book::downLoadFile()
{
    //指定位置存放下载文件
    QString strSaveFilePath = QFileDialog::getSaveFileName();
    if(strSaveFilePath.isEmpty()){
        return;
    }
    else{
        m_strSaveFilePath = strSaveFilePath;
    }
    m_iRecved=0;
    m_iTotal=0;

    QListWidgetItem *pItem = nullptr;
    pItem  = this->m_pBookListW->currentItem();
    if(pItem==nullptr){
        QMessageBox::information(this,"下载文件","请选择要文件");
        return;
    }
    qDebug()<<"m_strSaveFilePath :"<<m_strSaveFilePath;
    QString strCurPath =TcpClienT::getInstance().curPath();//获取当前所在目录
    PDU *pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    QString strFileName = pItem->text();

    strncpy(pdu->caData,strFileName.toStdString().c_str(),strFileName.size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());//




    TcpClienT::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    if(pdu!=nullptr){
        free(pdu);
        pdu=nullptr;
    }

}

void Book::shareFile()
{
#if 0
    //获得当前分享文件名字
    QListWidgetItem *pItem = nullptr;
    pItem  = this->m_pBookListW->currentItem();
    //判断是否为空
    if(pItem == nullptr){
        QMessageBox::warning(this,"分享文件","请选择分享文件");
        return;
    }
     strShareFileName = pItem->text();//获得分享文件名



    //获得在线好友列表
    QListWidget *friendList = OpeWidget::getInstance().pFriend()->pFriendListWidget();
//    for(int i = 0; i < friendList->count(); ++i) {
//        QListWidgetItem* item = friendList->item(i);
//        // 在这里处理每个 item
//        qDebug()<<item->text();
//    }
    ShareFile::getInstance().updateFriend(friendList);

    if(ShareFile::getInstance().isHidden()){
        ShareFile::getInstance().show();

    }
    else{
        ShareFile::getInstance().hide();
    }
#endif
    //获得当前分享文件名字
    QListWidgetItem *pItem = nullptr;
    pItem  = this->m_pBookListW->currentItem();
    //判断是否为空
    if(pItem == nullptr){
        QMessageBox::warning(this,"分享文件","请选择分享文件");
        return;
    }
    strShareFileName = pItem->text();//获得分享文件名
    Share::getInstance().show();
}

QString Book::strSaveFilePath() const
{
    return m_strSaveFilePath;
}

bool Book::downLoad() const
{
    return m_downLoad;
}



