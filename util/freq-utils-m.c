#include "freq-utils-m.h"

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

void sample(unit_data *unit_data, int time_ns) {
    long time_between_measurements = time_ns * 1000000L;
    CFDictionaryRef cpusamp_a  = IOReportCreateSamples(unit_data->cpu_sub, unit_data->cpu_sub_chann, NULL);
    nanosleep((const struct timespec[]){{0, time_between_measurements}}, NULL);
    CFDictionaryRef cpusamp_b  = IOReportCreateSamples(unit_data->cpu_sub, unit_data->cpu_sub_chann, NULL);
  
    CFDictionaryRef cpu_delta  = IOReportCreateSamplesDelta(cpusamp_a, cpusamp_b, NULL);
    
    // Done with these
    CFRelease(cpusamp_a);
    CFRelease(cpusamp_b);
    IOReportIterate(cpu_delta, ^int(IOReportSampleRef sample) {
        for (int i = 0; i < IOReportStateGetCount(sample); i++) {
            CFShow(sample);
        }
        return kIOReportIterOk;
    });
     CFRelease(cpu_delta);
}

// uint64_t* get_state_res(int core_id, int time){

// }

// uint64_t get_frequency(int core_id){

// }

int main(int argc, char* argv[]) {
    struct unit_data *unit = malloc(sizeof(unit_data));

    // initialize the cmd_data
    init_unit_data(unit);
    sample(unit, 10);
}