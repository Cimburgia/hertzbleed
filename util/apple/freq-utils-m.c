#include "freq-utils-m.h"

static char *chann_array[] = {"ECPU0","ECPU1","ECPU2","ECPU3","PCPU0","PCPU1","PCPU2","PCPU3","ECPU","PCPU"};
static float e_array[] = {600, 912, 1284, 1752, 2004, 2256, 2424};
static float p_array[] = {660, 924, 1188, 1452, 1704, 1968, 2208, 2400, 2568, 2724, 2868, 2988, 3096, 3204, 3324, 3408, 3504};
static float *freq_state_cores[] = {e_array, p_array};
static unit_data *unit;
static int num_cores;

#define TIME_BETWEEN_MEASUREMENTS 1000000L // 1 millisecond
/**
 * Initialize the channel subscriptions so we can continue to sample over them throughout the
 * experiments. 
*/
void init_unit_data(){
    // Allocate memory and get number of cores
    unit = malloc(sizeof(unit_data));
    num_cores = get_core_num();

    //Initialize channels
    unit->cpu_chann = IOReportCopyChannelsInGroup(CFSTR("CPU Stats"), 0, 0, 0, 0);
    unit->energy_chann = IOReportCopyChannelsInGroup(CFSTR("Energy Model"), 0, 0, 0, 0);

    // Create subscription
    unit->cpu_sub  = IOReportCreateSubscription(NULL, unit->cpu_chann, &unit->cpu_sub_chann, 0, 0);
    unit->pwr_sub  = IOReportCreateSubscription(NULL, unit->energy_chann, &unit->pwr_sub_chann, 0, 0);
    CFRelease(unit->cpu_chann);
    CFRelease(unit->energy_chann);
}

sample_deltas *sample() {
    CFDictionaryRef cpusamp_a  = IOReportCreateSamples(unit->cpu_sub, unit->cpu_sub_chann, NULL);
    CFDictionaryRef pwrsamp_a  = IOReportCreateSamples(unit->pwr_sub, unit->pwr_sub_chann, NULL);
    nanosleep((const struct timespec[]){{0, TIME_BETWEEN_MEASUREMENTS}}, NULL);
    CFDictionaryRef cpusamp_b  = IOReportCreateSamples(unit->cpu_sub, unit->cpu_sub_chann, NULL);
    CFDictionaryRef pwrsamp_b  = IOReportCreateSamples(unit->pwr_sub, unit->pwr_sub_chann, NULL);
  
    CFDictionaryRef cpu_delta  = IOReportCreateSamplesDelta(cpusamp_a, cpusamp_b, NULL);
    CFDictionaryRef pwr_delta  = IOReportCreateSamplesDelta(pwrsamp_a, pwrsamp_b, NULL);

    // Done with these
    CFRelease(cpusamp_a);
    CFRelease(cpusamp_b);
    CFRelease(pwrsamp_a);
    CFRelease(pwrsamp_b);
    
    sample_deltas *deltas = (sample_deltas *) malloc(sizeof(sample_deltas));
    deltas->cpu_delta = cpu_delta;
    deltas->pwr_delta = pwr_delta;
    return deltas;
}

/*
 * This will be updated to now always return all core/complex information
*/
void get_state_residencies(CFDictionaryRef cpu_delta, cpu_freq_data *data){
    __block int i = 0;
    IOReportIterate(cpu_delta, ^int(IOReportSampleRef sample) {
        // Check sample group and only expand if CPU Stats
        CFStringRef group = IOReportChannelGetGroup(sample);
        if (CFStringCompare(group, CFSTR("CPU Stats"), 0) == kCFCompareEqualTo){
            // Get subgroup (core or complex) and chann_name (core/complex id)
            CFStringRef subgroup    = IOReportChannelGetSubGroup(sample);
            CFStringRef chann_name  = IOReportChannelGetChannelName(sample);
            // Only take these
            if (CFStringCompare(subgroup, CFSTR("CPU Core Performance States"), 0) == kCFCompareEqualTo || 
                CFStringCompare(subgroup, CFSTR("CPU Complex Performance States"), 0) == kCFCompareEqualTo){
                // Only save the CPU-specific samples
                if (CFStringFind(chann_name, CFSTR("CPU"), 0).location != kCFNotFound){
                    data->core_labels[i] = chann_name;
                    data->num_dvfs_states[i] = IOReportStateGetCount(sample);
                    data->residencies[i] = malloc(IOReportStateGetCount(sample)*sizeof(uint64_t));
                    // Indicies represent samples for each DVFS state
                    int ii = 0;
                    for (int iii = 0; iii < IOReportStateGetCount(sample); iii++) {
                        CFStringRef idx_name    = IOReportStateGetNameForIndex(sample, iii);
                        uint64_t residency      = IOReportStateGetResidency(sample, iii);
                        if (CFStringFind(idx_name, CFSTR("IDLE"), 0).location != kCFCompareEqualTo){
                            data->residencies[i][ii] = residency;
                            ii++;
                        }
                    }
                    i++;
                }
            }

        }
        return kIOReportIterOk;
    });
}

