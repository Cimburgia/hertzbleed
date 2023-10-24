#ifndef _POWER_UTILS_H_
#define _POWER_UTILS_H_

#ifdef ARCH_X86
#include "./x86/rapl-utils.h"
#endif

#ifdef ARCH_UNIVERSAL
#include "./common/power-utils-common.h"
#endif


void power_init(int core_ID);
double get_pkg_energy(int core_ID);

#endif  //_POWER_UTILS_H_