#include <iostream>
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

			Connect();
		}
	}


	virtual void ProcessConnection(int fd) = 0;
 protected:
 	int sockfd;
 	struct addrinfo *servinfo;
};

class McServer : public Server {
 public:
 	McServer(const char* port): Server(port) {}
 	virtual void ProcessConnection() override;
};

void McServer::ProcessConnection(int fd) {
	SocketRBuffer srb(sockfd, 4096);
	SocketWBuffer swb(fd, 4096);

	McCommand command.Deserialize(srb);

	// I don't know what I should write here
}

int main(int argc, char const *argv[]) {
	char* port; 
	std::cin >> *port;
	McServer ms(port);
	ms.Run();
	return 0;
}