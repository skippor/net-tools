#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#include "pktutils.h"

const char* proto_name(uint8_t protocol)
{
    switch (protocol) {
        case IPPROTO_TCP:   return "TCP";
        case IPPROTO_UDP:   return "UDP";
        case IPPROTO_ESP:   return "ESP";
        case IPPROTO_ICMP:  return "ICMP";
    }

    return NULL;
}

int push_udp_hdr(struct udphdr *udph, uint16_t sport, uint16_t dport, uint16_t len)
{
    udph->source = htons(sport);
    udph->dest = htons(dport);
    udph->len = htons(len);
    udph->check = 0;
}

int push_ipv4_hdr(struct iphdr *iph, uint32_t saddr, uint32_t daddr, uint8_t proto,
                    uint16_t len, uint8_t df, int16_t id, uint8_t tos)
{
#define DONT_FRAG 0x1fff
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = tos;
    iph->tot_len = htons(len);
    iph->frag_off = df ? htons(DONT_FRAG) : 0;
    iph->ttl = 64;
    iph->id = id;
    iph->protocol = proto;
    iph->check = 0;
    iph->daddr = daddr;
    iph->saddr = saddr;

    return 0;
}

int push_eth_hdr(struct ethhdr *ehdr, const uint8_t smac[6], const uint8_t dmac[6])
{
    memcpy(ehdr->h_source, smac, 6);
    memcpy(ehdr->h_dest, dmac, 6);
    ehdr->h_proto = htons(ETH_P_IP);

    return 0;
}
