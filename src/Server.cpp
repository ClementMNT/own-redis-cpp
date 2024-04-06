#include "./Server.hpp"

std::string stringToBulk(const std::string &str) {
	return "$" + std::to_string(str.size()) + "\r\n" + str + "\r\n";
}

void cmdEcho(int client_fd, const std::vector<std::string> &args) {
	if (args.size() != 2) {
		send(client_fd, "-ERR wrong number of arguments for 'echo' command\r\n", 47, 0);
	} else {
		std::string response = stringToBulk(args[1]);
		send(client_fd, response.c_str(), response.size(), 0);
	}
}

void cmdPing(int client_fd, const std::vector<std::string> &args) {
	if (args.size() > 2) {
		send(client_fd, "-ERR wrong number of arguments for 'ping' command\r\n", 47, 0);
	} else {
		std::string response = "+PONG\r\n";
		send(client_fd, response.c_str(), response.size(), 0);
	}
}

int handle_client(int client_fd) {
    char buffer[1024];
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        return 84;
    }

    std::string request(buffer, bytes_received);
    RedisParser parser(request);
    std::vector<std::string> args = parser.parse();

    if (args.empty()) {
        send(client_fd, args[0].c_str(), args[0].size(), 0);
        return 0;
    }

    std::string command = args[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
	if (command == "echo") {
		cmdEcho(client_fd, args);
	} else if (command == "ping") {
		cmdPing(client_fd, args);
	} else {
		send(client_fd, args[0].c_str(), args[0].size(), 0);
	}
    return 0;
}



int main(int argc, char **argv) {
	// You can use print statements as follows for debugging, they'll be visible when running tests.
	std::cout << "Logs from your program will appear here!\n";

	// Uncomment this block to pass the first stage
	
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
	std::cerr << "Failed to create server socket\n";
	return 1;
	}
	
	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		std::cerr << "setsockopt failed\n";
		return 1;
	}
	
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(6379);
	
	if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
		std::cerr << "Failed to bind to port 6379\n";
		return 1;
	}
	
	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		std::cerr << "listen failed\n";
		return 1;
	}
	
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	
	std::cout << "Waiting for a client to connect...\n";

	fd_set master_set;
	fd_set read_fds;
	int max_fd = server_fd;

	FD_ZERO(&master_set);
	FD_SET(server_fd, &master_set);

	while(true) {
		read_fds = master_set;
		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			std::cerr << "select failed\n";
			return 1;
		}

		for (int i = 0; i <= max_fd; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == server_fd) {
					int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
					if (client_fd == -1) {
						std::cerr << "accept failed\n";
					} else {
						FD_SET(client_fd, &master_set);
						max_fd = std::max(max_fd, client_fd);
						std::cout << "Client connected\n";
					}
				} else {
					if (handle_client(i) == 84) {
						FD_CLR(i, &master_set);
						close(i);
						std::cout << "Client disconnected\n";
					}
				}
			}
		}
	}

	close(server_fd);

	return 0;
}
