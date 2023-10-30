# Get OS
OS_TYPE := $(shell uname -s)
# Default arch
ARCH := ARCH_X86
# Default cpu vendor to intel
VENDOR = _INTEL

# # Get raw cpuid manufacturer string
ifeq ($(OS_TYPE), Darwin)
	CPU_MAN_STR := $(shell sysctl -n machdep.cpu.brand_string)
else ifeq ($(OS_TYPE), Linux)
	CPU_MAN_STR := ${shell lscpu | grep "Vendor ID" | sed 's/^.\{36\}//'}
endif


# Compiler Flags
CC:= gcc
LIBS:= -lpthread -lm -lc -lrt 
UTILS:= ../util/util.o ../util/power-utils.o ../util/freq-utils.o \
		../util/common/freq-utils-common.o ../util/common/power-utils-common.o \
		../util/x86/freq-utils-x86.o ../util/x86/msr-utils.o ../util/x86/rapl-utils.o

ifeq ($(CPU_MAN_STR), AuthenticAMD)
	VENDOR = _AMD
else ifeq ($(findstring Apple, $(CPU_MAN_STR)), Apple)
	VENDOR := _APPLE
	ARCH := ARCH_ARM64
	LIBS:= -lpthread -lm -lc -lIOReport -framework IOKit -framework CoreFoundation -framework SystemConfiguration
	UTILS:= ../util/util.o ../util/freq-utils.o \
			../util/apple/freq-utils-m.o
endif

CFLAGS:= -O3 -D_POSIX_SOURCE -D_GNU_SOURCE -D$(VENDOR) -D$(ARCH) -m64 -falign-functions=64 -Wno-unused-result -Wall

all: obj bin out data driver

driver: obj/driver.o $(UTILS)
	$(CC) -o bin/$@ $^ $(LIBS)

obj/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

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
	rm -rf ../util/*/*.o

.PHONY: all clean
