#include "Connection.h"

// 构造函数
// 为新客户端连接准备读事件和属性设置，并添加到epoll中。
Connection::Connection(EventLoop *loop, std::unique_ptr<Socket> clientsock)
    : loop_(loop), clientsock_(std::move(clientsock)),
      clientchannel_(new Channel(loop_, clientsock_->fd())), disconnect_(false)
{
    // 绑定回调函数
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback, this));
    clientchannel_->useet();         // 设置边缘触发，
    clientchannel_->enablereading(); // 将新的客户端fd的读事件添加到epoll中
}

Connection::~Connection()
{
}

int Connection::fd() const
{
    return clientsock_->fd();
}

std::string Connection::ip() const
{
    return clientsock_->ip();
}

uint16_t Connection::port() const
{
    return clientsock_->port();
}

void Connection::onmessage()
{
    std::string buffer(1024, '\0');
    while (true) // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {
        buffer.assign(1024, '\0');
        // 从套接字中读数据
        ssize_t nread = ::read(fd(), &buffer[0], sizeof(buffer));

        // 成功的读取到了数据。
        if (nread > 0)
        {
            // 把接收到的报文内容存到buffer中。
            inputbuffer_.append(buffer.data(), nread);
        }
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            // 可以把以下代码封装在Buffer类中，还可以支持固定长度、指定报文长度和分隔符等多种格式。
            while (true)
            {
                // 如果缓冲区的大小不足以四字节
                if (inputbuffer_.size() < sizeof(int))
                {
                    break;
                }

                // 取出报文首部
                int len = *reinterpret_cast<const int *>(inputbuffer_.data());

                // 如果inputbuffer_的数据量小于报文头部，说明inputbuffer_中的报文不完整
                if (inputbuffer_.size() < static_cast<size_t>(len + 4))
                    break;

                // 从inputbuffer中取出一个报文（略过报文头部）
                std::string message(inputbuffer_.data() + 4, len);
                inputbuffer_.erase(0, len + 4); // 删除已经取出的数据

                std::cout << "message (eventfd=" << fd() << "):" << message << std::endl;

                lastime_ = Timestamp::now();        // 更新时间戳
                // std::cout <<"lasttime="<<lastime_.tostring() <<std::endl;

                onmessagecallback_(shared_from_this(), message);
            }
            break;
        }
        else if (nread == 0) // 客户端连接已断开。
        {
            // clientchannel_->remove();
            closecallback();
            break;
        }
    }
}

void Connection::closecallback()
{
    disconnect_ = true;
    clientchannel_->remove();
    closecallback_(shared_from_this());
}

void Connection::errorcallback()
{
    disconnect_ = true;
    clientchannel_->remove();
    errorcallback_(shared_from_this());
}

void Connection::writecallback()
{
    // 尝试把发送缓冲区的数据全部发出去
    int writen = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if (writen > 0)
        outputbuffer_.erase(0, writen);

    if (outputbuffer_.size() == 0)
    {
        // 如果数据已经全部发送，不再关注写事件。
        clientchannel_->disablewriting();
        sendcompletecallback_(shared_from_this());
    }
}

void Connection::setclosecallback(std::function<void(spConnection)> fn)
{
    closecallback_ = fn;
}

void Connection::seterrorcallback(std::function<void(spConnection)> fn)
{
    errorcallback_ = fn;
}

void Connection::setonmessagecallback(std::function<void(spConnection, std::string &)> fn)
{
    onmessagecallback_ = fn;
}

void Connection::setsendcompletecallback(std::function<void(spConnection)> fn)
{
    sendcompletecallback_ = fn;
}

// 发送数据（任何线程都是调用此函数）
void Connection::send(const char *data, size_t sz)
{
    if (disconnect_)
    {
        std::cout << "客户端连接已断开。。。send()直接返回。" << std::endl;
        return;
    }
    std::shared_ptr<std::string> message(new std::string(data));

    // 判断当前线程是否为事件循环线程（IO线程）
    if (loop_->isinloopthread())
    {
        // 如果是IO线程，直接执行发送数据的操作
        std::cout << "send()在事件循环（IO）的线程中。\n";
        sendinloop(message);
    }
    else
    {
        // 如果当前线程不是IO线程，把发送数据的操作转交给事件循环线程去执行
        std::cout << "send()不在事件循环（IO）的线程中。\n";
        loop_->queueinloop(std::bind(&Connection::sendinloop,this,message));
    }
}

void Connection::sendinloop(std::shared_ptr<std::string> data)
{
    // 把数据存到缓冲区
    outputbuffer_.appendwithhead(data->data(), data->size());
    clientchannel_->enablewriting(); // 注册写事件d
}
