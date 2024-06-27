#include "opedb.h"
#include <QMessageBox>
OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    init();
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;

    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\QT\\Qtproject\\yun3\\TcpServer\\cloud.db");
    if(m_db.open())
    {
        QSqlQuery query;
        QString data="select *from usrInfo";
        if(query.exec(data))
        {
            QString usrdata;
            while(query.next()){
                usrdata=QString("%1,%2,%3").arg(query.value(0).toString()).
                          arg(query.value(1).toString()).arg(query.value(2).toString());
                qDebug() << "User  Data:" << usrdata;
            }
        }
        else{
            QMessageBox::critical(NULL,"query.exec","error");
        }

    }else
    {
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

OpeDB::~OpeDB()
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    if(name==NULL||pwd==NULL){
        qDebug()<<"regist error:name or pwd is null";
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO usrInfo (name, pwd) VALUES (?, ?)");
    query.addBindValue(QString(name));
    query.addBindValue(QString(pwd));
    return query.exec();


}

//返回-1登录查询出错
//返回1用户不存在或密码不匹配
//返回2用户已经登录
//返回3正常登录成功
int OpeDB::handleLogin(const char *name, const char *pwd)
{

    if(name==NULL||pwd==NULL){
        qDebug()<<"login error:name or pwd is null";
        return -1;
    }
    //判断登录账号是否存在数据库
    QSqlQuery query;

    // 检查用户是否存在并且密码匹配
    query.prepare("SELECT * FROM usrInfo WHERE name = ? AND pwd = ? ");
    query.addBindValue(QString(name));
    query.addBindValue(QString(pwd));
    if(!query.exec()) {
        qDebug() << "login error: " << query.lastError().text();
        qDebug()<<"-1-1-1";
        return -1;
    }
    if (!query.next()) {
        qDebug()<<"用户不存在或密码不匹配";
        return 1; // 用户不存在或密码不匹配

    }

    // 检查用户是否已经登录
    int onlineStatus = query.value("online").toInt(); // 假设online是查询结果中的一个字段
    if (onlineStatus == 1) {
        qDebug()<<"用户已经登录";
        return 2; // 用户已经登录
    }
    // 更新用户在线状态
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE usrInfo SET online = 1 WHERE name = ? AND online = 0");
    updateQuery.addBindValue(QString(name));
    if (!updateQuery.exec()) {
        qDebug() << "Update error:" << updateQuery.lastError().text();
        qDebug()<<"更新在线状态失败";
        return -1; // 更新在线状态失败
    }

    qDebug()<<"登录成功";
    return 3; // 登录成功



}

void OpeDB::handleOffline(const char *name)
{
    if(name==NULL){
        qDebug() << "handleOffline error: name is null";
        return ;
    }
    QSqlQuery query;
    query.prepare("update usrInfo set online =0 where name=?");
    query.addBindValue(QString(name));
    if(query.exec()) {
        qDebug() << "User" << name << "has been set offline successfully.";
    } else {
        qDebug() << "handleOffline error:" << query.lastError().text();
    }
}

QStringList OpeDB::handleAllOnline()
{
    QStringList onlineUsers;
    onlineUsers.clear();

    QSqlQuery query(m_db);
    QString sql = "SELECT name FROM usrInfo WHERE online = 1";

    if (query.exec(sql)) {
        while (query.next()) {
            QString userName = query.value(0).toString();
            onlineUsers.append(userName);
        }
    } else {
        qDebug() << "Query failed:" << query.lastError().text();
    }

    return onlineUsers;
}

int OpeDB::handleSearchUsr(const char *name)
{
    if (name == NULL) {
        return -1;
    }

    QSqlQuery query;
    query.prepare("SELECT online FROM usrInfo WHERE name = ?");
    query.addBindValue(QString(name));

    if (!query.exec()) {
        qDebug() << "Search error:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        int onlineStatus = query.value(0).toInt();
        if (onlineStatus == 1) {
            // 用户存在并且在线
            return 1;
        } else {
            // 用户存在但离线
            return 2;
        }
    } else {
        // 用户不存在
        return 0;
    }
}

int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    //判断数据有效性
    if(pername==nullptr||name==nullptr){
        return -1;
    }


    QSqlQuery query;

    // 先获取两个用户名对应的用户ID
    query.prepare("SELECT id, name,online FROM usrInfo WHERE name = :pername OR name = :name");
    query.bindValue(":pername", QString(pername));
    query.bindValue(":name", QString(name));
    if (!query.exec()) {
        qDebug() << "Error getting user ids:" << query.lastError().text();
        return -1;
    }

    int perId = -1, nameId = -1;
    int perNameOnline =-1;
    while (query.next()) {
        if (query.value("name").toString() == QString(pername)) {
            perId = query.value("id").toInt();
            perNameOnline =query.value("online").toInt();
        } else if (query.value("name").toString() == QString(name)) {
            nameId = query.value("id").toInt();
        }
    }

    if (perId == -1 || nameId == -1) {
        qDebug() << "users not found";
        return 1;//对方不存在返回1
    }

    // 检查是否已经是好友
    query.prepare("SELECT COUNT(*) FROM friend WHERE (id = :perId AND friendId = :nameId) OR (id = :nameId AND friendId = :perId)");
    query.bindValue(":perId", perId);
    query.bindValue(":nameId", nameId);
    qDebug()<<perId<<"  "<< nameId;
    //    if (query.exec() && query.next()) {
    //        qDebug() << "They are already friends";
    //        return 2; // 返回2表示他们已经是好友了
    //    }
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt(); // 获取查询结果的计数值
        qDebug() << "Friend count:" << count;
        if (count > 0) {
            qDebug() << "They are already friends";
            return 2; // 如果计数大于0，表示他们已经是好友
        }
    }


    return (perNameOnline==1)? 3:4;
    //返回3，对方存在在线
    //返回4，对方存在不在线

}

