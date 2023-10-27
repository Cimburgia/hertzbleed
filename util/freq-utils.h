#ifndef _FREQ_UTILS_H_
#define _FREQ_UTILS_H_


#include <stdint.h>

#ifdef ARCH_X86
#include "./x86/freq-utils-x86.h"
#endif

#ifdef ARCH_UNIVERSAL
#include "./common/freq-utils-common.h"
#endif

#ifdef ARCH_ARM64
#include "./apple/freq-utils-m.h"
#endif

void freq_init(int core_ID);
uint32_t get_frequency(int core_ID);

#endif  // _FREQ_UTILS_H_