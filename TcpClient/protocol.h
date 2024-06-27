#ifndef PROTOCOL_H
#define PROTOCOL_H


#include <stdlib.h>
#include <string.h>
typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed : name existed "
#define LOGIN_ERROR_UNKNOWN "login failed: unknown error occurred"
#define LOGIN_FAILED_USER_NOT_FOUND "login failed: username not found or password incorrect"
#define LOGIN_FAILED_ALREADY_LOGGED_IN "login failed: user already logged in"
#define LOGIN_SUCCESS "login successful: welcome back"

#define SEARCH_USR_NO "no such people"
#define SEARCH_USR_ONLINE "people online"
#define SEARCH_USR_OFFLINE "people offline"

#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OFFLINE "usr offline"
#define ADD_FRIEND_NOEXIST "usr not exist"
#define DEL_FRIEND_SUCCESS "delete friend success"
#define DEL_FRIEND_FAILED  "delete friend failed"
#define DIR_NO_EXIST "cur dir not exist"
#define FILE_NAME_EXIST "file name exist"
#define CREATE_DIR_OK "create dir success"
#define CREATE_DIR_FAILED "create dir failed"

#define DEL_DIR_SUCCESS "delete dir success"
#define DEL_DIR_FAILED "delete dir failed"
#define DEL_DIR_FAILED_NOT_DIR "path is not a directory"
#define DEL_DIR_FAILED_NOT_EXIST "directory does not exist"
#define RENAME_SUCCESS "rename success"
#define RENAME_FAILED "rename failed"
#define RENAME_FAILED_NOT_EXIST "source file or directory does not exist"
#define UPLOAD_FILE_OK "upload file ok"
#define UPLOAD_FILE_FAILURED "iupload file failured"


#define DEL_FILE_SUCCESS "delete file success"
#define DEL_FILE_FAILED "delete file failed"


enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGIST_REQUEST,//注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,//注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST,//登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,//登录回复
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,//显示所有在线人数请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,//显示所有在线人数回应
    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,//搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,//搜索用户回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,//添加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,//同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,//不同意添加好友
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,//刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,//刷新好友回复

    ENUM_MSG_TYPE_DEL_FRIEND_REQUEST,//删除好友请求
    ENUM_MSG_TYPE_DEL_FRIEND_RESPOND,//删除好友回复
    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,//私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,//私聊回复
    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,//群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESOPND,//群聊回复
    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,//创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,//创建文件夹回复
    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,//刷新文件夹请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,//刷新文件夹回复
    ENUM_MSG_TYPE_DEL_DIR_REQUEST,//删除目录夹请求
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,//删除目录回复
    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,//重命名请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,//重命名回复
    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,//进入目录请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,//进入目录回复
    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,//上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,//是上传文件回复

    ENUM_MSG_TYPE_DEL_FILE_REQUEST,//删除文件请求
    ENUM_MSG_TYPE_DEL_FILE_RESPOND,//删除文件回复

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,//下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,//下载文件回复


    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,//共享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,//共享文件回复
    ENUM_MSG_TYPE_SHARE_FILE_NOTE,

    ENUM_MSG_TYPE_LOGOUT_REQUEST,//注销请求
    ENUM_MSG_TYPE_LOGOUT_RESPOND,//注销回复

    ENUM_MSG_TYPE_SIGNIN_REQUEST,//签到请求
    ENUM_MSG_TYPE_SIGNIN_RESPOND,//签到回复

    ENUM_MSG_TYPE_VIEW_CHECK_INS_REQUEST,//查看签到请求
    ENUM_MSG_TYPE_VIEW_CHECK_INS_RESPOND,//查看签到回复


    //    ENUM_MSG_TYPE_
    //    ENUM_MSG_TYPE_
    //    ENUM_MSG_TYPE_
    //    ENUM_MSG_TYPE_
    ENUM_MSG_TPPE_MAX=0x00ffffff,
};

struct FileInfo {
    char name[256];  // 文件或目录名
    int iFileType;     // 文件类型
};


struct PDU{
    uint uiPDULen;//总的协议数据单元大小
    uint uiMsgType;//消息类型
    char caData[64];
    uint uiMsgLen;//实际消息长度
    int caMsg[];//实际消息

};
PDU* mkPDU(uint uiMsgLen);


#endif // PROTOCOL_H
