#include "freq-utils.h"


void freq_init(int core_ID) {
    #ifdef ARCH_X86
    set_frequency_units(core_ID);
    #endif
}

uint32_t get_frequency(int core_ID) {
    #ifdef ARCH_X86
    return get_frequency_rapl(core_ID);
    #endif

    #ifdef ARCH_UNIVERSAL

    return get_frequency_common(core_ID);
    #endif
}

