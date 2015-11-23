#include "buffer.h"

class SocketRBuffer : public RBuffer {
 public:
 	SocketRBuffer(int fd_value, size_t buffer_size_value);

 	void ReadMore();
 	char ReadChar();
 	void ReadCharCheck(char check);
 	uint32_t ReadUint32();
 	std::string ReadField(char sep);
 	std::vector<char> ReadBytes(size_t bytes_num);
 protected:
 	int fd;
 	int buffer_size;
};

class SocketWBuffer : public WBuffer {
 public:
 	SocketWBuffer(int fd_value, size_t buffer_size);

 	void Flush() override;
 	void WriteChar(char ch);
 	void WriteUint32(uint32_t v);
 	void WriteField(std::string field);
 	void WriteField(std::string field, char sep);
 	void WriteBytes(const std::vector<char>& buffer);
 protected:
 	int fd;
};

