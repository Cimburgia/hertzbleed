#ifndef _FREQ_UTILS_M_H
#define _FREQ_UTILS_M_H

#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

enum {
    kIOReportIterOk,
    kIOReportIterFailed,
    kIOReportIterSkipped,
    kIOReportIterStop
};
typedef struct IOReportSubscriptionRef* IOReportSubscriptionRef;
typedef CFDictionaryRef IOReportSampleRef;

extern IOReportSubscriptionRef IOReportCreateSubscription(void* a, CFMutableDictionaryRef desiredChannels, CFMutableDictionaryRef* subbedChannels, uint64_t channel_id, CFTypeRef b);
extern CFDictionaryRef IOReportCreateSamples(IOReportSubscriptionRef iorsub, CFMutableDictionaryRef subbedChannels, CFTypeRef a);
extern CFDictionaryRef IOReportCreateSamplesDelta(CFDictionaryRef prev, CFDictionaryRef current, CFTypeRef a);

extern CFMutableDictionaryRef IOReportCopyChannelsInGroup(CFStringRef, CFStringRef, uint64_t, uint64_t, uint64_t);

typedef int (^ioreportiterateblock)(IOReportSampleRef ch);
extern void IOReportIterate(CFDictionaryRef samples, ioreportiterateblock);

extern int IOReportStateGetCount(CFDictionaryRef);
extern uint64_t IOReportStateGetResidency(CFDictionaryRef, int);
extern uint64_t IOReportArrayGetValueAtIndex(CFDictionaryRef, int);
extern long IOReportSimpleGetIntegerValue(CFDictionaryRef, int);
extern CFStringRef IOReportChannelGetChannelName(CFDictionaryRef);
extern CFStringRef IOReportChannelGetSubGroup(CFDictionaryRef);
extern CFStringRef IOReportStateGetNameForIndex(CFDictionaryRef, int);
extern CFStringRef IOReportChannelGetGroup(CFDictionaryRef);

extern void IOReportMergeChannels(CFMutableDictionaryRef, CFMutableDictionaryRef, CFTypeRef);

typedef struct unit_data{
    IOReportSubscriptionRef cpu_sub;
    CFMutableDictionaryRef cpu_sub_chann;
    CFMutableDictionaryRef cpu_chann;
    IOReportSubscriptionRef pwr_sub;
    CFMutableDictionaryRef pwr_sub_chann;
    CFMutableDictionaryRef energy_chann;
} unit_data;

typedef struct cpu_freq_data{
    uint64_t *residencies;
    uint64_t frequency;
} cpu_freq_data;

typedef struct sample_deltas{
    CFDictionaryRef cpu_delta;
    CFDictionaryRef pwr_delta;
} sample_deltas;

void init_unit_data();
sample_deltas *sample(int time_ms);
uint64_t *get_state_res(CFDictionaryRef cpu_delta, int core_id);
float get_frequency_apple(CFDictionaryRef cpu_delta, int core_id);
float get_power_apple(CFDictionaryRef pwr_delta, int core_id);
#endif