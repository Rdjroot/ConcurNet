#ifndef SOCKET_H_ 
#define SOCKET_H_ 

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"
#include <iostream>

// ����һ����������fd
int createnonblocking();

// Socket��
// ����Socket���������ú�����
class Socket
{
private:
    const int fd_; // ����socket���ļ�������

public:
    Socket(int fd); // ����һ����׼���õ�fd
    ~Socket();

    int fd() const;              // ����fd��Ա����
    void setreuseaddr(bool on);  // ����SO_REUSEADDR,�������°󶨴���`time_wait`״̬�ĵ�ַ��
    void setreuseport(bool on);  // ����SO_REUSEPORT�� �������׽��ְ󶨵�ͬһ�˿�
    void settcpnodelay(bool on); // ����TCP_NODELAY ѡ��Ƿ����Nagle�㷨
    void setkeepalive(bool on);  // ����SO_KEEPALIVE,��TCP�����Է���̽����Ϣ��

    void listen(int nn = 128);              // ��������
    void bind(const InetAddress &servaddr); // �����socket���ã���fd��ip�˿�Э�������
    int accept(InetAddress &clientaddr);    // �����µĿͻ�������
};

#endif // SOCKET_H_ 