#ifndef __XPACKET_UTILS_H_20230319__
#define __XPACKET_UTILS_H_20230319__

#include <stdint.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/ip_icmp.h>

const char* proto_name(uint8_t protocal);

int push_udp_hdr(struct udphdr *udph, uint16_t sport, uint16_t dport, uint16_t len);

int push_ipv4_hdr(struct iphdr *iph, uint32_t saddr, uint32_t daddr, uint8_t proto,
                    uint16_t len, uint8_t df, int16_t id, uint8_t tos);

int push_eth_hdr(struct ethhdr *ehdr, const uint8_t smac[6], const uint8_t dmac[6]);

int push_icmp_hdr(struct icmp *icmph, uint8_t type, uint8_t code, uint16_t id, uint16_t seq);

#endif