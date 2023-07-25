#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#include "../util/util.h"

volatile static int attacker_core_ID;

#define TIME_BETWEEN_MEASUREMENTS 1000000L // 1 millisecond

#define STACK_SIZE 8192

struct args_t {
	uint64_t iters;
	int selector;
};

static __attribute__((noinline)) void *victim(void *varg){
	struct args_t *arg = varg;
	uint64_t my_uint64 = 0x0000FFFFFFFF0000;
	uint64_t count = (uint64_t)arg->selector;

	// shift %1 (my_unit64) left by %0 (count) and store in %register
	asm volatile(
		".align 64\t\n"
		"loop:\n\t"
		
		"lsl %%x1, %1, %0\n\t"
		"lsl %%x2, %1, %0\n\t"
		"lsr %%x3, %1, %0\n\t"
		"lsr %%x4, %1, %0\n\t"
		"lsl %%x5, %1, %0\n\t"
		"lsl %%x6, %1, %0\n\t"
		"lsr %%x7, %1, %0\n\t"
		"lsr %%x8, %1, %0\n\t"
		"lsl %%x9, %1, %0\n\t"
		"lsl %%x10, %1, %0\n\t"

		"lsr %%x1, %1, %0\n\t" 
		"lsr %%x2, %1, %0\n\t"
		"lsl %%x3, %1, %0\n\t"
		"lsl %%x4, %1, %0\n\t"
		"lsr %%x5, %1, %0\n\t"
		"lsr %%x6, %1, %0\n\t"
		"lsl %%x7, %1, %0\n\t"
		"lsl %%x8, %1, %0\n\t"
		"lsr %%x9, %1, %0\n\t"
		"lsr %%x10, %1, %0\n\t"

		"jmp loop\n\t"
		:
		: "r"(count), "r"(my_uint64)
		: "%x1", "%x2", "%x3", "%x4", "%x5", "%x6", "%x7", "%x8", "%x9", "%x10");

	return 0;
}

// Collects traces
static __attribute__((noinline)) void *monitor(void *in){
	static int rept_index = 0;

	struct args_t *arg = (struct args_t *)in;

	// look for how to pin monitor to a single CPU

	// Set filename
	// The format is, e.g., ./out/all_02_2330.out
	// where 02 is the selector and 2330 is an index to prevent overwriting files
	char output_filename[64];
	sprintf(output_filename, "./out/all_%02d_%06d.out", arg->selector, rept_index);
	rept_index += 1;

	// Prepare output file
	FILE *output_file = fopen((char *)output_filename, "w");
	if (output_file == NULL) {
		perror("output file");
	}

	// Prepare initial measurement
	
	// Collect measurements
	for (uint64_t i = 0; i < arg->iters; i++) {
		// Collect samples and wait between each
	}

	// Clean up
	fclose(output_file);
	return 0;
}

int main(int argc, char *argv[]){
	// Check arguments
	if (argc != 4) {
		fprintf(stderr, "Wrong Input! Enter: %s <ntasks> <samples> <outer>\n", argv[0]);
		exit(1);
	}

	// Read in args
	int ntasks;
	struct args_t arg;
	int outer;
	sscanf(argv[1], "%d", &ntasks);
	if (ntasks < 0) {
		fprintf(stderr, "ntasks cannot be negative!\n");
		exit(1);
	}
	sscanf(argv[2], "%" PRIu64, &(arg.iters));
	sscanf(argv[3], "%d", &outer);
	if (outer < 0) {
		fprintf(stderr, "outer cannot be negative!\n");
		exit(1);
	}

	// Open the selector file
	FILE *selectors_file = fopen("input.txt", "r");
	if (selectors_file == NULL)
		perror("fopen error");

	// Read the selectors file line by line
	int num_selectors = 0;
	int selectors[100];

	char *line = NULL;
	while (fgets(line, 0, selectors_file) != NULL) {
		// remove newline in the buffer
		size_t read = strlen(line);
		if (read > 0 && line[read - 1] == '\n') {
			line[read - 1] = '\0';
			--read;
		}

		// Read selector
		sscanf(line, "%d", &(selectors[num_selectors]));
		num_selectors += 1;
	}

	// Set the scheduling priority to high to avoid interruptions
	// (lower priorities cause more favorable scheduling, and -20 is the max)
	setpriority(PRIO_PROCESS, 0, -20);

	// Prepare up monitor/attacker
	attacker_core_ID = 0;
	// Start code to measure CPU

	// Run experiment once for each selector
	for (int i = 0; i < outer * num_selectors; i++) {

		// Set alternating selector
		arg.selector = selectors[i % num_selectors];

		pthread_t threads[ntasks];
		for (int tnum = 0; tnum < ntasks; tnum++) {
			pthread_create(&threads[tnum], NULL, victim, &arg);
		}

		// Start the monitor thread
		pthread_t monitor_thread;
		pthread_create(&monitor_thread, NULL, monitor, &arg);

		// Join monitor thread
		pthread_join(monitor_thread, NULL);

		// Kill victim threads
		for (int tnum = 0; tnum < ntasks; tnum++) {
			pthread_cancel(threads[tnum]);

			// Need to join o/w the threads remain as zombies
			// https://askubuntu.com/a/427222/1552488
			pthread_join(threads[tnum], NULL);
		}
	}
}
