
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdarg.h>
#include <math.h>
#include <sys/stat.h>

#define MAX(a,b) (((a)>(b))?(a):(b))
#define DIRECTORY "server-files"

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
    char *dest = malloc(10255);
    char *newMsg = malloc(10255);
    strcpy(newMsg, message);
    char *stuf_position_pointer = strstr(newMsg, "byye");
    if (stuf_position_pointer == NULL) return message;
    
    while(stuf_position_pointer != NULL) {
        int position = stuf_position_pointer - newMsg;
        
        free(dest);
        dest = malloc(10255);
        strncpy(dest, newMsg + 0, position);
        dest[position] = '\0';
        strcat(dest, "bye");
        strncat(dest, newMsg + (position + 4), strlen(newMsg) - (position + 4));
        free(newMsg);
        newMsg = malloc(10255);
        strcpy(newMsg, dest);
        stuf_position_pointer = strstr(newMsg, "byye");
    }

    return dest;
}

int main(int argc, char* argv[]) {
    struct stat st = {0};
    int server_socket_fd, client_socket_fd,client_read_size;
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_addr_len = sizeof(server_addr), client_addr_len = sizeof(client_addr);
    char *client_connection_messages, *client_message, *client_message_unstuffed, *client_ip, *server_answer;
    char *client_directory_name, *client_file_name, *client_file_path;
    FILE *client_file;
    
    if (argc != 4) {
        printf("Usage: %s <IP server_addr> <Port> <Message Size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(DIRECTORY, &st) == -1) {
        if (mkdir(DIRECTORY, 0777) != 0) {
            errorHandler("Erro ao criar o diretório");
        }
        printf("Diretório '%s' criado com sucesso.\n", DIRECTORY);
    } else {
        printf("Diretório '%s' já existe.\n", DIRECTORY);
    }

    const char *ip_address = argv[1];
    int port = atoi(argv[2]);
    const int MSG_SIZE = atoi(argv[3]);

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1) {
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

    client_connection_messages = malloc(sizeof(char)*5);
    client_read_size = recv(client_socket_fd, client_connection_messages, sizeof(char)*5, 0);
    if (client_read_size < 0) {
        closeSockets(client_socket_fd, server_socket_fd);
        errorHandler("Could not read client message\n");
    }
    if (strcmp(client_connection_messages, "READY") != 0) {
        closeSockets(client_socket_fd, server_socket_fd);
        errorHandler("Client Message not known\n");
    }
    printf("Starts directory list save service.\n");
    free(client_connection_messages);

    server_answer = "READY ACK";
    send(client_socket_fd, server_answer, sizeof(char)*strlen(server_answer), 0);

    client_read_size = recv(client_socket_fd, client_message, pow(2,16), 0);
    if (client_read_size < 0) {
        closeSockets(client_socket_fd, server_socket_fd);
        errorHandler("Could not read client message\n");
    }
    client_message_unstuffed = messageDestuffing(client_message);
    strcpy(client_directory_name, client_message_unstuffed);
    printf("Received client directory name: %s\n", client_directory_name);

    client_file_name = malloc(pow(2,16));
    strcpy(client_file_name, client_ip);
    strcat(client_file_name, ":");
    strcat(client_file_name, argv[2]);
    strcat(client_file_name, "-");
    strcat(client_file_name, client_directory_name);
    strcat(client_file_name, ".txt");
    client_file_path = malloc(pow(2,16));
    strcpy(client_file_path, DIRECTORY);
    strcat(client_file_path, "/");
    strcat(client_file_path, client_file_name);
    client_file = fopen(client_file_path, "w+");
    if (client_file == NULL) {
        errorHandler("Erro ao abrir/criar o arquivo");
    }
    printf("Created file %s\n", client_file_path);
    free(client_message);
    client_message = malloc(MAX(3*8, MSG_SIZE));

    printf("%d\n", MAX(3*8, MSG_SIZE));
    while(
        (client_read_size = recv(client_socket_fd, client_message, 3, 0)) > 0
    ) {
        client_message_unstuffed = messageDestuffing(client_message);
        printf("msg> %s\n", client_message_unstuffed);
        if (strcmp(client_message_unstuffed, "bye") == 0) {
            printf("Finished directory list save service\n");
            break;
        }

        fprintf(client_file, "%s", client_message_unstuffed);
    }

    closeSockets(client_socket_fd, server_socket_fd);
    fclose(client_file);
}