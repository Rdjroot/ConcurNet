#ifndef ECHOSERVER_H_
#define ECHOSERVER_H_

#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <functional>
#include "Logger.h"

/**
 * 业务类
 * 实现了回显业务
 */
extern Logger &logger;

class EchoServer
{
private:
    /* data */
    TcpServer tcpserver_;
    ThreadPool threadpool_; // 工作线程池

public:
    EchoServer(const std::string &ip, uint16_t port, int subthreadnum = 3, int workthreadnum = 5);
    ~EchoServer();

    void Start(); // 启动服务
    void Stop();  // 停止服务

    void HandleNewConnection(spConnection conn);                 // 处理新的客户端连接，回调函数
    void HandleClose(spConnection conn);                         // 关闭客户端连接，回调函数
    void HandleError(spConnection conn);                         // 客户端连接出错关闭，回调函数
    void HandleMessage(spConnection conn, std::string &message); // 处理客户端的请求报文
    void HandleSendComplete(spConnection conn);                  // 数据发送完成，处理
    void HandleTimeOut(EventLoop *loop);                         // epoll_wait()超时回调函数

    void OnMessage(spConnection conn, std::string &message); // 处理客户端的请求报文，添加到线程池中
};

#endif