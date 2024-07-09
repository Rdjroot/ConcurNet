#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include<functional>
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"

class Acceptor
{
private:
    /* data */
    EventLoop *loop_;        // Acceptor��Ӧ���¼�ѭ�����ӹ��캯�����룬ʹ�õ���ӵ��
    Socket *servsock_;       // ��������ڼ�����socket
    Channel *acceptchannel_;    // ��Ӧ��Channel���ڹ��캯���д���
public:
    Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port);
    ~Acceptor();
};



#endif