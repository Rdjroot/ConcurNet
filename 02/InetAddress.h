#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress
{
private:
    sockaddr_in addr_;

    public:
    // ������fd��ʼ��
    InetAddress(const std::string &ip, uint16_t port);
    // �������Ŀͻ���
    InetAddress(const sockaddr_in addr);

    ~InetAddress();

    // �����ַ�����ʾ��ip��ַ
    const char *ip() const;

    // ����������ʾ�Ķ˿ں�
    uint16_t port() const;
    
    // ����addr_��Ա����ת����sockaddr�ṹ��
    const sockaddr *addr() const;
};