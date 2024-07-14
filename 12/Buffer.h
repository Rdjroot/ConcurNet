#ifndef BUFFER_H_
#define BUFFER_H_
#include <string>
#include <iostream>

class Buffer
{
private:
    std::string buf_;      // ���ڴ������
public:
    Buffer();
    ~Buffer();

    void append(const char *data, size_t size);         // ׷������
    void appendwithhead(const char *data, size_t size);         // ������׷�ӵ�buffer�У�����׷�ӱ���ͷ��
    size_t size();  // ����buf_��С
    const char *data(); // ����buf_���׵�ַ
    void clear();       // ���buf_
    void erase(size_t pos, size_t sz);      // ��pos��ʼɾ��sz���ַ�
};

#endif