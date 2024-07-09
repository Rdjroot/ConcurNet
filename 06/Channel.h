#ifndef CHANNEL_H
#define CHANNEL_H
#include <sys/epoll.h>
#include <functional>
#include "InetAddress.h"
#include "Socket.h"
#include <iostream>
#include "Epoll.h"
#include <string>

class Epoll;
// 处理socket的功能性，忽视Socket的底层属性（如端口、ip、协议等）
// 掌管处理socket事件的发生和变化
// 相当于解析完成epoll_wait之后的任务
class Channel
{
private:
    int fd_ = -1;          // Channel拥有的fd，Channel与fd 为一对一关系
    Epoll *ep_ = nullptr;  // Channel对应的红黑树，Channel和Epoll是多对一的关系，一个Channel只对应一个Epoll
    bool inepoll_ = false; // Channel是否已经添加到epoll句柄中，调用epoll_ctl()的时候使用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD
    uint32_t events_ = 0;  // fd_需要监视的事件，listenfd和clientfd需要监视EPOLLIN，clientfd可能需要监视EPOLLOUT
    uint32_t revents_ = 0; // fd_已发生的事件

    std::function<void()> readcallback_; // fd_读事件的回调函数

public:
    Channel(Epoll *ep, int fd);
    ~Channel();

    // 获取成员数据
    int fd();
    bool inpoll();
    uint32_t events();
    uint32_t revents();

    void useet();                 // 采用边缘触发
    void enablereading();         // 让epoll_wait监视fd_的读事件
    void setinepoll();            // 把inepoll的值设置为true
    void setrevents(uint32_t ev); // 设置revents_成员的值为ev

    void handleevent();                             // 事件处理函数，epoll_wait()返回的时候，执行它
    void newconnection(Socket *servsock);    // 处理新客户端连接请求。
    void onmessage();                               // 处理对端发送过来的消息
    void setreadcallback(std::function<void()> fn); // 设置fd_读事件的回调函数
};

# endif