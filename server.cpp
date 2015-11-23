#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "socketbuffer.h"
#include "protocol.h"

class Server {
 public:
 	Server(const char* port) {
 		int status;
		struct addrinfo hints;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
    		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    		exit(1);
    	}

    	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    	if (sockfd == -1) {
    		fprintf(stderr, "socket error:%d\n", errno);
    		exit(1);
    	}
 	}

 	void Run() {
 		if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
    		fprintf(stderr, "bind error:%d\n", errno);
    		exit(1);
    	}

    	if (listen(sockfd, 1) == -1) {
    		fprintf(stderr, "listen error:%d\n", errno);
    		exit(1);
    	}

    	struct sockaddr_storage conn_addr;
		socklen_t addr_size = sizeof(conn_addr);
		while (true) {
			int conn_fd = accept(sockfd, (struct sockaddr *)&conn_addr, &addr_size);
			if (conn_fd == -1) {
				fprintf(stderr, "accept error%d\n", errno);
				exit(1);
			}

			ProcessConnection(sockfd);
		}

		freeaddrinfo(servinfo);
	}


	virtual void ProcessConnection(int fd) = 0;
 protected:
 	int sockfd;
 	struct addrinfo *servinfo;
};

class McServer : public Server {
 public:
 	McServer(const char* port): Server(port) {}
 	void ProcessConnection(int fd) override;
};

void McServer::ProcessConnection(int fd) {
	SocketRBuffer srb(fd, 4096);
	SocketWBuffer swb(fd, 4096);

	McCommand cmd;
	cmd.Deserialize(&srb);

	if (cmd.command == CMD_GET) {
		std::string key("Forget your troubles");
		int flags = 1;
		const std::vector<char> data_block({'B', 'e', ' ', 'h', 'a', 'p', 'p', 'y'});
		McValue mv(key, flags, data_block);

		std::vector<McValue> vmv(1, mv);
		McResult mr(vmv);
		mr.Serialize(&swb);
	}

	if (cmd.command == CMD_ADD) {
		McResult mr(R_STORED);
		mr.Serialize(&swb);
	}

	if (cmd.command == CMD_SET) {
		McResult mr(R_STORED);
		mr.Serialize(&swb);
	}

	if (cmd.command == CMD_DELETE) {
		McResult mr(R_STORED);
		mr.Serialize(&swb);
	}

	swb.Flush();
}

int main(int argc, char const *argv[]) {
	McServer ms("4444");
	ms.Run();
	return 0;
}
