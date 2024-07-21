#include "Epoll.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

Epoll::Epoll()
{
    if ((epollfd_ = epoll_create(1)) == -1) // ����epoll��������������
    {
        logger.logFormatted(LogLevel::ERROR, "epoll_create() failed(%d).", errno);
        exit(-1);
    }
}

Epoll::~Epoll()
{
    close(epollfd_);
}

// ��channel���/���µ�������ϣ�channel����fd��Ҳ����Ҫ���ӵ��¼���
void Epoll::updatechannel(Channel *ch)
{
    // Ϊfd׼���¼���
    epoll_event ev;           // �����¼������ݽṹ��
    ev.data.ptr = ch;         // ָ��Channel
    ev.events = ch->events(); // ָ���¼�(���¼���д�¼�;ˮƽ����|��Ե����)

    // ����Ѿ��ں������
    if (ch->inpoll())
    {
        // ���¼����¼�ģʽ
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1)
        {
            logger.logFormatted(LogLevel::ERROR, "epoll_ctl() for MOD failed(%d).", errno);
            exit(-1);
        }
    }
    else
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1)
        {
            logger.logFormatted(LogLevel::ERROR, "epoll_ctl() for ADD failed(%d).", errno);
            exit(-1);
        }
        ch->setinepoll(); // ����Ѿ�����
    }
}

// �Ӻ������ɾ��channeL
void Epoll::removechannel(Channel *ch)
{
    // ����Ѿ��ں������
    if (ch->inpoll())
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, ch->fd(), 0) == -1)
        {
            logger.logFormatted(LogLevel::ERROR, "epoll_ctl() for DEL failed(%d).", errno);
            exit(-1);
        }
    }
}

// ����epoll_wait(), �ȴ��¼��������ѷ������¼���vector��������
std::vector<Channel *> Epoll::loop(int timeout)
{
    std::vector<Channel *> channels = {};

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
        logger.log(LogLevel::ERROR, "epoll_wait() failed.");
        exit(-1);
    }

    // ��ʱ��
    if (infds == 0)
    {
        // epoll���r�ˣ�����ϵͳ�ܿ��У����ص�channelΪ��
        // cerr << "epoll_wait() timeout." << endl;
        return channels;
    }

    // ���infds>0����ʾ���¼�������fd������
    for (int ii = 0; ii < infds; ii++)
    {
        // ȡ��һ��ʼ�����ch�����ch��Ӧ��һ��socket
        // ������ȡ����ch��Ҳ�����¼�������socket
        Channel *ch = (Channel *)events_[ii].data.ptr;
        ch->setrevents(events_[ii].events); // ����channel�ķ����¼���Ա
        channels.push_back(ch);
    }

    return channels;
}