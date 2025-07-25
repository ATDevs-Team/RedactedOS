#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "net/network_types.h"

typedef struct __attribute__((packed)) udp_hdr_t {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} udp_hdr_t;

void create_udp_packet(uintptr_t p, network_connection_ctx source, network_connection_ctx destination, sizedptr payload);
uint16_t udp_parse_packet(uintptr_t ptr);
sizedptr udp_parse_packet_payload(uintptr_t ptr);

#ifdef __cplusplus
}
#endif