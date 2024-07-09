#include "Channel.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

Channel::Channel(Epoll *ep, int fd) : ep_(ep), fd_(fd)
{
}

Channel::~Channel()
{
    // ����fd_��ep_�����඼ֻ��ʹ���������ǲ����ڱ��࣬���Բ������ٺ͹ر�
}

int Channel::fd()
{
    return fd_;
}
// �ж�Channel�Ƿ��ں������
bool Channel::inpoll()
{
    return inepoll_;
}

// ��Ҫ���ӵ��¼�
uint32_t Channel::events()
{
    return events_;
}

// �ѷ������¼�
uint32_t Channel::revents()
{
    return revents_;
}

// ���ñ�Ե����
void Channel::useet()
{
    events_ = events_ | EPOLLET;
}

// ���Ӷ��¼�
void Channel::enablereading()
{
    events_|=EPOLLIN;
    ep_->updatechannel(this);
}

// ��¼Channel����ӵ�epoll�ĺ������
void Channel::setinepoll()
{
    inepoll_ = true;
}

// �����ѷ������¼�
void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}

// ���������¼�
void Channel::handleevent()
{
    // �Է��ѹرգ���Щϵͳ��ⲻ��������ʹ��EPOLLIN��recv()����0��
    // EPOLLRDHUP ��ʾ�Զ˹ر����ӻ��رա�
    if (revents_ & EPOLLRDHUP)
    {
        cout << "client(eventfd=" << fd_ << ") disconnected.\n";
        // �رոÿͻ��˵�fd��
        close(fd_);
    }
    //  EPOLLIN ��ͨ����  EPOLLPRI��������
    else if (revents_ & (EPOLLIN | EPOLLPRI)) // ���ջ������������ݿ��Զ���
    {
        // ������µ����ӣ�����fd�󶨵�Ӧ����newconnection
        // ���fd�ǿͻ��ˣ�����Ӧ�ð󶨵�Ӧ���� onmessage
        readcallback_();
    }
    else if (revents_ & EPOLLOUT) // ��������Ҫд����ʱû�д��룬�Ժ���˵��
    {
    }
    else // �����¼�������Ϊ����
    {
        cout << "client(eventfd=" << fd_ << ") error." << endl;
        close(fd_); // �رտͻ��˵�fd��
    }
}

void Channel::newconnection(Socket *servsock)
{
    InetAddress clientaddr;

    // �����µĿͻ��˵����ӣ���accept�����ó��˷�������
    Socket *clientsock = new Socket(servsock->accept(clientaddr));

    cout << "accept client(fd=" << clientsock->fd() << ", ip=" << clientaddr.ip() << ", port=" << clientaddr.port() << ") ok." << endl;

    // Ϊ�¿ͻ�������׼�����¼����������ã�����ӵ�epoll�С�
    Channel *clientchannel = new Channel(ep_, clientsock->fd());
    // �󶨻ص�����
    clientchannel->setreadcallback(std::bind(&Channel::onmessage, clientchannel));
    clientchannel->useet();         // ���ñ�Ե������
    clientchannel->enablereading(); // ���µĿͻ���fd�Ķ��¼���ӵ�epoll��
}

void Channel::onmessage()
{
    string buffer(1024, '\0');
    while (true) // ����ʹ�÷�����IO��һ�ζ�ȡbuffer��С���ݣ�ֱ��ȫ�������ݶ�ȡ��ϡ�
    {
        buffer.assign(buffer.size(), '\0');

        // ���׽����ж�����
        ssize_t nread = read(fd_, &buffer[0], sizeof(buffer));

        // �ɹ��Ķ�ȡ�������ݡ�
        if (nread > 0)
        {
            // �ѽ��յ��ı�������ԭ�ⲻ���ķ���ȥ��
            cout << "recv(eventfd=" << fd_ << "):" << buffer << endl;
            send(fd_, &buffer[0], buffer.size(), 0);
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
            cout << "client(eventfd=" << fd_ << ") disconnected." << endl;
            close(fd_); // �رտͻ��˵�fd��
            break;
        }
    }
}

void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_ = fn;
}