/*
 * Takes a sample and a core id and returns cpu power
 * Keys and values of cores are hard coded for now:
 * 
 * ECPU Complex (0)
 * PCPU Complex (1)
 * ECPU Core 0-3 (2-5)
 * PCPU Core 0-3 (6-7)
 * All (8)
*/
float get_power_apple(CFDictionaryRef pwr_delta, int core_id){
    __block float pwr = 0;
    // Get number of indicies 8 or 18 depending on E vs. P
    IOReportIterate(pwr_delta, ^int(IOReportSampleRef sample) {
        CFStringRef core_id_str = CFStringCreateWithCString(NULL, chann_array[core_id], kCFStringEncodingUTF8);
        CFStringRef chann_name  = IOReportChannelGetChannelName(sample);
        CFStringRef group       = IOReportChannelGetGroup(sample);
        long      value       = IOReportSimpleGetIntegerValue(sample, 0);
      
        if (CFStringCompare(group, CFSTR("Energy Model"), 0) == kCFCompareEqualTo) {
            if (CFStringCompare(chann_name, core_id_str, 0) == kCFCompareEqualTo){
                pwr = ((float)value/(TIME_BETWEEN_MEASUREMENTS));
                return kIOReportIterStop;
            }
        }  
        return kIOReportIterOk;
    });
    return pwr;
}

cpu_freq_data *get_frequency_apple(CFDictionaryRef cpu_delta){
    // Initialize data struct
    cpu_freq_data *data = malloc(sizeof(cpu_freq_data));
    data->core_labels = malloc(num_cores * sizeof(CFStringRef));
    data->frequencies = malloc(num_cores * sizeof(uint64_t));
    data->residencies = malloc(num_cores * sizeof(uint64_t*));
    data->num_dvfs_states = malloc(num_cores * sizeof(uint64_t));

    // Take sample and fill in residency table
    get_state_residencies(cpu_delta, data);
    
    uint64_t sum = 0;
    float freq = 0;

    for (int i = 0; i < num_cores; i++){
        // Sum all 
        uint64_t num_states = data->num_dvfs_states[i];
        uint64_t sum = 0;
        float freq = 0;
        // Hardcoded, change soon
        int table_idx = 1;
        if (num_states < 10){
            table_idx = 0;
        }
        for (int ii = 0; ii < num_states; ii++){
            sum += data->residencies[i][ii];
            //printf("%lld\n", sum);
        }
        // Take average
        for (int ii = 0; ii < num_states; ii++){
            float percent = (float)data->residencies[i][ii]/sum;
            freq += (percent*freq_state_cores[table_idx][ii]);
        }
        // Save to data struct
        data->frequencies[i] = freq;
    }
    return data;
}

int get_core_num(){
    FILE *fp;
    char result[128];

    fp = popen("sysctl -n hw.logicalcpu", "r");
    fgets(result, sizeof(result) - 1, fp);
    pclose(fp);
    int num_cores = atoi(result);
    // Add two extra for complexes
    return num_cores + 2;
}

int main(int argc, char* argv[]) {
    struct unit_data *unit = malloc(sizeof(unit_data));

    // initialize the cmd_data
    init_unit_data();
    sample_deltas *deltas = sample();
    cpu_freq_data *data = get_frequency_apple(deltas->cpu_delta);
    for(int i = 0; i<num_cores; i++){
        //CFShow(data->core_labels[i]);
        //printf("%lld\n", data->residencies[i][0]);
        printf("%lld\n", data->frequencies[i]);
    }
}