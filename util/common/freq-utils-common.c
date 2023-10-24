#include "freq-utils-common.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>


/*
 * Get CPU Frequency using linux device driver API
 */
uint32_t get_frequency_common(int cpu_id)
{
	// Open sysfs CPU frequency file once
	static FILE *cur_freq_file = NULL;
	if (!cur_freq_file) {
		char file_name[100];

		// scaling_cur_freq is the file with the current frequency on our system
		sprintf(file_name, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", cpu_id);

		cur_freq_file = fopen(file_name, "r");
		if (cur_freq_file == NULL) {
			perror("Error opening scaling_cur_freq file");
			exit(1);
		}
	}

	// Read current CPU frequency
	uint32_t scaling_cur_freq = 0;
	if (cur_freq_file) {
		rewind(cur_freq_file);
		fflush(cur_freq_file);
		if (fscanf(cur_freq_file, "%" PRIu32, &scaling_cur_freq) == 1) {
			return scaling_cur_freq;
		}
	}

	// Abort if file could not be opened or read
	fprintf(stderr, "Unable to get cpufreq\n");
	fclose(cur_freq_file);
	cur_freq_file = NULL;
	return -1;
}
