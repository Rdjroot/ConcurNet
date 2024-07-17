#include "Epoll.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

Epoll::Epoll()
{
    if ((epollfd_ = epoll_create(1)) == -1) // ����epoll��������������
    {
        cerr << "epoll_create() failed(" << errno << ").\n";
        exit(-1);
    }
}

Epoll::~Epoll()
{
    close(epollfd_);
}

/**
 * ��epoll��������Ҫ���ӵ�socket
 * op��ʾ�������¼������¼���д�¼���
 */
// void Epoll::addfd(int fd, uint32_t op)
// {
//     // Ϊfd׼���¼���
//     epoll_event ev;  // �����¼������ݽṹ��
//     ev.data.fd = fd; // ָ���¼����Զ������ݣ�������epoll_wait()���ص��¼�һ�����ء�
//     ev.events = op;  // ����ˮƽ������

//     // ����Ҫ���ӵ�listenfd�������¼�����epollfd�С�
//     if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
//     {
//         cout << "epoll_ctl() failed(" << errno << ")." << endl;
//         exit(-1);
//     }
// }

// ����Chanenl������Ҫ������fd��Channel��
void Epoll::updatechannel(Channel *ch)
{
    // Ϊfd׼���¼���
    epoll_event ev;           // �����¼������ݽṹ��
    ev.data.ptr = ch;         // ָ��Channel
    ev.events = ch->events(); // ָ���¼�

    // ����Ѿ��ں������
    if (ch->inpoll())
    {   
        // ���¼����¼�ģʽ
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1)
        {
            cout << "epoll_ctl() failed(" << errno << ")." << endl;
            exit(-1);
        }
    }
    else
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1)
        {
            cout << "epoll_ctl() failed(" << errno << ")." << endl;
            exit(-1);
        }
        ch->setinepoll();           // ����Ѿ�����
    }
}

// �Ӻ������ɾ��channeL
void Epoll::removechannel(Channel *ch)
{
    // ����Ѿ��ں������
    if (ch->inpoll())
    {
        // std::cout<<"removechannel();\n";
        if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, ch->fd(), 0) == -1)
        {
            cout << "epoll_ctl() failed(" << errno << ")." << endl;
            exit(-1);
        }
    }
}

// ����epoll_wait(), �ȴ��¼��������ѷ������¼���vector��������
std::vector<Channel *> Epoll::loop(int timeout)
{
    std::vector<Channel *>channels = {};

    bzero(events_, sizeof(events_));
    // �ȴ����ӵ�fd���¼�������
    // timeout=-1 ��ʾ��Ҫ���޵ȴ����޳�ʱ
    int infds = epoll_wait(epollfd_, events_, MaxEvents, timeout);
    
    // ����ʧ�ܡ�
    if (infds < 0)
    {
        // EBADF ��epfd����һ����Ч����������
        // EFAULT ������eventsָ����ڴ����򲻿�д��
        // EINVAL ��epfd����һ��epoll�ļ������������߲���maxeventsС�ڵ���0��
        // EINTR �����������б��ź��жϣ�epoll_pwait()���Ա��⣬���ߴ������У�����error�����µ���epoll_wait()��
        // ��Reactorģ���У�������ʹ���źţ���Ϊ�źŴ����������鷳��û�б�Ҫ��------ ��˶
        cerr << "epoll_wait() failed.\n";
        exit(-1);
    }

    // ��ʱ��
    if (infds == 0)
    {
        // epoll���r�ˣ�����ϵͳ�ܿ��У����ص�channelΪ��
        // cerr << "epoll_wait() timeout." << endl;
        return channels;
    }

    for(int ii = 0; ii < infds; ii++)
    {
        // ȡ��һ��ʼ�����ch�����ch��Ӧ��һ��socket
        // ������ȡ����ch��Ҳ�����¼�������socket
        Channel *ch = (Channel *)events_[ii].data.ptr;
        ch->setrevents(events_[ii].events);         // ����channel�ķ����¼���Ա
        channels.push_back(ch);
    }

    return channels;
}