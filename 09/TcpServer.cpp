#include "TcpServer.h"

// ��ʼ��eventloop
// �����������ķ���˼���socket
// ���ݸ�epoll����������¼�����ʼ����
TcpServer::TcpServer(const std::string &ip, uint16_t port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
}

TcpServer::~TcpServer()
{
    delete acceptor_;
}

// ����ѭ������
void TcpServer::start()
{
    loop_.run();
}
