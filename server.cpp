#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

std::unordered_map<std::string, std::string> database;
std::mutex db_mutex;    // Mutex for synchronizing/excluding access to the database

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    
    while(true) {
        ssize_t bytes_rec = recv(client_socket, buffer, BUFFER_SIZE- 1, 0);
        // received nothing
        if (bytes_rec <= 0) break;
        buffer[bytes_rec] = '\0';
        std::string input(buffer);
        std::istringstream iss(input);   // easy parsing of input string
        std::string cmd, key, value;
        iss >> cmd; // get the command first

        if (cmd == "SET"){
            iss >> key >> value;
            // check valid key and value
            if (key.empty() || value.empty()) {
                // key or value is missing
                std::string err = "-ERR: wrong number of arguments for 'SET'\r\n";
                send(client_socket, err.c_str(), err.size(), 0);
            } else {
                // valid command arguments
                std::lock_guard<std::mutex> lock(db_mutex);
                // lock() ensures only one thread has access to the db at a time.
                // prevents race condition
                database[key] = value;
                std::string success = "+SUCCESS\r\n";
                send(client_socket, success.c_str(), success.size(), 0);
            }
        } else if (cmd == "GET") {
            // client sends a key to get
            iss >> key; 
            std::lock_guard<std::mutex> lock(db_mutex);
            auto iter = database.find(key); // returns an iterator
            // check if key exists
            if (iter == database.end()) {
                // key doesn't exist
                std::string nil = "-1\r\n";
                send(client_socket, nil.c_str(), nil.size(), 0);
            } else {
                std::string val = "$" + std::to_string(iter->second.size()) + "\r\n" + iter->second + "\r\n";   // return convention
                send(client_socket, val.c_str(), val.size(), 0);
            }
        } else {    // unexpect command
            std::string err = "-ERR unknown command\r\n";
            send(client_socket, err.c_str(), err.size(), 0);
        }
    }
    // end of connection
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);

    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true){
        client_socket = accept(server_fd, (sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }
        std::thread([client_socket]() {
            handle_client(client_socket);
        }).detach();
    }
    return 0;
}
