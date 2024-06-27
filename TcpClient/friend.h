#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>//垂直布局
#include <QHBoxLayout>//水平布局
#include "online.h"
#include <QDateTime>


class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);

    //刷新online接受
    void showAllOnlineUsr(PDU *pdu);
    void searchUsr();
    QString m_strSearchName;

    //刷新好友列表函数接受
    void updateFriendList(PDU *pdu);

    //群发信息
    void updateGroupMsg(PDU *pdu);


    QListWidget *pFriendListWidget() const;

signals:
public slots:
    //显示online槽函数发送
    void showOnlien();

    //刷新好友列表槽函数发送
    void flushFriend();

    //删除好友槽函数
    void delFriend();

    //私聊槽函数
    void privateChat();

    //群聊槽函数
    void groupChat();

    //查看学生签到情况槽函数
    void showLoginstudent();

    //签到槽函数
    void singIn();


private:
    QTextEdit *m_pShowMsgTE;
    QListWidget *m_pFriendListWidget;
    QLineEdit *m_pInputMsgLE;//信息输入框
    QPushButton *m_pDelFriendPB;
    QPushButton *m_pFlushFriendPB;
    QPushButton *m_pShowOnlineUsrPB;
    QPushButton *m_pSearchUsrPB;
    QPushButton *m_pMsgSendPB;
    QPushButton *m_pPrivateChatPB;
    Online  *m_pOnline;

    //查看学生签到按钮
    QPushButton *m_pShowLogInPB;

    //学生签到按钮
    QPushButton *m_pSignInPB;

};

#endif // FRIEND_H
