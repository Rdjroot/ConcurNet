/**
 * һ���򵥵�epoll����˳���
 * ��Ҫ����д�¼�
 *
 */
#include <iostream>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <cstdlib> // ʹ�� <cstdlib> ��� <stdlib.h>����Ϊ���� C++ ��׼���е�ͷ�ļ�
#include <ctime>   // ʹ�� <ctime> ��� <time.h>����Ϊ���� C++ ��׼���е�ͷ�ļ�
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h> // TCP_NODELAY��Ҫ�������ͷ�ļ���
#include <functional>
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "usage: ./tcpepoll ip port\n";
        cout << "example: ./tcpepoll 192.168.8.128  5085\n";
        return -1;
    }

    // �����������ļ���socket
    Socket servsock(createnonblocking());

    // ����һЩ������ܵ�����
    servsock.setkeepalive(true);
    servsock.setreuseaddr(true);
    servsock.setreuseport(true);
    servsock.settcpnodelay(true);

    // ����˵ĵ�ַ��Э��
    InetAddress servaddr(argv[1], atoi(argv[2]));
    servsock.bind(servaddr);    // ��ip�Ͷ˿�  
    servsock.listen();          // ��������

    Epoll ep;    
    Channel *servchannel = new Channel(&ep, servsock.fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection,servchannel,&servsock));
   
    // ��ch����epoll����У��������¼�
    servchannel->enablereading();
    
    while(true)
    {
        // ��ͣ��ʹ��epoll_wait()�����¼�
        // Ȼ���ŵ�channels��
        std::vector<Channel *> channels = ep.loop(10000);
        for(auto &ch: channels)
        {
            // �����¼�
            ch->handleevent();
        }
    }
    return 0;
}