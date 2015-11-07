#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/*struct sockaddr {
    unsigned short sa_family;
    char sa_data[14];
};

struct in_addr {
    uint32_t s_addr;
};

struct sockaddr_in {
    short int sin_family;
    unsigned short sin_port;
    struct in_addr sin_addr;
    unsigned char sin_zero[8];
};

struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    struct sockaddr *ai_addr;
    char *ai_canonname;
    struct addrinfo *ai_next;
};*/

int main(int argc, char const *argv[]) {
	if (argc > 1) {
		int status;
		struct addrinfo hints;
		struct addrinfo *servinfo;
		const char* port = argv[1];

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
    		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    		exit(1);
    	}

    	int sockfd;

    	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    	if (sockfd == -1) {
    		fprintf(stderr, "socket error:%d\n", errno);
    		exit(1);
    	}

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

			std::vector<char> buffer;
			char buf[4096];

			// read all data from socket
			int read_bytes;
			while ((read_bytes = read(conn_fd, buf, sizeof(buffer))) > 0) {
			    buffer.insert(buffer.end(), buf, buf + read_bytes);
			}

			// write all data to socket
			int write_bytes;
			char* data = buffer.data();
			int bytes_to_write = buffer.size();
			while ((write_bytes = write(conn_fd, data, bytes_to_write)) > 0) {
			    data += write_bytes;
			    bytes_to_write -= write_bytes;
			}

			if (bytes_to_write > 0) {  // not all written, probably socket on the other side closed early
			    std::cout << "Socket closed early";
			}
		}

    	freeaddrinfo(servinfo);
	}
	return 0;
}