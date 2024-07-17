#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <iostream>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <cstdlib> // 使用 <cstdlib> 替代 <stdlib.h>，因为这是 C++ 标准库中的头文件
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h> // TCP_NODELAY需要包含这个头文件。
#include <functional>
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>
#include <vector>
#include "ThreadPool.h"
#include <memory>
#include<mutex>

class TcpServer
{
private:
    std::unique_ptr<EventLoop> mainloop_;                                 // 主事件循环
    std::vector<std::unique_ptr<EventLoop>> subloops_;                    // 从事件循环容器
    Acceptor acceptor_;                                                   // 一个TcpServer只能有一个acceptor对象
    int threadnum_;                                                       // 线程池大小
    ThreadPool threadpool_;                                               // 线程池S
    std::mutex mutex_;                                                    // connection的互斥锁
    std::map<int, spConnection> conns_;                                   // 一个TcpServer有多个Connection对象
    std::function<void(spConnection)> newconnectioncb_;                   // 回调BankServer::HandleNewConnection()。
    std::function<void(spConnection)> closeconnectioncb_;                 // 回调BankServer::HandleClose()。
    std::function<void(spConnection)> errorconnectioncb_;                 // 回调BankServer::HandleError()。
    std::function<void(spConnection, std::string &message)> onmessagecb_; // 回调BankServer::HandleMessage()。
    std::function<void(spConnection)> sendcompletecb_;                    // 回调BankServer::HandleSendComplete()。
    std::function<void(EventLoop *)> timeoutcb_;                          // 回调BankServer::HandleTimeOut()。

public:
    TcpServer(const std::string &ip, uint16_t port, int threadnum);
    ~TcpServer();

    void start();
    void stop();    //停止IO线程和事件循环

    void newconnection(std::unique_ptr<Socket> clientsock);  // 处理新的客户端连接，回调函数
    void closeconnection(spConnection conn);                 // 关闭客户端连接，回调函数
    void errorconnection(spConnection conn);                 // 客户端连接出错关闭，回调函数
    void onmessage(spConnection conn, std::string &message); // 处理客户端的请求报文
    void sendcomplete(spConnection conn);
    void epolltimeout(EventLoop *loop); // epoll_wait()超时回调函数

    void setnewconnectioncb(std::function<void(spConnection)> fn);
    void setcloseconnectioncb(std::function<void(spConnection)> fn);
    void seterrorconnectioncb(std::function<void(spConnection)> fn);
    void setonmessagecb(std::function<void(spConnection, std::string &message)> fn);
    void setsendcompletecb(std::function<void(spConnection)> fn);
    void settimeoutcb(std::function<void(EventLoop *)> fn);

    void removeconn(int fd); // 删除conns_中的Connection对象，在事件循环的超时判断里回调
};

#endif