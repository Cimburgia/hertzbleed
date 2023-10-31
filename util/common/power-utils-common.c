#include "power-utils-common.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

// Unimplemented
uint32_t get_power_common(int core_ID) {
    // Open sysfs CPU power file once
	static FILE *cur_energy_file = NULL;
	if (!cur_energy_file) {
		char file_name[100];

		sprintf(file_name, "/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj");

		cur_energy_file = fopen(file_name, "r");
		if (cur_energy_file == NULL) {
			perror("Error opening scaling_cur_freq file");
			exit(1);
		}
	}

	// Read current CPU frequency
	uint32_t cur_power = 0;
	if (cur_energy_file) {
		rewind(cur_energy_file);
		fflush(cur_energy_file);
		if (fscanf(cur_energy_file, "%" PRIu32, &cur_power) == 1) {
			return cur_power;
		}
	}

	// Abort if file could not be opened or read
	fprintf(stderr, "Unable to get cpufreq\n");
	fclose(cur_energy_file);
	cur_energy_file = NULL;
	return -1;
}