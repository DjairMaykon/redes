// dns.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include "dns_tools.h"

#define MAX_DNS_SIZE 512

unsigned char* build_dns_message(const char* qname, const char* qtype, int* length) {
    unsigned char* message = malloc(MAX_DNS_SIZE);
    if (!message) {
        return NULL;
    }
    
    srand(time(NULL));
    uint16_t message_id = rand() & 0xFFFF;
    
    struct dns_header header = {
        .id = htons(message_id),
        .flags = htons(0x0100),
        .qdcount = htons(1),
        .ancount = 0,
        .nscount = 0,
        .arcount = 0
    };
    
    memcpy(message, &header, sizeof(header));
    *length = sizeof(header);
    
    encode_domain_name(qname, message, length);
    
    uint16_t qtype_num = (strcmp(qtype, "A") == 0) ? 1 : 28;
    uint16_t qclass = 1;
    
    qtype_num = htons(qtype_num);
    qclass = htons(qclass);
    
    memcpy(message + *length, &qtype_num, sizeof(qtype_num));
    *length += sizeof(qtype_num);
    memcpy(message + *length, &qclass, sizeof(qclass));
    *length += sizeof(qclass);
    
    return message;
}

int main(int argc, char *argv[]) {
    char *qtype = NULL;
    char *qname = NULL;
    char *server_ip = NULL;
    int opt;
    
    while ((opt = getopt(argc, argv, "t:n:s:")) != -1) {
        switch (opt) {
            case 't':
                qtype = optarg;
                break;
            case 'n':
                qname = optarg;
                break;
            case 's':
                server_ip = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -t type -n name -s server\n", argv[0]);
                exit(1);
        }
    }
    
    if (!qtype || !qname || !server_ip) {
        fprintf(stderr, "All arguments are required\n");
        fprintf(stderr, "Usage: %s -t type -n name -s server\n", argv[0]);
        exit(1);
    }
    
    if (strcmp(qtype, "A") != 0 && strcmp(qtype, "AAAA") != 0) {
        fprintf(stderr, "Error: Query Type must be 'A' (IPv4) or 'AAAA' (IPv6)\n");
        exit(1);
    }
    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error setting timeout");
        close(sock);
        exit(1);
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(53);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid server address");
        close(sock);
        exit(1);
    }
    
    int message_length;
    unsigned char* message = build_dns_message(qname, qtype, &message_length);
    if (!message) {
        perror("Failed to build DNS message");
        close(sock);
        exit(1);
    }
    
    if (sendto(sock, message, message_length, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to send DNS query");
        free(message);
        close(sock);
        exit(1);
    }
    
    unsigned char response[MAX_DNS_SIZE];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    
    int received = recvfrom(sock, response, MAX_DNS_SIZE, 0, (struct sockaddr*)&from_addr, &from_len);
    
    if (received < 0) {
        perror("Failed to receive DNS response");
        free(message);
        close(sock);
        exit(1);
    }
    
    free(message);
    close(sock);
    
    decode_dns(response, received);
}