#include "gtest/gtest.h"
#include "socketbuffer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>

TEST(SBuffer, ReadChar) {
	int socks[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, socks), 0);
	//pipe(socks);

	SocketRBuffer srb(socks[0], 5);

	std::vector<char> buf(5, 'a');
	char *data = buf.data();

	ASSERT_EQ(write(socks[1], data, 5), 5);

	ASSERT_EQ('a', srb.ReadChar());
}

TEST(SBuffer, ReadUint32) {
	int socks[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, socks), 0);

	SocketRBuffer srb(socks[0], 10);
	std::vector<char> buf({'1', '2', '2', ',', '4', '5', '6', '7', ' '});
	char *data = buf.data();

	write(socks[1], data, 10);
	ASSERT_EQ(122, srb.ReadUint32());
	ASSERT_EQ(',', srb.ReadChar());
	ASSERT_EQ(4567, srb.ReadUint32());
	ASSERT_EQ(' ', srb.ReadChar());
}

TEST(SBuffer, ReadField) {
	int socks[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, socks), 0);

	SocketRBuffer srb(socks[0], 10);
	std::vector<char> buf({'a', 'b', 'c', ',', 't', 'r', 'i', 'p', ' '});
	char *data = buf.data();

	write(socks[1], data, 10);
	ASSERT_EQ("abc", srb.ReadField(','));
	ASSERT_EQ(',', srb.ReadChar());
	ASSERT_EQ("trip", srb.ReadField(' '));
	ASSERT_EQ(' ', srb.ReadChar());
}

TEST(SBuffer, WriteChar) {
	int socks[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, socks), 0);

	SocketWBuffer swb(socks[0], 1);

	swb.WriteChar('a');
	swb.Flush();

	char a[1];
	ASSERT_EQ(read(socks[1], a, sizeof(a)), sizeof(a));
	ASSERT_EQ('a', a[0]);
}

TEST(SBuffer, WriteUint32) {
	int socks[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, socks), 0);

	SocketWBuffer swb(socks[0], 1000);

	swb.WriteUint32(1000000);
	swb.Flush();

	char c[32];
	ASSERT_EQ(read(socks[1], c, sizeof(c)), sizeof(c));
	ASSERT_EQ('1', c[0]);
	ASSERT_EQ('0', c[1]);
	ASSERT_EQ('0', c[2]);
	ASSERT_EQ('0', c[3]);
	ASSERT_EQ('0', c[4]);
	ASSERT_EQ('0', c[5]);
	ASSERT_EQ('0', c[6]);
}

TEST(SBuffer, WriteMulti) {
	int socks[2];
	ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, socks), 0);
	SocketWBuffer swb(socks[0], 1000);

	swb.WriteField("Pray for Paris");
	swb.WriteChar(' ');
	swb.WriteUint32(13);
	swb.WriteChar('.');
	swb.WriteUint32(11);
	swb.WriteChar('.');
	swb.WriteUint32(2015);
	swb.WriteChar('\0');
	swb.Flush();

	char b[26];
	ASSERT_EQ(read(socks[1], b, sizeof(b)), sizeof(b));
	std::string str(b);
	ASSERT_EQ("Pray for Paris 13.11.2015", str);
}