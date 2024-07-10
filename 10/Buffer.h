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
    size_t size();  // ����buf_��С
    const char *data(); // ����buf_���׵�ַ
    void clear();       // ���buf_
};

#endif