bool OpeDB::handleAgreeAddFriend(const char *pername, const char *name)
{
    if (pername == nullptr || name == nullptr) {
        return false;
    }
    // 获取两个用户名对应的用户ID
    QSqlQuery query;
    qDebug()<<pername<<" -------- "<<name;
    query.prepare("SELECT id,name FROM usrInfo WHERE name = :name OR name = :pername");
    query.bindValue(":name", QString(name));
    query.bindValue(":pername", QString(pername));
    if (!query.exec()) {
        qDebug() << "Error getting user ids:" << query.lastError().text();
        return false;
    }

    int perId = -1, nameId = -1;
    while (query.next()) {
        QString currentName = query.value("name").toString();
        if (currentName == pername) {
            perId = query.value("id").toInt();
        } else if (currentName == name) {
            nameId = query.value("id").toInt();
        }
    }

    if (perId == -1 || nameId == -1) {
        qDebug() << "One or both users not found";
        return false;
    }
    qDebug()<<perId<<"  "<<nameId;
    // 插入好友关系
    query.prepare("INSERT INTO friend (id, friendId) VALUES (:perId, :nameId), (:nameId, :perId)");
    query.bindValue(":perId", perId);
    query.bindValue(":nameId", nameId);

    if (!query.exec()) {
        qDebug() << "Error adding friend:" << query.lastError().text();
        return false;
    }

    return true;
}

QStringList OpeDB::handleFlushFriend(const char *name)
{
    QStringList friendList;

    if (name == nullptr) {
        return friendList;  // 如果名字为空，直接返回空列表
    }

    QSqlQuery query;

    // 首先获取用户的ID
    query.prepare("SELECT id FROM usrInfo WHERE name = :name");
    query.bindValue(":name", QString(name));
    if (!query.exec() || !query.next()) {
        qDebug() << "Error getting user id for" << name << ":" << query.lastError().text();
        return friendList;
    }

    int userId = query.value(0).toInt();

    // 然后查询好友关系表，获取所有好友的ID
    query.prepare("SELECT friendId FROM friend WHERE id = :userId");
    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qDebug() << "Error getting friends for" << name << ":" << query.lastError().text();
        return friendList;
    }

    // 对于每一个好友ID，查询对应的用户名
    while (query.next()) {
        int friendId = query.value(0).toInt();

        QSqlQuery friendQuery;
        //friendQuery.prepare("SELECT name FROM usrInfo WHERE id = :friendId and online=:online");
        friendQuery.prepare("SELECT name FROM usrInfo WHERE id = :friendId ");
        friendQuery.bindValue(":friendId", friendId);
        //friendQuery.bindValue(":online", 1);
        if (friendQuery.exec() && friendQuery.next()) {
            QString friendName = friendQuery.value(0).toString();
            friendList.append(friendName);
        }
    }
    for(QString a:friendList){
        qDebug()<<a;
    }
    return friendList;
}

bool OpeDB::handleLogout(const char *name, const char *pwd)
{
    if(name==nullptr||pwd==nullptr){
        return false;
    }
    QSqlQuery query;
    int logoutId=-1;
    query.prepare("select id from usrInfo where name =:name and pwd =:pwd");
    query.bindValue(":name",name);
    query.bindValue(":pwd",pwd);
    if(query.exec()&&query.next()){
        logoutId=query.value("id").toInt();
        qDebug()<<"logoutId:"<<logoutId;
        if(logoutId==-1){

            return false;
        }
        //开始删除
        query.prepare("delete from usrInfo where id=:id");
        query.bindValue(":id",logoutId);
        if(!query.exec()){
            return false;
        }
        return true;
    }
    qDebug()<<"真的假的/";
    return false;

}

