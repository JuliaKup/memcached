#include "socketbuffer.h"
#include "buffer.h"
#include <algorithm>
#include <iostream>
#include <unistd.h>

SocketRBuffer::SocketRBuffer(int fd_value, size_t buffer_size_value)
        : RBuffer(buffer_size_value)
        , fd(fd_value)
        , closed_(false) {
    ReadMore();
}

void SocketRBuffer::ReadMore() {
    pos_ = 0;
    char* data = buffer_.data();

    int rd = read(fd, data, buffer_.size());
    end_ = rd;

    if (rd == 0) {
        close(fd);
        closed_ = true;
        //throw std::runtime_error("Failed to read more from SocketRBuffer");
    }
}

SocketRBuffer::~SocketRBuffer() {
    if (!closed_) {
        close(fd);
        closed_ = true;
    }
}

char SocketRBuffer::ReadChar() {
    if (pos_ == end_) {
        ReadMore();
    }
    return buffer_[pos_++];
}

void SocketRBuffer::ReadCharCheck(char check) {
    if (ReadChar() != check) {
        throw std::runtime_error("Invalid character in input " + std::to_string(buffer_[pos_]) + " should be " + std::to_string(check));
    }
}

uint32_t SocketRBuffer::ReadUint32() {
    uint32_t value = 0;
    char ch;
    while (isdigit(ch = ReadChar())) {
        int digit = ch - '0';
        value *= 10;
        value += digit;
    }
    --pos_;

    return value;
}

std::string SocketRBuffer::ReadField(char sep) {
    std::string field;
    char ch;
    while ((ch = ReadChar()) != sep) {
        field.push_back(ch);
    }
    --pos_;
    return field;
}

std::vector<char> SocketRBuffer::ReadBytes(size_t bytes_num) {
    std::vector<char> bytes(bytes_num);
    for (size_t i = 0; i < bytes_num; ++i) {
        bytes[i] = ReadChar();
    }

    return bytes;
}

SocketWBuffer::SocketWBuffer(int fd_value, size_t buffer_size):
    WBuffer(buffer_size), fd(fd_value) {}

void SocketWBuffer::Flush() {
    int write_bytes;
    char* data = buffer_.data();
    int bytes_to_write = pos_;
    while ((bytes_to_write > 0) && (write_bytes = write(fd, data, bytes_to_write)) > 0) {
        data += write_bytes;
        bytes_to_write -= write_bytes;
    }

    if (write_bytes == 0 && pos_) {
        close(fd);
        closed_ = true;
        throw std::runtime_error("Failed to write more to SocketWBuffer");
    }
    pos_ = 0;
}

void SocketWBuffer::WriteChar(char ch) {
    buffer_[pos_++] = ch;
    if (pos_ == buffer_.size()) {
        Flush();
    }
}

void SocketWBuffer::WriteUint32(uint32_t v) {
    char buf[35];
    int chw;
    snprintf(buf, 32, "%u%n", v, &chw);
    for (int i = 0; i < chw; ++i) {
        WriteChar(buf[i]);
    }
}

void SocketWBuffer::WriteField(std::string field) {
    for (size_t i = 0; i < field.size(); ++i) {
        WriteChar(field[i]);
    }
}

void SocketWBuffer::WriteField(std::string field, char sep) {
    WriteField(field);
    WriteChar(sep);
}

void SocketWBuffer::WriteBytes(const std::vector<char>& buffer) {
    for (char c : buffer) {
        WriteChar(c);
    }
}

SocketWBuffer::~SocketWBuffer() {
    if (!closed_) {
        close(fd);
        closed_ = true;
    }
}