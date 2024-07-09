/**
 * һ���򵥵Ŀͻ��˳���
 */
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>   // ʹ�� <cstdlib> ��� <stdlib.h>����Ϊ���� C++ ��׼���е�ͷ�ļ�
#include <ctime>     // ʹ�� <ctime> ��� <time.h>����Ϊ���� C++ ��׼���е�ͷ�ļ�

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "usage:./client ip port\n";
        cout << "example:./client 192.168.8.128  5085\n";
        return -1;
    }

    int sockfd;

    // ����socketʧ��
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "socket() failed." << endl;
        return -1;
    }

    // C++11����ʹ�þۺϳ�ʼ������ʼ���ṹ��ĳ�Ա
    // ��memset(&servaddr,0,sizeof(servaddr))�������
    struct sockaddr_in servaddr = {};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(std::atoi(argv[2]));      // �ַ��� port -> int port(������) -> �����ֽ���
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);      // �ַ��� ip -> ����� ip

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        cout << "connect(" << argv[1] << ":" << argv[2] << ") failed." << endl;
        close(sockfd);
        return -1;
    }

    cout << "connect ok." << endl;

    // ����/���� ������
    string buf(1024, '\0');

    for (int ii = 0; ii < 200000; ii++)
    {
        buf.assign(buf.size(),'\0');
        cout << "please input: ";
        cin >> buf;

        // ����������ݷ��͸������
        if(send(sockfd, &buf[0],buf.size(),0) <= 0)
        {
            cout<<"write() failed." <<endl;
            close(sockfd);
            return -1;
        }

        buf.assign(buf.size(),'\0');
        if(recv(sockfd, &buf[0], buf.size(), 0) <= 0)
        {
            cout <<" read() failed." <<endl;
            close(sockfd);
            return -1;
        }
        cout<<"recv: "<< buf <<endl;
    }

    close(sockfd);
    return 0;
}