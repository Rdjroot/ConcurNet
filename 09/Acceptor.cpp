#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port):loop_(loop)
{
    // �����������ļ���socket
    servsock_ = new Socket(createnonblocking());

    // ����һЩ������ܵ�����
    servsock_->setkeepalive(true);
    servsock_->setreuseaddr(true);
    servsock_->setreuseport(true);
    servsock_->settcpnodelay(true);

    // ����˵ĵ�ַ��Э��
    InetAddress servaddr(ip, port);
    servsock_->bind(servaddr); // ��ip�Ͷ˿�
    servsock_->listen();       // ��������

    acceptchannel_ = new Channel(loop_, servsock_->fd());
    acceptchannel_->setreadcallback(std::bind(&Channel::newconnection, acceptchannel_, servsock_));
    // ��ch����epoll����У��������¼�
    acceptchannel_->enablereading();
}

Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}
