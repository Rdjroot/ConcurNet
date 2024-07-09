#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <iostream>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <cstdlib> // ʹ�� <cstdlib> ��� <stdlib.h>����Ϊ���� C++ ��׼���е�ͷ�ļ�
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h> // TCP_NODELAY��Ҫ�������ͷ�ļ���
#include <functional>
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Acceptor.h"

class TcpServer
{
private:
    EventLoop loop_;
    Acceptor *acceptor_;     // һ��TcpServerֻ����һ��acceptor����
public:
    TcpServer(const std::string &ip, uint16_t port);
    ~TcpServer();

    void start();
};

#endif