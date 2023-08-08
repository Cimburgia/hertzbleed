#include "freq-utils-m.h"

/**
 * Initialize the channel subscriptions so we can continue to sample over them throughout the
 * experiments. 
*/
void init_unit_data(unit_data *data){
    //Initialize channels
    data->cpu_chann = IOReportCopyChannelsInGroup(CFSTR("CPU Stats"), 0, 0, 0, 0);
    
    // Create subscription
    data->soc_samples.cpu_sub  = IOReportCreateSubscription(NULL, data->cpu_chann, &data->cpu_sub_chann, 0, 0);
    
    CFRelease(data->cpu_chann);
}

void sample(int time_ns) {
    CFArrayRef complex_freq_chankeys = CFArrayCreate(kCFAllocatorDefault, (const void *[]){CFSTR("ECPU"), CFSTR("PCPU"), CFSTR("GPUPH")}, 3, &kCFTypeArrayCallBacks);
    CFArrayRef core_freq_chankeys = CFArrayCreate(kCFAllocatorDefault, (const void *[]){CFSTR("ECPU"), CFSTR("PCPU")}, 2, &kCFTypeArrayCallBacks);
    
    CFDictionaryRef cpusamp_a  = IOReportCreateSamples(unit_data->soc_samples.cpu_sub, unit_data->soc_samples.cpu_sub_chann, NULL);
    
    CFDictionaryRef cpusamp_b  = IOReportCreateSamples(unit_data->soc_samples.cpu_sub, unit_data->soc_samples.cpu_sub_chann, NULL);
  
    CFDictionaryRef cpu_delta  = IOReportCreateSamplesDelta(cpusamp_a, cpusamp_b, NULL);
    
    // Done with these
    CFRelease(cpusamp_a);
    CFRelease(cpusamp_b);

}

uint64_t* get_state_res(int core_id, int time){

}

uint64_t get_frequency(int core_id){

}

int main(int argc, char* argv[]) {
    unit_data* unit = malloc(sizeof(unit_data));

    // initialize the cmd_data
    init_unit_data(unit);
}