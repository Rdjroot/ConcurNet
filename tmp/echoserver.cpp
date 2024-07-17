/**
 * 一个简单的epoll服务端程序
 * 主要监听写事件
 *
 */
#include<signal.h>
#include "EchoServer.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

// 1、设置2和15的信号
// 2、在信号处理函数中停止主从时间循环和工作线程
// 3、服务程序主动退出

EchoServer *echoserver;

void Stop(int sig)
{
    cout<<"sig="<<sig<<endl;
    // 调用Echoserver的Stop()停止服务
    echoserver->Stop();
    cout<<"echoserver 已停止服务"<<endl;
    delete echoserver;
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "usage: ./echoserver ip port\n";
        cout << "example: ./echoserver 192.168.8.128  5085\n";
        return -1;
    }

    signal(SIGTERM,Stop);       // 信号15 系统kill或者killall命令
    signal(SIGINT, Stop);       // 信号2 Ctrl + C


    // 服务端的地址和协议
    echoserver = new EchoServer(argv[1], atoi(argv[2]),5,0);
    // 开启事件循环
    echoserver->Start();

    return 0;
}