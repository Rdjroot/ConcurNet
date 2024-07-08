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
#include "InetAddress.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "usage: ./tcpepoll ip port\n"
             << endl;
        cout << "example: ./tcpepoll 192.168.8.128  5085\n\n";
        return -1;
    }

    // ������������ڼ�����listenfd��
    // ���`SOCK_NONBLOCK`���ԣ���listenfd��Ϊ�������ġ�
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        cerr << "socket() failed" << endl;
        return -1;
    }

    int opt = 1;
    // �������°󶨴���`time_wait`״̬�ĵ�ַ��
    // Ҳ�������ӷ��������ӶϿ���2MSL
    // �ڱ�ģ���б������
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
    // ����Nagle�㷨����������С���ݰ�
    // ��ʵʱ��Ҫ��ߵ�Ӧ����ʹ��
    // �ڱ�ģ���б������
    setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
    // �������׽��ְ󶨵�ͬһ���˿�
    // ������̿��Լ���ͬһ���˿�
    // ���ã������岻��
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
    // ���� TCP ���ֻ���Ӽ�⡣TCP�������Է���̽����Ϣ����ȷ������ʱ���
    // �������ã��������Լ�������
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));

    InetAddress servaddr(argv[1], atoi(argv[2]));

    // ���ṹ���listenfd��
    if (bind(listenfd, servaddr.addr(), sizeof(servaddr)) < 0)
    {
        cerr << "bind() failed" << endl;
        close(listenfd);
        return -1;
    }

    if (listen(listenfd, 128) != 0) // �ڸ߲���������������У��ڶ�������Ҫ��һЩ��
    {
        cerr << "listen() failed" << endl;
        close(listenfd);
        return -1;
    }

    int epollfd = epoll_create(1); // ����epoll��������������

    // Ϊ����˵�listenfd׼�����¼���
    epoll_event ev; // �����¼������ݽṹ��
    ev.data.fd = listenfd; // ָ���¼����Զ������ݣ�������epoll_wait()���ص��¼�һ�����ء�
    ev.events = EPOLLIN;   // ��epoll����listenfd�Ķ��¼�������ˮƽ������

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev); // ����Ҫ���ӵ�listenfd�������¼�����epollfd�С�

    epoll_event evs[10]; // ���epoll_wait()�����¼������顣

    while (true) // �¼�ѭ����
    {
        // �ȴ����ӵ�fd���¼�������
        // -1 ��ʾ��Ҫ���޵ȴ�
        int infds = epoll_wait(epollfd, evs, 10, -1);

        // ����ʧ�ܡ�
        if (infds < 0)
        {
            cerr << "epoll_wait() failed" << endl;
            break;
        }

        // ��ʱ��
        if (infds == 0)
        {
            cerr << "epoll_wait() timeout." << endl;
            continue;
        }

        // ���infds>0����ʾ���¼�������fd��������
        for (int ii = 0; ii < infds; ii++) // ����epoll���ص�����evs��
        {
            // �Է��ѹرգ���Щϵͳ��ⲻ��������ʹ��EPOLLIN��recv()����0��
            // EPOLLRDHUP ��ʾ�Զ˹ر����ӻ��رա�
            if (evs[ii].events & EPOLLRDHUP)
            {
                cout << "1client(eventfd=" << evs[ii].data.fd << ") disconnected.\n";
                // �رոÿͻ��˵�fd��
                close(evs[ii].data.fd);
            }
            //  EPOLLIN ��ͨ����  EPOLLPRI��������
            else if (evs[ii].events & (EPOLLIN | EPOLLPRI)) // ���ջ������������ݿ��Զ���
            {
                // �����listenfd���¼�����ʾ���µĿͻ�����������
                if (evs[ii].data.fd == listenfd)
                {
                    sockaddr_in peeraddr;
                    socklen_t len = sizeof(peeraddr);

                    // accept4()������Linux 2.6.28֮�������ĺ������������accept()������
                    // ���SOCK_NONBLOCK����clientfd��Ϊ�������ġ�
                    int clientfd = accept4(listenfd, (struct sockaddr *)&peeraddr, &len, SOCK_NONBLOCK);

                    cout << "accept client(fd=" << clientfd << ", ip=" << inet_ntoa(peeraddr.sin_addr) << ", port=" << ntohs(peeraddr.sin_port) << ") ok." << endl;

                    // Ϊ�¿ͻ�������׼�����¼�������ӵ�epoll�С�
                    ev.data.fd = clientfd;
                    ev.events = EPOLLIN | EPOLLET; // ��Ե������
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
                }
                else // ����ǿͻ������ӵ�fd���¼���
                {
                    string buffer(1024, '\0');
                    while (true) // ����ʹ�÷�����IO��һ�ζ�ȡbuffer��С���ݣ�ֱ��ȫ�������ݶ�ȡ��ϡ�
                    {
                        buffer.assign(buffer.size(), '\0');

                        // ���׽����ж�����
                        ssize_t nread = read(evs[ii].data.fd, &buffer[0], sizeof(buffer));

                        // �ɹ��Ķ�ȡ�������ݡ�
                        if (nread > 0)
                        {
                            // �ѽ��յ��ı�������ԭ�ⲻ���ķ���ȥ��
                            cout << "recv(eventfd=" << evs[ii].data.fd << "):" << buffer << endl;
                            send(evs[ii].data.fd, &buffer[0], buffer.size(), 0);
                        }
                        else if (nread == -1 && errno == EINTR) // ��ȡ���ݵ�ʱ���ź��жϣ�������ȡ��
                        {
                            continue;
                        }
                        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // ȫ���������Ѷ�ȡ��ϡ�
                        {
                            break;
                        }
                        else if (nread == 0) // �ͻ��������ѶϿ���
                        {
                            cout << "2client(eventfd=" << evs[ii].data.fd << ") disconnected." << endl;
                            close(evs[ii].data.fd); // �رտͻ��˵�fd��
                            break;
                        }
                    }
                }
            }
            else if (evs[ii].events & EPOLLOUT) // ��������Ҫд����ʱû�д��룬�Ժ���˵��
            {
            }
            else // �����¼�������Ϊ����
            {
                cout << "2client(eventfd=" << evs[ii].data.fd << ") error." << endl;
                close(evs[ii].data.fd); // �رտͻ��˵�fd��
            }
        }
    }

    return 0;
}