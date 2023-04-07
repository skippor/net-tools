#ifndef __XPCAP_DUMP_H_20230319__
#define __XPCAP_DUMP_H_20230319__

#include <stdint.h>

typedef struct pcap_st pcap_t;

pcap_t* pcap_open(const char *pcapfile);

int pcap_read(pcap_t* pcap, const char *filter);

int pcap_print(pcap_t* pcap);

int pcap_write(pcap_t* pcap, const char *pcapfile);

void pcap_close(pcap_t* pcap);

#endif