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

class EventLoop;
class Socket;
class Channel;
class InetAddress;

class TcpServer
{
private:
    EventLoop loop_;

public:
    TcpServer(const std::string &ip, uint16_t port);
    ~TcpServer();

    void start();
};

#endif