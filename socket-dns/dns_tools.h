// dns_tools.h
#ifndef DNS_TOOLS_H
#define DNS_TOOLS_H

#include <stdint.h>

// DNS Header structure
struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

// DNS Question structure
struct dns_question {
    char *qname;
    uint16_t qtype;
    uint16_t qclass;
};

// Function declarations
const char* rcode_to_str(int rcode);
const char* qtype_to_str(int qtype);
const char* class_to_str(int qclass);
int decode_name(const unsigned char* raw_bytes, int offset, char* output, int max_len);
void decode_dns(const unsigned char* raw_bytes, int length);
void encode_domain_name(const char* domain, unsigned char* buffer, int* offset);

#endif