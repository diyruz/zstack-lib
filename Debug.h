#ifndef _DEBUG_H
#define _DEBUG_H

#include "hal_types.h"
#include "hal_uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define BUFFLEN 128

extern halUARTCfg_t halUARTConfig;
extern bool DebugInit(void);
extern void LREP(char *format, ...);
extern void LREPMaster(const char *data);

void vprint(const char *fmt, va_list argp);

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')
#endif