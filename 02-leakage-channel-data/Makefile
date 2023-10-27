# Get raw cpuid manufacturer string
CPU_MAN_STR := ${shell lscpu | grep "Vendor ID" | sed 's/^.\{36\}//'}

# Default cpu vendor to intel
VENDOR = _INTEL

# default arch = x86
ARCH = ARCH_X86
#ARCH = ARCH_UNIVERSAL

ifeq ($(CPU_MAN_STR), AuthenticAMD)
	VENDOR = _AMD
endif


CC:= gcc
CFLAGS:= -O3 -D_POSIX_SOURCE -D_GNU_SOURCE -D$(VENDOR) -D$(ARCH) -m64 -falign-functions=64 -Wno-unused-result -Wall
LIBS:= -lpthread -lrt -lm
UTILS:= ../util/util.o ../util/power-utils.o ../util/freq-utils.o \
		../util/common/freq-utils-common.o ../util/common/power-utils-common.o \
		../util/x86/freq-utils-x86.o ../util/x86/msr-utils.o ../util/x86/rapl-utils.o
		
all: obj bin out data driver driver-steady

driver: obj/driver.o $(UTILS)
	$(CC) -o bin/$@ $^ $(LIBS)

driver-steady: obj/driver-steady.o $(UTILS)
	$(CC) -o bin/$@ $^ $(LIBS)

obj/%.o: %.c
	$(CC) -c $(CFLAGS) -DSLEEP=1 -o $@ $<

obj/%-steady.o: %.c
	$(CC) -c $(CFLAGS) -DSLEEP=0 -o $@ $<

obj:
	mkdir -p $@

bin:
	mkdir -p $@

out:
	mkdir -p $@

data:
	mkdir -p $@

clean:
	rm -rf bin obj out
	rm -rf ../util/*.o

.PHONY: all clean
