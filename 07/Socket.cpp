#include "Socket.h"

using std::cerr;
using std::endl;

// �����������ļ���socket
int createnonblocking()
{
    // ������������ڼ�����listenfd��
    // ���`SOCK_NONBLOCK`���ԣ���listenfd��Ϊ�������ġ�
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        cerr << "socket() failed" << endl;
    }
    return listenfd;
}

Socket::Socket(int fd) : fd_(fd){};

Socket::~Socket()
{
    close(fd_);
}

// ����fd��Ա
int Socket::fd() const
{
    return fd_;
}

// ����Nagle�㷨����������С���ݰ�
// ��ʵʱ��Ҫ��ߵ�Ӧ����ʹ��
// �ڱ�ģ���б������
void Socket::settcpnodelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)); // TCP_NODELAY����ͷ�ļ� <netinet/tcp.h>
}

// �������°󶨴���`time_wait`״̬�ĵ�ַ��
// Ҳ�������ӷ��������ӶϿ���2MSL
// �ڱ�ģ���б������
void Socket::setreuseaddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

// �������׽��ְ󶨵�ͬһ���˿�
// ������̿��Լ���ͬһ���˿�
// ���ã������岻��
void Socket::setreuseport(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

// ���� TCP ���ֻ���Ӽ�⡣TCP�������Է���̽����Ϣ����ȷ������ʱ���
// �������ã��������Լ�������
void Socket::setkeepalive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

// ���ṹ��ͼ���fd��
void Socket::bind(const InetAddress &servaddr)
{
    if (::bind(fd_, servaddr.addr(), sizeof(servaddr)) < 0)
    {
        cerr << "bind() failed" << endl;
        close(fd_);
        exit(-1);
    }
}

void Socket::listen(int nn)
{
    if (::listen(fd_, nn) != 0) // �ڸ߲���������������У��ڶ�������Ҫ��һЩ��
    {
        cerr << "listen() failed" << endl;
        close(fd_);
        exit(-1);
    }
}

int Socket::accept(InetAddress &clientaddr)
{
    sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);

    // accept4()������Linux 2.6.28֮�������ĺ������������accept()������
    // ���SOCK_NONBLOCK����clientfd��Ϊ�������ġ�
    int clientfd = accept4(fd_, (struct sockaddr *)&peeraddr, &len, SOCK_NONBLOCK);

    clientaddr.setaddr(peeraddr); // �ͻ���

    return clientfd;
}