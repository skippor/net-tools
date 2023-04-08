#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

#include "pcapdump.h"

#define MAX_PACKET_LEN		65536

/* pcap头部信息 */
typedef struct pcap_info_st {
	uint32_t magic;				/* 主标识:a1b2c3d4 */
	uint16_t version_major;		/* 主版本号 */
	uint16_t version_minor;		/* 次版本号 */
	uint32_t thiszone;			/* 区域时间0 */
	uint32_t sigfigs;			/* 时间戳0 */
	uint32_t snaplen;			/* 数据包最大长度 */
	uint32_t linktype;			/* 链路层类型 */
} pcap_info_t;

/* pcap每包头部 */
typedef struct packet_head_st {
	uint32_t gmt_sec;			/* 时间戳，秒部分 */
	uint32_t gmt_msec;			/* 时间戳，微秒部分 */
	uint32_t caplen;			/* 被抓取部分的长度 */
	uint32_t len;				/* 数据包原长度 */
} packet_head_st;

#define NIPQUAD(addr) \
	((const unsigned char *)&addr)[0], \
	((const unsigned char *)&addr)[1], \
	((const unsigned char *)&addr)[2], \
	((const unsigned char *)&addr)[3]

typedef struct packet_st {
	uint32_t 	size;
	uint8_t 	data[0];
} packet_t;

struct pcap_st {
	FILE 		*fp;
	uint32_t	count;
	pcap_info_t	info;
	packet_t	**pkts;
};

pcap_t* pcap_open(const char *pcapfile)
{
	pcap_t *pcap = (pcap_t *)malloc(sizeof(pcap_t));
	pcap->fp = fopen(pcapfile, "rb");
	if (!pcap->fp) {
		fprintf(stderr, "File is not exist: %s\n", pcapfile);
		return NULL;
	}

	if (fread(&pcap->info, sizeof(pcap->info), 1, pcap->fp) != 1) {
		fprintf(stderr, "Failed to read pcap head: %s\n", pcapfile);
		goto err;
	}

	if (pcap->info.magic != 0xa1b2c3d4) {
		fprintf(stderr, "Invalid pcap file magic: %s\n", pcapfile);
		goto err;
	}

	if (pcap->info.linktype != 1) {
		fprintf(stderr, "Unsupport pcap linktype(%u): %s\n", pcap->info.linktype, pcapfile);
		goto err;
	}

	return pcap;

err:
	fclose(pcap->fp);
	free(pcap);
	return NULL;
}

int pcap_read(pcap_t* pcap, const char *filter)
{
	packet_head_st head;
	while (fread(&head, sizeof(head), 1, pcap->fp) == 1) {
		if (head.caplen > pcap->info.snaplen || head.caplen > MAX_PACKET_LEN) {
			fprintf(stderr, "Packet %d: Invalid packet head(caplen: %u > snaplen: %u)\n",
				pcap->count, head.caplen, pcap->info.snaplen);
			return 1;
		}

		pcap->pkts = realloc(pcap->pkts, (pcap->count + 1) * sizeof(packet_t *));
		packet_t *packet = malloc(sizeof(packet_t) + head.caplen);
		packet->size = head.caplen;

		if (fread(packet->data, 1, head.caplen, pcap->fp) != head.caplen) {
			fprintf(stderr, "Packet %d: Read packet data failed\n", pcap->count);
			free(packet);
			return 1;
		}

		pcap->pkts[pcap->count] = packet;
		++ pcap->count;
	}

	return 0;
}

int pcap_write(pcap_t* pcap, const char *pcapfile)
{
	return 0;
}

void pcap_close(pcap_t* pcap)
{
	int i;
	if (!pcap) {
		return;
	}

	/* 释放数据包 */
	for (i = 0; i < pcap->count; ++i) {
		free(pcap->pkts[i]);
		pcap->pkts[i] = NULL;
	}
	free(pcap->pkts);
	pcap->count = 0;

	fclose(pcap->fp);
	free(pcap);
}

int pcap_print(pcap_t* pcap)
{
	int i;
	printf("packet count: %d\n", pcap->count);
	for (i = 0; i < pcap->count; ++i) {
		char *curr = pcap->pkts[i]->data;
		struct ethhdr *l2hdr = (struct ethhdr *)curr;
		curr += sizeof(struct ethhdr);

		/* 只处理ipv4的包 */
		if (l2hdr->h_proto != htons(0x800)) {
			printf("%d    not ipv4(%x), continue\n", i, l2hdr->h_proto);
			continue;
		}

		struct iphdr *iph = (struct iphdr *)curr;
		curr += (iph->ihl * 4);

		/* 不处理分片包 */
		if (iph->frag_off & htons(0x3fff)) {
			printf("%d    %u.%u.%u.%u -> %u.%u.%u.%u FRAGMENT, continue\n",
					i, NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));
			continue;
		}

		struct tcphdr *tcph = NULL;
		struct udphdr *udph = NULL;

		if (iph->protocol == IPPROTO_TCP) {
			tcph = (struct tcphdr *)curr;
			curr += (tcph->doff * 4);
		} else if (iph->protocol == IPPROTO_UDP) {
			udph = (struct udphdr *)curr;
			curr += (sizeof(struct udphdr));
		} else {
			printf("%d    %u.%u.%u.%u -> %u.%u.%u.%u proto:%u, continue\n",
					i, NIPQUAD(iph->saddr), NIPQUAD(iph->daddr), iph->protocol);
			continue;
		}

		const char *udata = curr;
		uint16_t ldata = ntohs(iph->tot_len) - (curr - (char *)iph);

		printf("%d %s, %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u data len: %d\n", i, tcph ? "TCP":"UDP",
			NIPQUAD(iph->saddr), tcph ? ntohs(tcph->source) : ntohs(udph->source),
			NIPQUAD(iph->daddr), tcph ? ntohs(tcph->dest) : ntohs(udph->dest), ldata);
	}

	return 0;
}