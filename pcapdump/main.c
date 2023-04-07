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

#define MODE_DECODE    0
#define MODE_ENCODE    1

static struct option s_options[] =
{
    {"base64",      no_argument,        NULL,   'b'},
    {"help",        no_argument,        NULL,   'h'},
    {"version",     no_argument,        NULL,   'v'},
    {NULL,          0,                  NULL,    0 },
};

static const char *s_optstr = ":hvb";

static void print_usage(void)
{
    fprintf(stderr, "Usage: "APPNAME" <options> <file>          \n"
           "  Options:                                          \n"
           "    -b, --base64         File encoded with base64   \n"
           "                                                    \n"
           "    -v, --version        Print version details      \n"
           "    -h, --help           Show this help message     \n"
           "                                                    \n"
           "  Example:                                          \n"
           "    - "APPNAME" envelop.bin                         \n"
           "    - "APPNAME" -b envelop.evp                      \n"
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
    int base64;
};

static struct param_st s_params;

static int parse_options(int argc, char** argv, struct param_st *params)
{
    int opt;
    while((opt = getopt_long(argc, argv, s_optstr, s_options, NULL)) != -1)
    {
        switch (opt) {
            case 'b':
                params->base64 = 1;
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


    return 0;
}

static char _author[] REG = "author : skippor";
static char _license[] REG = "license : GPL";