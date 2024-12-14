
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdarg.h>

#define MSG_SIZE 1024
#define CHAR_STUF "STUF"

void closeSockets(int n, ...) {
    va_list sockets_fd;
    va_start(sockets_fd, n);
    if (n > 0) {
        for (int i = 0; i < n; i++) {
            close(va_arg(sockets_fd, int));
        }
    }
    va_end(sockets_fd);
}

void errorHandler(char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

char* messageDestuffing(char* message){
    char* msg_copy = message;
    char *stuf_position_pointer = strstr(message, strcat(CHAR_STUF, CHAR_STUF));
    int position = stuf_position_pointer - message;

    
}

int main(int argc, char* argv[]) {
    int server_socket_fd, client_socket_fd,client_read_size;
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_addr_len = sizeof(server_addr), client_addr_len = sizeof(client_addr);
    char *client_message, *client_message_unstuffed, *client_ip, *server_answer;
    char *directory_name, *directory_files_list;
    
    if (argc != 3) {
        printf("Usage: %s <IP server_addr> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip_address = argv[1];
    int port = atoi(argv[2]);

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1) {
        closeSockets(server_socket_fd);
        errorHandler("Could not create socket\n");
    }
    printf("Socket created\n");

    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        closeSockets(server_socket_fd);
        errorHandler("Bind failed. Error");
    }
    printf("Bind done\n");
    
    if(listen(server_socket_fd, 1) < 0) {
        closeSockets(server_socket_fd);
        errorHandler("Could not listen\n");
    }
    printf("Server listening on %s:%d\n", ip_address, port);

    client_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket_fd == -1) {
        closeSockets(client_socket_fd, server_socket_fd);
        errorHandler("Could not accept client connection\n");
    }
    client_ip = inet_ntoa(client_addr.sin_addr);
    printf("Accept connection of client %s\n", client_ip);

    client_read_size = recv(client_socket_fd, client_message, MSG_SIZE - 1, 0);
    if (client_read_size < -1) {
        closeSockets(client_socket_fd, server_socket_fd);
        errorHandler("Could not read client message\n");
    }
    
    if (strcmp(client_message, "READY") != 0) {
        closeSockets(client_socket_fd, server_socket_fd);
        errorHandler("Client Message not known\n");
    }
    printf("Client starts directory list save service.");

    server_answer = "READY ACK";
    send(client_socket_fd, server_answer, strlen(server_answer), 0);

    do {
        client_read_size = recv(client_socket_fd, client_message, MSG_SIZE - 1, 0);
        if (client_read_size < -1) {
            closeSockets(client_socket_fd, server_socket_fd);
            errorHandler("Could not read client message\n");
        }
        client_message_unstuffed = client_message


    } while(client_read_size > 0 || client_message_unstuffed != "bye");

    closeSockets(client_socket_fd, server_socket_fd);
}