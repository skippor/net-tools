#ifndef __XHEXDUMP_H_20230328__
#define __XHEXDUMP_H_20230328__

#include <stdio.h>
#include <stdint.h>

#ifndef BYTES_PER_LINE
#define BYTES_PER_LINE  (16)
#endif

static inline char to_char(uint8_t value)
{
	return (32 <= value && value < 127) ? (char)value : '.';
}

static inline uint16_t to_index(int index, uint8_t base)
{
    return base ? (index / base) * base : index;
}

/**
  * @brief hexdump bytes to string
  * @param input    [number] input byte buffer
  * @param size     [number] input size
  * @param length   [number] bytes per line
  * @param title    [number] title to show
  * @return void
  */
static inline void hexdump(uint8_t *input, int size, int upper)
{
    static const char * s_lowercase = "0123456789abcdef";
    static const char * s_uppercase = "0123456789ABCDEF";
    const char * letters = upper ? s_uppercase : s_lowercase;

    int i = 0, j = 0;
    char hex[BYTES_PER_LINE * 3 + 1];
    char line[BYTES_PER_LINE + 1];
    memset(line, 0, sizeof(line));
    memset(hex, 0, sizeof(hex));

    for(i = 0; i < size; ++i, j = i % BYTES_PER_LINE) {
        if (i > 0 && j == 0) {
            printf("%04x: %-*s |%-*s|\n", to_index(i - 1, BYTES_PER_LINE),
                        3 * BYTES_PER_LINE, hex, BYTES_PER_LINE, line);
            memset(line, 0, sizeof(line));
            memset(hex, 0, sizeof(hex));
        }
        line[j] = to_char(input[i]);
        hex[3 * j] = letters[input[i] >> 4];
        hex[3 * j + 1] = letters[input[i] & 0xF];
        hex[3 * j + 2] = ' ';
    }

    printf("%04x: %-*s |%-*s|\n", to_index(i - 1, BYTES_PER_LINE),
                3 * BYTES_PER_LINE, hex, BYTES_PER_LINE, line);
}

#define HEXDUMP(input, size) do {                       \
    printf("hexdump: %s, size: %d\n", #input, size);    \
    hexdump(input, size, 0);                            \
} while(0)

#endif