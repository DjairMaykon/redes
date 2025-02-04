// dns_tools.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "dns_tools.h"

const char* rcode_to_str(int rcode) {
    switch(rcode) {
        case 0: return "No error";
        case 1: return "Format error (name server could not interpret your request)";
        case 2: return "Server failure";
        case 3: return "Name Error (Domain does not exist)";
        case 4: return "Not implemented (name server does not support your request type)";
        case 5: return "Refused (name server refused your request for policy reasons)";
        default: return "WARNING: Unknown rcode";
    }
}

const char* qtype_to_str(int qtype) {
    switch(qtype) {
        case 1: return "A";
        case 2: return "NS";
        case 5: return "CNAME";
        case 15: return "MX";
        case 28: return "AAAA";
        default: {
            static char buf[50];
            snprintf(buf, sizeof(buf), "WARNING: Record type %d not decoded", qtype);
            return buf;
        }
    }
}

const char* class_to_str(int qclass) {
    if (qclass == 1) return "IN";
    static char buf[50];
    snprintf(buf, sizeof(buf), "WARNING: Class %d not decoded", qclass);
    return buf;
}

int decode_name(const unsigned char* raw_bytes, int offset, char* output, int max_len) {
    int current_pos = offset;
    int out_pos = 0;
    int jumped = 0;
    int jump_count = 0;
    int next_pos = offset;
    
    while (1) {
        if (jump_count > 20) {
            return -1;
        }
        
        unsigned char length = raw_bytes[current_pos];
        if (length == 0) {
            if (!jumped) {
                next_pos = current_pos + 1;
            }
            break;
        }
        
        if ((length & 0xC0) == 0xC0) {
            if (!jumped) {
                next_pos = current_pos + 2;
                jumped = 1;
            }
            current_pos = ((length & 0x3F) << 8) | raw_bytes[current_pos + 1];
            jump_count++;
            continue;
        }
        
        current_pos++;
        
        if (out_pos > 0 && out_pos < max_len - 1) {
            output[out_pos++] = '.';
        }
        
        for (int i = 0; i < length && out_pos < max_len - 1; i++) {
            output[out_pos++] = raw_bytes[current_pos++];
        }
        
        if (!jumped) {
            next_pos = current_pos;
        }
    }
    
    output[out_pos] = '\0';
    return next_pos;
}

void decode_dns(const unsigned char* raw_bytes, int length) {
    if (length < 12) {
        printf("Error: Message too short\n");
        return;
    }
    
    printf("Server Response\n");
    printf("---------------\n");
    
    uint16_t id = ntohs(*(uint16_t*)&raw_bytes[0]);
    uint16_t flags = ntohs(*(uint16_t*)&raw_bytes[2]);
    uint16_t qdcount = ntohs(*(uint16_t*)&raw_bytes[4]);
    uint16_t ancount = ntohs(*(uint16_t*)&raw_bytes[6]);
    uint16_t nscount = ntohs(*(uint16_t*)&raw_bytes[8]);
    uint16_t arcount = ntohs(*(uint16_t*)&raw_bytes[10]);
    
    int rcode = flags & 0xF;
    
    printf("Message ID: %d\n", id);
    printf("Response code: %s\n", rcode_to_str(rcode));
    printf("Counts: Query %d, Answer %d, Authority %d, Additional %d\n", qdcount, ancount, nscount, arcount);
    
    int offset = 12;
    char name_buffer[256];
    
    for (int i = 0; i < qdcount; i++) {
        offset = decode_name(raw_bytes, offset, name_buffer, sizeof(name_buffer));
        if (offset < 0 || offset + 4 > length) {
            printf("Error decoding name\n");
            return;
        }
        
        uint16_t qtype = ntohs(*(uint16_t*)&raw_bytes[offset]);
        uint16_t qclass = ntohs(*(uint16_t*)&raw_bytes[offset + 2]);
        
        printf("Question %d:\n", i + 1);
        printf("  Name: %s\n", name_buffer);
        printf("  Type: %s\n", qtype_to_str(qtype));
        printf("  Class: %s\n", class_to_str(qclass));
        
        offset += 4;
    }
    
    for (int i = 0; i < ancount; i++) {
        if (offset + 10 > length) {
            printf("Error: Message truncated\n");
            return;
        }
        
        offset = decode_name(raw_bytes, offset, name_buffer, sizeof(name_buffer));
        if (offset < 0) {
            printf("Error decoding name\n");
            return;
        }
        
        uint16_t atype = ntohs(*(uint16_t*)&raw_bytes[offset]);
        uint16_t aclass = ntohs(*(uint16_t*)&raw_bytes[offset + 2]);
        uint32_t attl = ntohl(*(uint32_t*)&raw_bytes[offset + 4]);
        uint16_t ardlength = ntohs(*(uint16_t*)&raw_bytes[offset + 8]);
        
        printf("Answer %d:\n", i + 1);
        printf("  Name: %s\n", name_buffer);
        printf("  Type: %s, Class: %s, TTL: %u\n",
               qtype_to_str(atype), class_to_str(aclass), attl);
        printf("  RDLength: %d bytes\n", ardlength);
        
        offset += 10;
        
        if (offset + ardlength > length) {
            printf("Error: Message truncated\n");
            return;
        }
        
        char ip_buffer[INET6_ADDRSTRLEN];
        
        switch(atype) {
            case 1:
                if (ardlength == 4) {
                    inet_ntop(AF_INET, &raw_bytes[offset], ip_buffer, sizeof(ip_buffer));
                    printf("  Addr: %s (IPv4)\n", ip_buffer);
                }
                offset += ardlength;
                break;
                
            case 28:
                if (ardlength == 16) {
                    inet_ntop(AF_INET6, &raw_bytes[offset], ip_buffer, sizeof(ip_buffer));
                    printf("  Addr: %s (IPv6)\n", ip_buffer);
                }
                offset += ardlength;
                break;
                
            case 5:
                decode_name(raw_bytes, offset, name_buffer, sizeof(name_buffer));
                printf("  CNAME: %s\n", name_buffer);
                offset += ardlength;
                break;
                
            default:
                printf("  Data: Unsupported record type %d\n", atype);
                offset += ardlength;
        }
    }
}

void encode_domain_name(const char* domain, unsigned char* buffer, int* offset) {
    const char* start = domain;
    const char* end = domain;
    
    while (*end) {
        if (*end == '.') {
            int len = end - start;
            buffer[*offset] = len;
            (*offset)++;
            memcpy(buffer + *offset, start, len);
            *offset += len;
            start = end + 1;
        }
        end++;
    }
    
    int len = end - start;
    if (len > 0) {
        buffer[*offset] = len;
        (*offset)++;
        memcpy(buffer + *offset, start, len);
        *offset += len;
    }
    
    buffer[*offset] = 0;
    (*offset)++;
}