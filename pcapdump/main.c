#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "pcapdump.h"
#include "hexdump.h"

#define REG __attribute__((section(".xversion"), used, aligned(1)))

#ifndef APPNAME
#define APPNAME "pcapdump"
#endif
#define VERSION "1.0.0"

static struct option s_options[] =
{
    {"read",        no_argument,        NULL,   'r'},
    {"write",       no_argument,        NULL,   'w'},
    {"help",        no_argument,        NULL,   'h'},
    {"version",     no_argument,        NULL,   'v'},
    {NULL,          0,                  NULL,    0 },
};

static const char *s_optstr = ":hvwr";

static void print_usage(void)
{
    fprintf(stderr, "Usage: "APPNAME" <options> <file>          \n"
           "  Options:                                          \n"
           "    -r, --read          Read pcaps from file        \n"
           "    -w, --write         Write pcaps to file         \n"
           "                                                    \n"
           "    -v, --version       Print version details       \n"
           "    -h, --help          Show this help message      \n"
           "                                                    \n"
           "  Example:                                          \n"
           "    - "APPNAME" -r test.pcap                        \n"
           "    - "APPNAME" -w test.pcap                        \n"
           "                                                    \n"
    );
}

static void print_version(void)
{
    fprintf(stderr, APPNAME ": v" VERSION "                     \n"
                    "Copyright (C) 2023 Skippor.                \n"
    );
}

struct param_st {
    const char * input;
    int mode;
};

static struct param_st s_params;

static int parse_options(int argc, char** argv, struct param_st *params)
{
    int opt;
    while((opt = getopt_long(argc, argv, s_optstr, s_options, NULL)) != -1)
    {
        switch (opt) {
            case 'r':
                params->mode = 0;
                break;
            case 'w':
                params->mode = 1;
                break;
            case 'v':
                print_version();
                exit(0);
            case 'h':
                print_usage();
                exit(0);
            case '?':
            case ':':
            default:
                return -1;
        }
    }

    /* the last argument is input file */
    if (optind >= argc) {
        return -1;
    }
    params->input = argv[optind];

    return 0;
}

int main(int argc, char** argv)
{
    struct param_st *params = &s_params;
    if (argc <= 1 || parse_options(argc, argv, params)) {
        print_usage();
        exit(1);
    }

    pcap_t *pcap = pcap_open(params->input);
    if (!pcap) {
        exit(1);
    }

    if (pcap_read(pcap, NULL)) {
        pcap_close(pcap);
        exit(1);
    }

    pcap_print(pcap);

    pcap_close(pcap);
    pcap = NULL;

    return 0;
}

static char _author[] REG = "author : skippor";
static char _license[] REG = "license : GPL";