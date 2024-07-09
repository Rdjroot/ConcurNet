#ifndef CHANNEL_H
#define CHANNEL_H
#include <sys/epoll.h>
#include <functional>
#include "InetAddress.h"
#include "Socket.h"
#include <iostream>
#include "Epoll.h"
#include <string>

class Epoll;
// ����socket�Ĺ����ԣ�����Socket�ĵײ����ԣ���˿ڡ�ip��Э��ȣ�
// �ƹܴ���socket�¼��ķ����ͱ仯
// �൱�ڽ������epoll_wait֮�������
class Channel
{
private:
    int fd_ = -1;          // Channelӵ�е�fd��Channel��fd Ϊһ��һ��ϵ
    Epoll *ep_ = nullptr;  // Channel��Ӧ�ĺ������Channel��Epoll�Ƕ��һ�Ĺ�ϵ��һ��Channelֻ��Ӧһ��Epoll
    bool inepoll_ = false; // Channel�Ƿ��Ѿ���ӵ�epoll����У�����epoll_ctl()��ʱ��ʹ��EPOLL_CTL_ADD��������EPOLL_CTL_MOD
    uint32_t events_ = 0;  // fd_��Ҫ���ӵ��¼���listenfd��clientfd��Ҫ����EPOLLIN��clientfd������Ҫ����EPOLLOUT
    uint32_t revents_ = 0; // fd_�ѷ������¼�

    std::function<void()> readcallback_; // fd_���¼��Ļص�����

public:
    Channel(Epoll *ep, int fd);
    ~Channel();

    // ��ȡ��Ա����
    int fd();
    bool inpoll();
    uint32_t events();
    uint32_t revents();

    void useet();                 // ���ñ�Ե����
    void enablereading();         // ��epoll_wait����fd_�Ķ��¼�
    void setinepoll();            // ��inepoll��ֵ����Ϊtrue
    void setrevents(uint32_t ev); // ����revents_��Ա��ֵΪev

    void handleevent();                             // �¼���������epoll_wait()���ص�ʱ��ִ����
    void newconnection(Socket *servsock);    // �����¿ͻ�����������
    void onmessage();                               // ����Զ˷��͹�������Ϣ
    void setreadcallback(std::function<void()> fn); // ����fd_���¼��Ļص�����
};

# endif