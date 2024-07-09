#ifndef EPOLL_H_
#define EPOLL_H_
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include "Channel.h"
#include <cstring>

class Channel;

// ��װEpoll�����ݽṹ���ƹ�epoll�������ɾ�� ��������¼�����
class Epoll
{
private:
    static const int MaxEvents = 100; // Ĭ�ϵķ��ش�С
    int epollfd_ = -1;                // ��ʼ��epoll������ڹ��캯���д���
    epoll_event events_[MaxEvents];   // ���epoll_wait�����¼�������
public:
    Epoll();
    ~Epoll();

    void addfd(int fd, uint32_t op);                 // ����Ҫ���ӵ�fd��ӵ�epoll��������������
    // std::vector<epoll_event> loop(int timeout = -1); // ����epoll_wait()���ȴ��¼��ķ������ѷ������¼���vector�������ء�

    void updatechannel(Channel *ch);                // ��channel���/���µ�������ϣ�channel����fd��Ҳ����Ҫ���ӵ��¼���
    std::vector<Channel *> loop(int timeout = -1);  // ����epoll_wait()���ȴ��¼��ķ������ѷ������¼���vector��������
};

# endif