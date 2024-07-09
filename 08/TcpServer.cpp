#include "TcpServer.h"

// ��ʼ��eventloop
// �����������ķ���˼���socket
// ���ݸ�epoll����������¼�����ʼ����
TcpServer::TcpServer(const std::string &ip, uint16_t port)
{
    // �����������ļ���socket
    Socket *servsock = new Socket(createnonblocking());

    // ����һЩ������ܵ�����
    servsock->setkeepalive(true);
    servsock->setreuseaddr(true);
    servsock->setreuseport(true);
    servsock->settcpnodelay(true);

    // ����˵ĵ�ַ��Э��
    InetAddress servaddr(ip, port);
    servsock->bind(servaddr); // ��ip�Ͷ˿�
    servsock->listen();       // ��������

    Channel *servchannel = new Channel(&loop_, servsock->fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, servsock));
    // ��ch����epoll����У��������¼�
    servchannel->enablereading();
}

TcpServer::~TcpServer()
{
}

// ����ѭ������
void TcpServer::start()
{
    loop_.run();
}
