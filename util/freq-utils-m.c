#include "freq-utils-m.h"

static char *chann_array[] = {"ECPU","PCPU","ECPU0","ECPU1","ECPU2","ECPU3","PCPU0","PCPU1","PCPU2","PCPU3"};
static char *idx_array_ecpu[] = {"IDLE","V0P6","V1P5","V2P4","V3P3","V4P2","V5P1","V6P0"};
static char *idx_array_pcpu[] = {"IDLE", "V0P16", "V1P15","V2P14","V3P13","V4P12","V5P11","V6P10","V7P9","V8P8","V9P7","V10P6","V11P5","V12P4","V13P3","V14P2","V15P1","V16P0"};
/**
 * Initialize the channel subscriptions so we can continue to sample over them throughout the
 * experiments. 
*/
void init_unit_data(unit_data *data){
    //Initialize channels
    data->cpu_chann = IOReportCopyChannelsInGroup(CFSTR("CPU Stats"), 0, 0, 0, 0);
    // Create subscription
    data->cpu_sub  = IOReportCreateSubscription(NULL, data->cpu_chann, &data->cpu_sub_chann, 0, 0);
    CFRelease(data->cpu_chann);
}

CFDictionaryRef sample(unit_data *unit_data, int time_ns) {
    long time_between_measurements = time_ns * 1000000L;
    CFDictionaryRef cpusamp_a  = IOReportCreateSamples(unit_data->cpu_sub, unit_data->cpu_sub_chann, NULL);
    nanosleep((const struct timespec[]){{0, time_between_measurements}}, NULL);
    CFDictionaryRef cpusamp_b  = IOReportCreateSamples(unit_data->cpu_sub, unit_data->cpu_sub_chann, NULL);
  
    CFDictionaryRef cpu_delta  = IOReportCreateSamplesDelta(cpusamp_a, cpusamp_b, NULL);
    
    // Done with these
    CFRelease(cpusamp_a);
    CFRelease(cpusamp_b);
    return cpu_delta;
}

/*
 * Takes a sample and a core id and returns a histogram of residency values
 * Keys and values of cores are hard coded for now:
 * 
 * ECPU Complex (0)
 * PCPU Complex (1)
 * ECPU Core 0-3 (2-5)
 * PCPU Core 0-3 (6-7)
*/
uint64_t *get_state_res(CFDictionaryRef cpu_delta, int core_id){
    // Get number of indicies 8 or 18 depending on E vs. P
    int num_idxs = 8;
    if (core_id == 1 || core_id > 5) num_idxs = 18;
    uint64_t *residencies = (uint64_t *)malloc(num_idxs * sizeof(uint64_t));
    CFStringRef core_id_str = CFStringCreateWithCString(NULL, chann_array[core_id], kCFStringEncodingUTF8);
  
    IOReportIterate(cpu_delta, ^int(IOReportSampleRef sample) {
        int ii = 0;
        for (int i = 0; i < IOReportStateGetCount(sample); i++) {
            CFStringRef subgroup    = IOReportChannelGetSubGroup(sample);
            CFStringRef idx_name    = IOReportStateGetNameForIndex(sample, i);
            CFStringRef chann_name  = IOReportChannelGetChannelName(sample);
            uint64_t residency      = IOReportStateGetResidency(sample, i);

            if (CFStringCompare(chann_name, core_id_str, 0) == kCFCompareEqualTo){
                residencies[ii] = residency;
                ii++;
            }
        }
        return kIOReportIterOk;
    });
    return residencies;
}

// uint64_t get_frequency(int core_id){

// }

int main(int argc, char* argv[]) {
    struct unit_data *unit = malloc(sizeof(unit_data));

    // initialize the cmd_data
    init_unit_data(unit);
    CFDictionaryRef s1 = sample(unit, 100);
    uint64_t *residencies = get_state_res(s1, 6);
    for (int i = 0; i < 18; i++){
        printf("%llu\n", residencies[i]);
    }
    free(residencies);
    CFRelease(s1);
}