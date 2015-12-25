#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <thread>
#include "socketbuffer.h"
#include "protocol.h"
#include "cache.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
 		int optval = 0;
 		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
 			perror("could not set SO_REUSEADDR");
        	exit(EXIT_FAILURE);
 		}
 		if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
 			perror("binderror");
    		fprintf(stderr, "bind error:%d\n", errno);
    		exit(1);
    	}

    	if (listen(sockfd, 1) == -1) {
    		fprintf(stderr, "listen error:%d\n", errno);
    		exit(1);
    	}

    	struct sockaddr_storage conn_addr;
		socklen_t addr_size = sizeof(conn_addr);
		
		std::vector<std::thread> threads;
		while (true) {
			int conn_fd = accept(sockfd, (struct sockaddr *)&conn_addr, &addr_size);
			if (conn_fd == -1) {
				fprintf(stderr, "accept error%d\n", errno);
				exit(1);
			}

			//threads.push_back(std::thread([conn_fd, this]() {
        		/*this->*/ProcessConnection(conn_fd);
    		//}));
		}

		/*for (auto& t : threads) {
    		t.join();
		}*/
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
	Cache<std::string, std::vector<char>> cache(4096);

	while(!srb.Closed()) {
		//pthread_mutex_lock(&mutex);
		McCommand cmd;
		cmd.Deserialize(&srb);
		//pthread_mutex_unlock(&mutex);
		if (cmd.command == CMD_GET) {
			for (std::string key : cmd.keys) {
				std::cout << key;

				std::vector<char> value;
				time_t exp_time;
				time_t update_time;

				bool flag = cache.get(key, &value, &exp_time, &update_time);

				if (flag) {
					std::vector<McValue> out;
					out.emplace_back(key, cmd.flags, value);
					McResult mr(out);
					mr.Serialize(&swb);
				} else {
					McResult mr(R_NOT_FOUND);
					mr.Serialize(&swb);
				}
			}
		}

		if (cmd.command == CMD_ADD) {
			for (std::string key : cmd.keys) {
				cache.set(key, cmd.exp_time, cmd.data);
			}
			McResult mr(R_STORED);
			mr.Serialize(&swb);
		}

		if (cmd.command == CMD_SET) {
			std::cout << "set1\n";
			for (std::string key : cmd.keys) {
				cache.set(key, cmd.exp_time, cmd.data);
			}
			std::cout << "set2\n";
			McResult mr(R_STORED);
			std::cout << "set3\n";
			mr.Serialize(&swb);
			std::cout << "set4\n";
		}

		if (cmd.command == CMD_DELETE) {
			for (auto key : cmd.keys) {
				bool flag = cache.remove(key);
				if (flag) {
					McResult mr(R_DELETED);
					mr.Serialize(&swb);
				} else {
					McResult mr(R_NOT_FOUND);
					mr.Serialize(&swb);				
				}
			}
		}
		swb.Flush();
	}
}

int main(int argc, char const *argv[]) {
	McServer ms(argv[1]);
	ms.Run();
	return 0;
}
