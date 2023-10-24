#include "power-utils.h"

void power_init(int core_ID) {
    #ifdef ARCH_X86
    set_rapl_units(core_ID);
    #endif
}

double get_pkg_energy(int core_ID) {
    #ifdef ARCH_X86
    return rapl_msr(core_ID, PKG_ENERGY);
    #endif

    #ifdef ARCH_UNIVERSAL
    return get_power_common(core_ID);
    #endif
}