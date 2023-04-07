#ifndef _XPCAP_H_20230319_
#define _XPCAP_H_20230319_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

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
} pcap_info_st;

/* pcap每包头部 */
typedef struct packet_head_st {
	uint32_t gmt_sec;			/* 时间戳，秒部分 */
	uint32_t gmt_msec;			/* 时间戳，微秒部分 */
	uint32_t caplen;			/* 被抓取部分的长度 */
	uint32_t len;				/* 数据包原长度 */
} packet_head_st;

/* 二层头，ethhdr，为了避免引入linux/if_ether.h，这里单独定义 */
typedef struct l2_head_st {
	char dest[6];				/* 目的mac地址 */
	char source[6];				/* 源mac地址 */
	uint16_t proto;				/* 三层协议 */
} l2_head_st;

#define NIPQUAD(addr) \
	((const unsigned char *)&addr)[0], \
	((const unsigned char *)&addr)[1], \
	((const unsigned char *)&addr)[2], \
	((const unsigned char *)&addr)[3]

static void print_packet(int pkti, const l2_head_st *l2h, const struct iphdr *iph,
						 const struct tcphdr *tcph, const struct udphdr *udph,
						 const char *data, uint16_t ldata)
{
	printf("%d--\n%s, %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\nuser data len: %d\n", pkti, tcph ? "TCP":"UDP",
			NIPQUAD(iph->saddr), tcph ? ntohs(tcph->source) : ntohs(udph->source),
			NIPQUAD(iph->daddr), tcph ? ntohs(tcph->dest) : ntohs(udph->dest), ldata);
}

int pcap_read(const char *filename)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "File is not exist: %s\n", filename);
		return -1;
	}

	pcap_info_st pi;
	if (fread(&pi, sizeof(pi), 1, fp) != 1) {
		perror("Read pcap head");
		goto fail;
	}

	if (pi.magic != 0xa1b2c3d4) {
		fprintf(stderr, "Invalid pcap file magic: %s\n", filename);
		goto fail;
	}

	if (pi.linktype != 1) {
		fprintf(stderr, "Unsupport pcap linktype(%u): %s\n", pi.linktype, filename);
		goto fail;
	}

	packet_head_st head;
	int pkt_counter;

	for (pkt_counter = 1; fread(&head, sizeof(head), 1, fp) == 1; ++pkt_counter) {
		if (head.caplen > pi.snaplen || head.caplen > MAX_PACKET_LEN) {
			fprintf(stderr, "Packet %d: Invalid packet head(caplen: %u > snaplen: %u)\n",
				pkt_counter, head.caplen, pi.snaplen);
			goto fail;
		}

		char data[MAX_PACKET_LEN];
		if (fread(data, 1, head.caplen, fp) != head.caplen) {
			fprintf(stderr, "Packet %d: Read packet data failed\n", pkt_counter);
			goto fail;
		}

		char *curr = data;
		l2_head_st *l2hdr = (l2_head_st *)curr;
		curr += sizeof(l2_head_st);

		/* 只处理ipv4的包 */
		if (l2hdr->proto != htons(0x800))
			continue;

		struct iphdr *iph = (struct iphdr *)curr;
		curr += (iph->ihl * 4);

		/* 不处理分片包 */
		if (iph->frag_off & htons(0x3fff))
			continue;

		struct tcphdr *tcph = NULL;
		struct udphdr *udph = NULL;

		if (iph->protocol == IPPROTO_TCP) {
			tcph = (struct tcphdr *)curr;
			curr += (tcph->doff * 4);
		} else if (iph->protocol == IPPROTO_UDP) {
			udph = (struct udphdr *)curr;
			curr += (sizeof(struct udphdr));
		} else {
			continue;
		}

		const char *udata = curr;
		uint16_t ldata = ntohs(iph->tot_len) - (curr - (char *)iph);

		print_packet(pkt_counter, l2hdr, iph, tcph, udph, udata, ldata);
	}

	fclose(fp);
	return 0;
fail:
	fclose(fp);
	return 1;
}

#endif