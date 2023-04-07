#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include "pcapinfo.h"
#include "pcapdump.h"

struct vpkt_st {
	uint32_t 	size;
	uint8_t 	*data;
};

struct pcap_st {
	FILE 			*fp;
	uint32_t 		count;
	pcap_info_st 	pi;
};

pcap_t* pcap_open(const char *pcapfile)
{
	pcap_t *pcap = (pcap_t *)malloc(sizeof(pcap_t));
	pcap->fp = fopen(pcapfile, "rb");
	if (!pcap->fp) {
		fprintf(stderr, "File is not exist: %s\n", pcapfile);
		return NULL;
	}

	if (fread(&pcap->pi, sizeof(pcap->pi), 1, pcap->fp) != 1) {
		fprintf(stderr, "Failed to read pcap head: %s\n", pcapfile);
		goto err;
	}

	if (pcap->pi.magic != 0xa1b2c3d4) {
		fprintf(stderr, "Invalid pcap file magic: %s\n", pcapfile);
		goto err;
	}

	if (pcap->pi.linktype != 1) {
		fprintf(stderr, "Unsupport pcap linktype(%u): %s\n", pcap->pi.linktype, pcapfile);
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
	return 0;
}

int pcap_write(pcap_t* pcap, const char *pcapfile)
{
	return 0;
}

int pcap_print(pcap_t* pcap)
{
	packet_head_st head;
	int pkt_counter;

	for (pkt_counter = 1; fread(&head, sizeof(head), 1, pcap->fp) == 1; ++pkt_counter) {
		if (head.caplen > pcap->pi.snaplen || head.caplen > MAX_PACKET_LEN) {
			fprintf(stderr, "Packet %d: Invalid packet head(caplen: %u > snaplen: %u)\n",
				pkt_counter, head.caplen, pcap->pi.snaplen);
			return 1;
		}

		char data[MAX_PACKET_LEN];
		if (fread(data, 1, head.caplen, pcap->fp) != head.caplen) {
			fprintf(stderr, "Packet %d: Read packet data failed\n", pkt_counter);
			return 1;
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

	return 0;
}

void pcap_close(pcap_t* pcap)
{
	if (pcap) {
		fclose(pcap->fp);
		free(pcap);
	}
}

void pcap_foreach(pcap_t* pcap, int(*callback)(void*, void*), void* userdata)
{

}