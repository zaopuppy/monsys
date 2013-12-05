//
//  zlog.c
//  base
//
//  Created by Zhao yi on 2/7/13.
//  Copyright (c) 2013 222. All rights reserved.
//

#include "zlog.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>

char printable_char(char c)
{
  static const char printable_char_map[256] = {
    '.', '.', '.', '.', '.', '.', '.', '.',     /*   8 */
    '.', '.', '.', '.', '.', '.', '.', '.',     /*  16 */
    '.', '.', '.', '.', '.', '.', '.', '.',     /*  24 */
    '.', '.', '.', '.', '.', '.', '.', '.',     /*  32 */

    ' ', '!', '\"', '#', '$', '%', '&', '\'',   /*  40 */
    '(', ')', '*', '+', ',', '-', '.', '/',     /*  48 */
    '0', '1', '2', '3', '4', '5', '6', '7',     /*  56 */
    '8', '9', ':', ';', '<', '=', '>', '?',     /*  64 */

    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',     /*  72 */
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',     /*  80 */
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',     /*  88 */
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',    /*  96 */

    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',     /* 104 */
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',     /* 112 */
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w',     /* 120 */
    'x', 'y', 'z', '{', '|', '}', '~', '.',     /* 128 */

    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',

    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',

    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',

    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
    '.', '.', '.', '.', '.', '.', '.', '.',
  };

  return printable_char_map[(uint8_t)c];
}

char hex2char(unsigned char h)
{
  static const char hex_char_map[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
  };

  return hex_char_map[h & 0xF];
}

void trace_bin(const char* data, uint32_t len)
{
  printf("hex dump(%u)\n", len);
  
  static const uint32_t MAX_DATA_LEN = 0xFFFFFE;
  // 000000  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ................'\n\0'
  static const uint32_t LINE_BUF_LEN = 6 + 2 + (16*3 + 1) + 16 + 2;

  if (len > MAX_DATA_LEN) {
    len = MAX_DATA_LEN;
  }

  char line_buf[LINE_BUF_LEN];

  // full size line count. 0 line for 5 bytes, 1 line for 17 bytes,
  // 2 line for 32 bytes, etc.
  // const uint32_t line_count = (len & 0xF) ? ((len >> 4) + 1) : (len >> 4);
  const uint32_t full_line_count = len >> 4;

  uint32_t offset;
  uint32_t i;
  for (i = 0; i < full_line_count; ++i) {
    offset = (i << 4);
    snprintf(line_buf, sizeof(line_buf),
             "%06X  "
             "%02X %02X %02X %02X %02X %02X %02X %02X "
             "%02X %02X %02X %02X %02X %02X %02X %02X  "
             "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
             i,
             (uint8_t)(data[offset +  0]), (uint8_t)(data[offset +  1]),
             (uint8_t)(data[offset +  2]), (uint8_t)(data[offset +  3]),
             (uint8_t)(data[offset +  4]), (uint8_t)(data[offset +  5]),
             (uint8_t)(data[offset +  6]), (uint8_t)(data[offset +  7]),
             (uint8_t)(data[offset +  8]), (uint8_t)(data[offset +  9]),
             (uint8_t)(data[offset + 10]), (uint8_t)(data[offset + 11]),
             (uint8_t)(data[offset + 12]), (uint8_t)(data[offset + 13]),
             (uint8_t)(data[offset + 14]), (uint8_t)(data[offset + 15]),
             //
             printable_char(data[offset +  0]), printable_char(data[offset +  1]),
             printable_char(data[offset +  2]), printable_char(data[offset +  3]),
             printable_char(data[offset +  4]), printable_char(data[offset +  5]),
             printable_char(data[offset +  6]), printable_char(data[offset +  7]),
             printable_char(data[offset +  8]), printable_char(data[offset +  9]),
             printable_char(data[offset + 10]), printable_char(data[offset + 11]),
             printable_char(data[offset + 12]), printable_char(data[offset + 13]),
             printable_char(data[offset + 14]), printable_char(data[offset + 15]));
    printf("%s", line_buf);
  }

  // bytes in last line
  const uint32_t left_bytes = len & 0x0F;
  static const uint32_t bin_offset = 6 + 2;
  static const uint32_t asc_offset = (6 + 2) + (16*3 + 1);

  // the last line

  if (left_bytes > 0) {
    // set buffer to blank first
    for (i = 0; i < sizeof(line_buf); ++i) {
      line_buf[i] = ' ';
    }

    offset = full_line_count * 16;
    snprintf(line_buf, sizeof(line_buf), "%06X  ", full_line_count);
    for (i = 0; i < left_bytes; ++i) {
      line_buf[bin_offset + ((i << 1) + i)    ] =
        hex2char(((uint8_t)(data[offset + i]) >> 4) & 0xF);
      line_buf[bin_offset + ((i << 1) + i) + 1] =
        hex2char((uint8_t)(data[offset + i]) & 0xF);

      line_buf[asc_offset + i] = printable_char(data[offset + i]);
    }
    line_buf[sizeof(line_buf) - 1] = 0x00; // end of string
    printf("%s\n", line_buf);
  }
}

// void zlog(const char *log_level, const char *format, ...)
void zlog(const char *format, ...)
{
  va_list args;
  va_start(args, format);

  // timestamp
  struct tm tm_v;
  time_t clock = time(NULL);
  gmtime_r(&clock, &tm_v);

  char format_buf[1024];

  size_t offset = strftime(format_buf, sizeof(format_buf), "%Y-%m-%d_%H:%M:%S", &tm_v);
  assert(offset <= sizeof(format_buf));

  // remove ending '\0'
  // offset -= 1;

  snprintf(format_buf + offset, sizeof(format_buf) - offset, "|%s", format);

  // vprintf(format, args);
  vprintf(format_buf, args);

  va_end(args);
}