bool OpeDB::handleDelFriend(const char *pername, const char *name)
{
    if(pername==nullptr||name==nullptr){
        return false;
    }
    // 获取两个用户名对应的用户ID
    QSqlQuery query;
    qDebug()<<pername<<" -------- "<<name;
    query.prepare("SELECT id,name FROM usrInfo WHERE name = :name OR name = :pername");
    query.bindValue(":name", QString(name));
    query.bindValue(":pername", QString(pername));
    if (!query.exec()) {
        qDebug() << "Error getting user ids:" << query.lastError().text();
        return false;
    }

    int perId = -1, nameId = -1;
    while (query.next()) {
        QString currentName = query.value("name").toString();
        if (currentName == pername) {
            perId = query.value("id").toInt();
        } else if (currentName == name) {
            nameId = query.value("id").toInt();
        }
    }

    if (perId == -1 || nameId == -1) {
        qDebug() << "One or both users not found";
        return false;
    }

    // 删除好友关系
    query.prepare("DELETE FROM friend WHERE (id = :perId AND friendId = :nameId) OR (id = :nameId AND friendId = :perId)");
    query.bindValue(":perId", perId);
    query.bindValue(":nameId", nameId);
    if (!query.exec()) {
        qDebug() << "Error deleting friend:" << query.lastError().text();
        return false;
    }

    return true;
}

//数据库签到操作
int OpeDB::handleSigIn(const char *name)
{
    if(name==nullptr){
        return false;
    }
    qDebug()<<"数据库签到操作";

    // 获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();
    QString currentDateTimeStr = currentTime.toString("yyyy-MM-dd HH:mm:ss");


    // 查询当天是否已经签到过
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM signIn WHERE name = :name AND signInTime >= :startTime AND signInTime <= :endTime");
    checkQuery.bindValue(":name", QString(name));
    checkQuery.bindValue(":startTime", currentTime.toString("yyyy-MM-dd 00:00:00"));
    checkQuery.bindValue(":endTime", currentTime.toString("yyyy-MM-dd 23:59:59"));

    if (!checkQuery.exec() || !checkQuery.first()) {
        qDebug() << "查询签到记录失败:" << checkQuery.lastError();
            return false;
    }

    int signInCount = checkQuery.value(0).toInt();
    if (signInCount > 0) {
        qDebug() << "今天已经签到过";
        return 0;
    }
    // 插入签到记录
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO signIn (name, signInTime) VALUES (:name, :signInTime)");
    insertQuery.bindValue(":name", QString(name));
    insertQuery.bindValue(":signInTime", currentDateTimeStr);

    if (!insertQuery.exec()) {
        qDebug() << "插入签到记录失败:" << insertQuery.lastError();
            return -1;
    }

    qDebug()<< name << "签到成功";
    return 1;





}

QStringList OpeDB::handleView_Check_ins(const char *name)
{
    QStringList viewCheckIns;
    if(name==nullptr){
            return viewCheckIns;
    }

    // 获取用户的ID
    QSqlQuery query;
    query.prepare("SELECT id FROM usrInfo WHERE name = :name");
    query.bindValue(":name", QString(name));
    if (!query.exec() || !query.next()) {
            qDebug() << "Error getting user id for" << name << ":" << query.lastError().text();
            return viewCheckIns;
    }

    int userId = query.value(0).toInt();

    // 然后查询好友关系表，获取所有好友的ID
    query.prepare("SELECT friendId FROM friend WHERE id = :userId");
    query.bindValue(":userId", userId);
    if (!query.exec()) {
            qDebug() << "Error getting friends for" << name << ":" << query.lastError().text();
            return viewCheckIns;
    }

    // 获取当前日期的开始时间和结束时间
    QDateTime currentTime = QDateTime::currentDateTime();

    // 对于每一个好友ID，查询对应的用户名和签到情况
    while (query.next()) {
            int friendId = query.value(0).toInt();

            QSqlQuery friendQuery;
            friendQuery.prepare("SELECT name FROM usrInfo WHERE id = :friendId");
            friendQuery.bindValue(":friendId", friendId);
            if (!friendQuery.exec() || !friendQuery.next()) {
            qDebug() << "Error getting friend name for id" << friendId << ":" << friendQuery.lastError().text();
            continue;
            }

            QString friendName = friendQuery.value(0).toString();


        // 查询好友当天的签到情况
        QSqlQuery checkInQuery;
        checkInQuery.prepare("SELECT signInTime FROM signIn WHERE name = :name AND signInTime >= :startTime AND signInTime <= :endTime");
        checkInQuery.bindValue(":name", friendName);
        checkInQuery.bindValue(":startTime", currentTime.toString("yyyy-MM-dd 00:00:00"));
        checkInQuery.bindValue(":endTime", currentTime.toString("yyyy-MM-dd 23:59:59"));
        if (!checkInQuery.exec()) {
            qDebug() << "Error checking sign-in status for" << friendName << ":" << checkInQuery.lastError().text();
            continue;
        }

        if (checkInQuery.next()) {
            QString signInTime = checkInQuery.value(0).toString();
            viewCheckIns.append(friendName + "  " + signInTime);
        } else {
            viewCheckIns.append(friendName + " Not signed in");
        }
    }

    return viewCheckIns;




}
