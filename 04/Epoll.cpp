#include "Epoll.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

Epoll::Epoll()
{
    if ((epollfd_ = epoll_create(1)) == -1) // ����epoll��������������
    {
        cerr << "epoll_create() failed(" << errno << ")." << endl;
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
void Epoll::addfd(int fd, uint32_t op)
{
    // Ϊfd׼���¼���
    epoll_event ev;  // �����¼������ݽṹ��
    ev.data.fd = fd; // ָ���¼����Զ������ݣ�������epoll_wait()���ص��¼�һ�����ء�
    ev.events = op;  // ����ˮƽ������

    // ����Ҫ���ӵ�listenfd�������¼�����epollfd�С�
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        cout << "epoll_ctl() failed(" << errno << ")." << endl;
        exit(-1);
    }
}

std::vector<epoll_event> Epoll::loop(int timeout)
{
    // ���epoll_wait()���ص��¼�
    std::vector<epoll_event> evs = {};

    // �ȴ����ӵ�fd���¼�������
    // -1 ��ʾ��Ҫ���޵ȴ�
    int infds = epoll_wait(epollfd_, events_, MaxEvents, timeout);

    // ����ʧ�ܡ�
    if (infds < 0)
    {
        cerr << "epoll_wait() failed" << endl;
        exit(-1);
    }

    // ��ʱ��
    if (infds == 0)
    {
        cerr << "epoll_wait() timeout." << endl;
        return evs;
    }

    for(int ii = 0; ii < infds; ii++)
    {
        evs.push_back(events_[ii]);
    }

    return evs;
}
