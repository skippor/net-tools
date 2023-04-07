#ifndef __XPCAP_INFO_H_20230319__
#define __XPCAP_INFO_H_20230319__

#include <stdint.h>

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

#endif