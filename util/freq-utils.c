#include "freq-utils.h"


void freq_init(int core_ID) {
    #ifdef ARCH_X86
    set_frequency_units(core_ID);
    #endif

    #ifdef ARCH_ARM64_M
    init_unit_data();
    #endif
}

uint32_t get_frequency(int core_ID) {
    #ifdef ARCH_X86
    return get_frequency_rapl(core_ID);
    #endif

    #ifdef ARCH_UNIVERSAL
    return get_frequency_common(core_ID);
    #endif

    #ifdef ARCH_ARM64_M
    sample_deltas *deltas = sample(0);
    float freq = get_frequency_apple(deltas->cpu_delta, core_ID);
    CFRelease(deltas->cpu_delta);
    CFRelease(deltas->pwr_delta);
    free(deltas);    
    return freq;
    #endif
}

