#ifndef _FREQ_UTILS_M_H
#define _FREQ_UTILS_M_H

#include <sys/sysctl.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

enum {
    kIOReportIterOk,
    kIOReportIterFailed,
    kIOReportIterSkipped
};

typedef struct IOReportSubscriptionRef* IOReportSubscriptionRef;
typedef CFDictionaryRef IOReportSampleRef;

typedef struct unit_data{
    IOReportSubscriptionRef cpu_sub;
    CFMutableDictionaryRef cpu_sub_chann;
    CFMutableDictionaryRef cpu_chann;
} unit_data;

extern IOReportSubscriptionRef IOReportCreateSubscription(void* a, CFMutableDictionaryRef desiredChannels, CFMutableDictionaryRef* subbedChannels, uint64_t channel_id, CFTypeRef b);
extern CFDictionaryRef IOReportCreateSamples(IOReportSubscriptionRef iorsub, CFMutableDictionaryRef subbedChannels, CFTypeRef a);
extern CFDictionaryRef IOReportCreateSamplesDelta(CFDictionaryRef prev, CFDictionaryRef current, CFTypeRef a);
extern CFMutableDictionaryRef IOReportCopyChannelsInGroup(CFStringRef, CFStringRef, uint64_t, uint64_t, uint64_t);
typedef int (^ioreportiterateblock)(IOReportSampleRef ch);
extern void IOReportIterate(CFDictionaryRef samples, ioreportiterateblock);
extern int IOReportStateGetCount(CFDictionaryRef);

void init_unit_data(unit_data *data);
void sample(unit_data *data, int time);
// uint64_t* get_state_res(int core_id, int time);
// uint64_t get_frequency(int core_id);
#endif