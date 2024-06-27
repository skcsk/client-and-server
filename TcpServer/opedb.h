#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QDateTime>
class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();
    void init();//连接数据库函数
    ~OpeDB();

    /**
     * @brief 数据库注册操作
     * @param name
     * @param pwd
     * @return
     */
    bool handleRegist(const char *name,const char *pwd);

    /**
     * @brief 数据库登录操作
     * @param name
     * @param pwd
     * @return
     */
    int handleLogin(const char *name,const char *pwd);

    /**
     * @brief 数据库下线函数
     * @param name
     */
    void handleOffline(const char *name);

    /**
     * @brief 数据库查询所有在线用户函数
     * @return
     */
    QStringList handleAllOnline();

    /**
     * @brief 数据库查找用户函数
     * @param name
     * @return
     */
    int handleSearchUsr(const char* name);

    /**
     * @brief 数据库添加好友函数
     * @param pername
     * @param name
     * @return
     */
    int handleAddFriend(const char* pername,const char* name);

    /**
     * @brief 确认添加好友函数
     * @param pername
     * @param name
     * @return
     */
    bool handleAgreeAddFriend(const char *pername,const char * name);

    /**
     * @brief 数据库刷新好友列表函数
     * @param name
     * @return
     */
    QStringList handleFlushFriend(const char* name);

    /**
     * @brief 数据库注销操作
     * @param name
     * @param pwd
     * @return
     */
    bool handleLogout(const char *name,const char *pwd);

    bool handleDelFriend(const char *pername,const char * name);


    /**
     * @brief 数据库签到操作
     * @param name
     * @return
     */
    int handleSigIn(const char *name);

    /**
     * @brief 数据库查询签到情况操作
     * @param name
     * @return
     */
    QStringList handleView_Check_ins(const char *name);

signals:

private:

    QSqlDatabase m_db;//连接数据库


};

#endif // OPEDB_H
