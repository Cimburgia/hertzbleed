CC:= gcc
CFLAGS:= -O3 -D_POSIX_SOURCE -D_GNU_SOURCE -DARCH_ARM64 -m64 -falign-functions=64 -Wall -g 
LIBS:= -lpthread -lm
FRAMEWORKS:= -lIOReport -framework IOKit -framework CoreFoundation -framework SystemConfiguration
UTILS:= ../util/util.o ../util/freq-utils.o
all: obj bin out data driver

driver: obj/driver.o $(UTILS)
	$(CC) -o bin/$@ $^ $(LIBS) $(FRAMEWORKS)

driver-input: obj/driver.o $(UTILS)
	$(CC) -o bin/$@ $^ $(LIBS) $(FRAMEWORKS)

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
	rm -f a.out

.PHONY: all clean
