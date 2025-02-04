
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#include <stdarg.h>
#include <math.h>

#define MIN(a,b) (((a)<(b))?(a):(b))

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

char* listDirectory(char* directory_path) {
    struct dirent *de;

    DIR *dr = opendir(directory_path);
  
    if (dr == NULL) { 
        errorHandler("Could not open current directory"); 
    } 
  
    char *list = malloc (pow(2,16));
    while ((de = readdir(dr)) != NULL)  {
        if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
            strcat(list, de->d_name);
            strcat(list, ", ");
        }
    }
    closedir(dr);

    return list;
}

char* messageStuffing(char* message){
    char *dest = malloc(10255);
    char *newMsg = malloc(10255);
    strcpy(newMsg, message);
    char *stuf_position_pointer = strstr(newMsg, "bye");
    if (stuf_position_pointer == NULL) return message;
    
    while(stuf_position_pointer != NULL) {
        int position = stuf_position_pointer - newMsg;
        free(dest);
        dest = malloc(10255);
        strncpy(dest, newMsg, position);
        dest[position] = '\0';
        strcat(dest, "byye");
        strncat(dest, newMsg + (position + 3), strlen(newMsg) - (position + 3));
        free(newMsg);
        newMsg = malloc(10255);
        strcpy(newMsg, dest);
        stuf_position_pointer = strstr(newMsg, "bye");
    }

    return dest;
}

int main(int argc, char* argv[]) {
    int server_socket_fd;
    struct sockaddr_in server_addr;
    char *connection_messages, *message, *message_stuffed, *server_answer;

    if (argc != 5) {
        printf("Usage: %s <IP server_addr> <Port> <Message Size> <Directory Path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip_address = argv[1];
    int port = atoi(argv[2]);
    const int MSG_SIZE = atoi(argv[3]);
    char *directory_list = messageStuffing(listDirectory(argv[4]));
    char *directory_path = messageStuffing(argv[4]);

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1) {
        errorHandler("Could not create socket\n");
    }
    printf("Socket created\n");

    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (connect(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        closeSockets(server_socket_fd);
        errorHandler("Connect failed. Error");
    }
    printf("Connected to server\n");

    connection_messages = "READY";
    if (send(server_socket_fd, connection_messages, sizeof(char)*strlen(connection_messages), 0) < 0) {
        closeSockets(server_socket_fd);
        errorHandler("Send failed");
    }

    if (recv(server_socket_fd, server_answer, sizeof(char)*9, 0) < 0) {
        closeSockets(server_socket_fd);
        errorHandler("Receive failed");
    }
    if (strcmp(server_answer, "READY ACK") != 0) {
        closeSockets(server_socket_fd);
        errorHandler("Server Message not known\n");
    }
    printf("Starts directory list save service.\n");

    char *aux = malloc(pow(2,16)); 
    while (strlen(directory_list) > 0) {
        free(aux);
        aux = malloc(pow(2,16));
        free(message);
        message = malloc(MSG_SIZE);
        int size_directory_list = sizeof(char)*strlen(directory_list);
        strncpy(message, directory_list+0, MIN(MSG_SIZE,size_directory_list));
        message[MIN(MSG_SIZE,size_directory_list)] = '\0';
        strncpy(aux, directory_list+MIN(MSG_SIZE,size_directory_list), strlen(directory_list) -1);
        aux[strlen(directory_list) -1] = '\0';
        strcpy(directory_list, aux);
        if (send(server_socket_fd, connection_messages, strlen(connection_messages), 0) < 0) {
            closeSockets(server_socket_fd);
            errorHandler("Send failed");
        }
    }
    printf("Send path to server: %s.\n", connection_messages);
    
    char *aux = malloc(pow(2,16)); 
    while (strlen(directory_list) > 0) {
        free(aux);
        aux = malloc(pow(2,16));
        free(message);
        message = malloc(MSG_SIZE);
        int size_directory_list = sizeof(char)*strlen(directory_list);
        strncpy(message, directory_list+0, MIN(MSG_SIZE,size_directory_list));
        message[MIN(MSG_SIZE,size_directory_list)] = '\0';
        strncpy(aux, directory_list+MIN(MSG_SIZE,size_directory_list), strlen(directory_list) -1);
        aux[strlen(directory_list) -1] = '\0';
        strcpy(directory_list, aux);

        // if (send(server_socket_fd, message, MSG_SIZE, 0) < 0) {
        //     closeSockets(server_socket_fd);
        //     errorHandler("Send failed");
        // }
        // printf("Send to server: %s\n", message);
    }
    connection_messages = "bye";
    if (send(server_socket_fd, connection_messages, sizeof(connection_messages), 0) < 0) {
        closeSockets(server_socket_fd);
        errorHandler("Send failed");
    }
    printf("Finished directory list save service\n");

    closeSockets(server_socket_fd);
}